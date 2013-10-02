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
	vector<string> randomSentence(int n);
	
private:
	typedef struct
	{
		int all;
		int titlecase;
		int uppercase;
		int period;
	} token_data;
	int maxK;
	map<kgram, map<string, token_data*>* >* stats;
};

void printKgram(kgram k);
std::string canonize(std::string f);

#endif