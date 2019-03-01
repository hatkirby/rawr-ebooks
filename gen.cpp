#include <cstdio>
#include <list>
#include <map>
#include "kgramstats.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <random>

int main(int argc, char** args)
{
  std::random_device randomDevice;
  std::mt19937 rng(randomDevice());
    
  if (argc == 1)
  {
    std::cout << "rawr-gen, version 1.0" << std::endl;
    std::cout << "Usage: rawr-gen corpus-file" << std::endl;
    std::cout << "  where 'corpus-file' is the path to your input" << std::endl;
        
    return 0;
  }
    
  std::ifstream infile(args[1]);
  if (!infile)
  {
    std::cout << "rawr-gen, version 1.0" << std::endl;
    std::cout << "Usage: rawr-gen corpus-file" << std::endl;
    std::cout << "  where 'corpus-file' is the path to your input" << std::endl;
    std::cout << std::endl;
    std::cout << "The file you specified does not exist." << std::endl;
        
    return 0;
  }
    
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
    
  std::cout << "Generating..." << std::endl;
  for (;;)
  {
    std::string doc = kgramstats.randomSentence(140, rng);
    doc.resize(140);

    std::cout << doc << std::endl;
		
    getc(stdin);
  }
	
  return 0;
}
