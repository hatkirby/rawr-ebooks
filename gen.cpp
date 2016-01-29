#include <cstdio>
#include <list>
#include <map>
#include "kgramstats.h"
#include <ctime>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "freevars.h"

int main(int argc, char** args)
{
  srand(time(NULL));
    
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
    corpus += line + "\n ";
  }
	
  std::cout << "Preprocessing corpus..." << std::endl;
  kgramstats* stats = new kgramstats(corpus, 4);
    
  std::cout << "Preprocessing freevars..." << std::endl;
  freevars* vars = new freevars();
  vars->addVar("name", "names.txt");
  vars->addVar("noun", "nouns.txt");
    
  std::cout << "Generating..." << std::endl;
  for (;;)
  {
    std::string doc = stats->randomSentence(rand() % 35 + 15);
    std::string hi = vars->parse(doc);
    hi.resize(140);

    std::cout << hi << std::endl;
		
    getc(stdin);
  }
	
  return 0;
}
