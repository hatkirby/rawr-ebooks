#include <cstdio>
#include <list>
#include <map>
#include "kgramstats.h"
#include <fstream>
#include <iostream>
#include <mastodonpp/mastodonpp.hpp>
#include <yaml-cpp/yaml.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>
#include "timeline.h"

const auto QUEUE_TIMEOUT = std::chrono::minutes(1);
const auto POLL_TIMEOUT = std::chrono::minutes(5);

int main(int argc, char** args)
{
  std::random_device randomDevice;
  std::mt19937 rng(randomDevice());

  YAML::Node config = YAML::LoadFile("config.yml");
  int delay = config["delay"].as<int>();

  mastodonpp::Instance instance{
    config["mastodon_instance"].as<std::string>(),
    config["mastodon_token"].as<std::string>()};
  mastodonpp::Connection connection{instance};

  timeline mentions(mastodonpp::API::v1::notifications);
  mentions.poll(connection); // just ignore the results

  std::ifstream infile(config["corpus"].as<std::string>().c_str());
  std::string corpus;
  std::string line;
  while (getline(infile, line))
  {
    if (line.back() == '\r')
    {
      line.pop_back();
    }

    corpus += line + "\n";
  }

  // Replace old-style freevars while I can't be bothered to remake the corpus yet
  std::vector<std::string> fv_names;
  std::ifstream namefile("names.txt");
  if (namefile.is_open())
  {
    while (!namefile.eof())
    {
      std::string l;
      getline(namefile, l);
      if (l.back() == '\r')
      {
        l.pop_back();
      }

      fv_names.push_back(l);
    }
  }

  namefile.close();

  std::cout << "Preprocessing corpus..." << std::endl;
  rawr kgramstats;
  kgramstats.addCorpus(corpus);
  kgramstats.compile(5);
  kgramstats.setTransformCallback([&] (std::string, std::string form) {
    size_t pos = form.find("$name$");
    if (pos != std::string::npos)
    {
      int fvInd = std::uniform_int_distribution<int>(0, fv_names.size()-1)(rng);
      form.replace(pos, 6, fv_names[fvInd]);
    }

    return form;
  });

  std::list<std::tuple<std::string, bool, std::string>> postQueue;

  auto startedTime = std::chrono::system_clock::now();

  auto queueTimer = std::chrono::system_clock::now();
  auto pollTimer = std::chrono::system_clock::now();
  auto genTimer = std::chrono::system_clock::now();

  for (;;)
  {
    auto currentTime = std::chrono::system_clock::now();

    if (currentTime >= genTimer)
    {
      std::string doc = kgramstats.randomSentence(140, rng);
      doc.resize(500);

      postQueue.emplace_back(std::move(doc), false, "");

      genTimer = currentTime + std::chrono::seconds(delay);
    }

    if (currentTime >= pollTimer)
    {
      pollTimer = currentTime;

      try
      {
        std::list<nlohmann::json> newNotifs = mentions.poll(connection);

        for (const nlohmann::json& notif : newNotifs)
        {
          if (
            notif["type"].get<std::string>() == "mention"
            // Ignore retweets
            && notif["status"]["reblog"].is_null())
          {
            std::string doc = "@" + notif["status"]["account"]["acct"].get<std::string>() + " ";
            doc += kgramstats.randomSentence(500 - doc.length(), rng);
            doc.resize(500);

            postQueue.emplace_back(std::move(doc), true, notif["status"]["id"]);
          }
        }
      } catch (const std::exception& e)
      {
        std::cout << "Twitter error while polling: " << e.what() << std::endl;
        // Wait out the rate limit (10 minutes here and 5 below = 15).
        pollTimer += std::chrono::minutes(10);
      }

      pollTimer += std::chrono::minutes(POLL_TIMEOUT);
    }

    if ((currentTime >= queueTimer) && (!postQueue.empty()))
    {
      auto [result, isReply, replyTo] = postQueue.front();
      postQueue.pop_front();

      mastodonpp::parametermap parameters{{"status", result}};
      if (isReply) {
        parameters["in_reply_to_id"] = replyTo;
      }

      auto answer{connection.post(mastodonpp::API::v1::statuses, parameters)};
      if (!answer)
      {
        if (answer.curl_error_code == 0)
        {
          std::cout << "HTTP status: " << answer.http_status << std::endl;
        }
        else
        {
          std::cout << "libcurl error " << std::to_string(answer.curl_error_code)
               << ": " << answer.error_message << std::endl;
        }
      }

      queueTimer = currentTime + std::chrono::minutes(QUEUE_TIMEOUT);
    }

    auto soonestTimer = genTimer;

    if (pollTimer < soonestTimer)
    {
      soonestTimer = pollTimer;
    }

    if ((queueTimer < soonestTimer) && (!postQueue.empty()))
    {
      soonestTimer = queueTimer;
    }

    int waitlen =
      std::chrono::duration_cast<std::chrono::seconds>(
        soonestTimer - currentTime).count();

    if (waitlen == 1)
    {
      std::cout << "Sleeping for 1 second..." << std::endl;
    } else if (waitlen < 60)
    {
      std::cout << "Sleeping for " << waitlen << " seconds..." << std::endl;
    } else if (waitlen == 60)
    {
      std::cout << "Sleeping for 1 minute..." << std::endl;
    } else if (waitlen < 60*60)
    {
      std::cout << "Sleeping for " << (waitlen/60) << " minutes..."
        << std::endl;
    } else if (waitlen == 60*60)
    {
      std::cout << "Sleeping for 1 hour..." << std::endl;
    } else if (waitlen < 60*60*24)
    {
      std::cout << "Sleeping for " << (waitlen/60/60) << " hours..."
        << std::endl;
    } else if (waitlen == 60*60*24)
    {
      std::cout << "Sleeping for 1 day..." << std::endl;
    } else {
      std::cout << "Sleeping for " << (waitlen/60/60/24) << " days..."
        << std::endl;
    }

    std::this_thread::sleep_until(soonestTimer);
  }

  return 0;
}
