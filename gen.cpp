#include <cstdio>
#include <list>
#include <map>
#include "kgramstats.h"
#include <ctime>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <yaml-cpp/yaml.h>

using namespace::std;

int main(int argc, char** args)
{
	srand(time(NULL));
	
	YAML::Node config = YAML::LoadFile("config.yml");
	
	ifstream infile(config["corpus"].as<std::string>().c_str());
	string corpus;
	string line;
	while (getline(infile, line))
	{
		corpus += " " + line;
	}
	
    cout << "Preprocessing corpus..." << endl;
	kgramstats* stats = new kgramstats(corpus, 5);
    
    cout << "Generating..." << endl;
	for (;;)
	{
		vector<string> doc = stats->randomSentence(rand() % 35 + 15);
		string hi;
		for (vector<string>::iterator it = doc.begin(); it != doc.end(); ++it)
		{
			hi += *it + " ";
		}

		cout << hi << endl;
		
        getc(stdin);
	}
	
	return 0;
}