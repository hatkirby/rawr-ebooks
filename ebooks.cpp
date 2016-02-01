#include <cstdio>
#include <list>
#include <map>
#include "kgramstats.h"
#include <ctime>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <twitcurl.h>
#include <unistd.h>
#include <yaml-cpp/yaml.h>

int main(int argc, char** args)
{
  srand(time(NULL));
	
  YAML::Node config = YAML::LoadFile("config.yml");
  int delay = config["delay"].as<int>();

  std::ifstream infile(config["corpus"].as<std::string>().c_str());
  std::string corpus;
  std::string line;
  while (getline(infile, line))
  {
    corpus += line + "\n ";
  }

  std::cout << "Preprocessing corpus..." << std::endl;
  kgramstats* stats = new kgramstats(corpus, 4);
  
  twitCurl twitter;
  twitter.getOAuth().setConsumerKey(config["consumer_key"].as<std::string>());
  twitter.getOAuth().setConsumerSecret(config["consumer_secret"].as<std::string>());
  twitter.getOAuth().setOAuthTokenKey(config["access_key"].as<std::string>());
  twitter.getOAuth().setOAuthTokenSecret(config["access_secret"].as<std::string>());

  std::cout << "Generating..." << std::endl;
  for (;;)
  {
    std::string doc = stats->randomSentence(rand() % 45 + 5);
    std::string hi = doc;
    hi.resize(140);
    
    std::string replyMsg;
    if (twitter.statusUpdate(hi))
    {
      twitter.getLastWebResponse(replyMsg);
      std::cout << "Twitter message: " << replyMsg << std::endl;
    } else {
      twitter.getLastCurlError(replyMsg);
      std::cout << "Curl error: " << replyMsg << std::endl;
    }

    sleep(rand() % delay);
  }
	
  return 0;
}
