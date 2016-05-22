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

int main(int argc, char** args)
{
  srand(time(NULL));
  rand(); rand(); rand(); rand();
	
  YAML::Node config = YAML::LoadFile("config.yml");
  int delay = config["delay"].as<int>();
  
  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());
  
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
    
    corpus += line + "\n ";
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
  kgramstats.compile(4);
  kgramstats.setTransformCallback([&] (std::string canonical, std::string) {
    size_t pos = canonical.find("$name$");
    if (pos != std::string::npos)
    {
      canonical.replace(pos, 6, fv_names[rand() % fv_names.size()]);
    }
    
    return canonical;
  });
  
  std::mutex stats_mutex;
  
  client.setUserStreamNotifyCallback([&] (twitter::notification n) {
    if (n.getType() == twitter::notification::type::tweet)
    {
      if ((!n.getTweet().isRetweet()) && (n.getTweet().getAuthor() != client.getUser()))
      {
        std::string original = n.getTweet().getText();
        std::string canonical;
        std::transform(std::begin(original), std::end(original), std::back_inserter(canonical), [] (char ch) {
          return std::tolower(ch);
        });
      
        if (canonical.find("@rawr_ebooks") != std::string::npos)
        {
          std::string doc = client.generateReplyPrefill(n.getTweet());
          {
            std::lock_guard<std::mutex> stats_lock(stats_mutex);
            doc += kgramstats.randomSentence(140 - doc.length());
            doc.resize(140);
          }
        
          twitter::tweet tw;
          twitter::response resp = client.updateStatus(doc, tw, n.getTweet());
          if (resp != twitter::response::ok)
          {
            std::cout << "Twitter error while tweeting: " << resp << std::endl;
          }
        }
      }
    }
  });
  
  client.startUserStream();
  std::this_thread::sleep_for(std::chrono::minutes(1));

  std::cout << "Generating..." << std::endl;
  for (;;)
  {
    std::string doc;
    {
      std::lock_guard<std::mutex> stats_lock(stats_mutex);
      doc = kgramstats.randomSentence(140);
    }
    doc.resize(140);
    
    twitter::tweet tw;
    twitter::response resp = client.updateStatus(doc, tw);
    if (resp != twitter::response::ok)
    {
      std::cout << "Twitter error while tweeting: " << resp << std::endl;
    }
    
    int waitlen = rand() % delay;
    if (waitlen == 0)
    {
      continue;
    } else if (waitlen == 1)
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
      std::cout << "Sleeping for " << (waitlen/60) << " minutes..." << std::endl;
    } else if (waitlen == 60*60)
    {
      std::cout << "Sleeping for 1 hour..." << std::endl;
    } else if (waitlen < 60*60*24)
    {
      std::cout << "Sleeping for " << (waitlen/60/60) << " hours..." << std::endl;
    } else if (waitlen == 60*60*24)
    {
      std::cout << "Sleeping for 1 day..." << std::endl;
    } else {
      std::cout << "Sleeping for " << (waitlen/60/60/24) << " days..." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(waitlen));
  }
	
  return 0;
}
