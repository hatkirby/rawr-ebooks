#include <string>
#include <map>
#include <list>
#include <vector>

using namespace::std;

#ifndef KGRAMSTATS_H
#define KGRAMSTATS_H

typedef list<string> kgram;

class kgramstats
{
public:
	kgramstats(string corpus, int maxK);
	map<string, int>* lookupExts(kgram tk);
	int getMaxK();
	vector<string> randomSentence(int n);
	
private:
	int maxK;
	map<kgram, map<string, int>* >* stats;
};

void printKgram(kgram k);

#endif