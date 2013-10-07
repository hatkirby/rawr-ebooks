#include <cstdio>
#include <list>
#include <map>
#include "kgramstats.h"
#include <ctime>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace::std;

int main(int argc, char** args)
{
	srand(time(NULL));
    
    if (argc == 1)
    {
        cout << "rawr-gen, version 1.0" << endl;
        cout << "Usage: rawr-gen corpus-file" << endl;
        cout << "  where 'corpus-file' is the path to your input" << endl;
        
        return 0;
    }
    
	ifstream infile(args[1]);
    if (!infile)
    {
        cout << "rawr-gen, version 1.0" << endl;
        cout << "Usage: rawr-gen corpus-file" << endl;
        cout << "  where 'corpus-file' is the path to your input" << endl;
        cout << endl;
        cout << "The file you specified does not exist." << endl;
        
        return 0;
    }
    
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