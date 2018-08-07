#include <cstdio>
#include <list>
#include <map>
#include "kgramstats.h"
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <twitter.h>
#include <yaml-cpp/yaml.h>
#include <thread>
#include <chrono>
#include <algorithm>

const auto QUEUE_TIMEOUT = std::chrono::minutes(1);
const auto POLL_TIMEOUT = std::chrono::minutes(5);

int main(int argc, char** args)
{
  srand(time(NULL));
  rand(); rand(); rand(); rand();

  YAML::Node config = YAML::LoadFile("config.yml");
  int delay = config["delay"].as<int>();

  twitter::auth auth(
    config["consumer_key"].as<std::string>(),
    config["consumer_secret"].as<std::string>(),
    config["access_key"].as<std::string>(),
    config["access_secret"].as<std::string>());

  twitter::client client(auth);

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
      form.replace(pos, 6, fv_names[rand() % fv_names.size()]);
    }

    return form;
  });

  std::list<std::tuple<std::string, bool, twitter::tweet_id>> postQueue;

  auto startedTime = std::chrono::system_clock::now();

  auto queueTimer = std::chrono::system_clock::now();
  auto pollTimer = std::chrono::system_clock::now();
  auto genTimer = std::chrono::system_clock::now();

  for (;;)
  {
    auto currentTime = std::chrono::system_clock::now();

    if (currentTime >= genTimer)
    {
      std::string doc = kgramstats.randomSentence(140);
      doc.resize(140);

      postQueue.emplace_back(std::move(doc), false, 0);

      int genwait = rand() % delay + 1;

      genTimer = currentTime + std::chrono::seconds(genwait);
    }

    if (currentTime >= pollTimer)
    {
      pollTimer = currentTime;

      try
      {
        std::list<twitter::tweet> newTweets =
          client.getMentionsTimeline().poll();

        for (const twitter::tweet& tweet : newTweets)
        {
          auto createdTime =
            std::chrono::system_clock::from_time_t(tweet.getCreatedAt());

          if (
            // Ignore tweets from before the bot started up
            createdTime > startedTime
            // Ignore retweets
            && !tweet.isRetweet()
            // Ignore tweets from yourself
            && tweet.getAuthor() != client.getUser())
          {
            std::string doc = tweet.generateReplyPrefill(client.getUser());
            doc += kgramstats.randomSentence(140 - doc.length());
            doc.resize(140);

            postQueue.emplace_back(std::move(doc), true, tweet.getID());
          }
        }
      } catch (const twitter::rate_limit_exceeded&)
      {
        // Wait out the rate limit (10 minutes here and 5 below = 15).
        pollTimer += std::chrono::minutes(10);
      } catch (const twitter::twitter_error& e)
      {
        std::cout << "Twitter error while polling: " << e.what() << std::endl;
      }

      pollTimer += std::chrono::minutes(POLL_TIMEOUT);
    }

    if ((currentTime >= queueTimer) && (!postQueue.empty()))
    {
      auto post = postQueue.front();
      postQueue.pop_front();

      try
      {
        if (std::get<1>(post))
        {
          client.replyToTweet(std::get<0>(post), std::get<2>(post));
        } else {
          client.updateStatus(std::get<0>(post));
        }
      } catch (const twitter::twitter_error& error)
      {
        std::cout << "Twitter error while tweeting: " << error.what()
          << std::endl;
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
