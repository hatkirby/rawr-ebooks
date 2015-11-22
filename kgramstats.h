#include <string>
#include <map>
#include <list>
#include <vector>
#include "malaprop.h"

#ifndef KGRAMSTATS_H
#define KGRAMSTATS_H

typedef std::list<std::string> kgram;

class kgramstats
{
public:
	kgramstats(std::string corpus, int maxK);
	std::vector<std::string> randomSentence(int n);
	
private:
	typedef struct
	{
		int all;
		int titlecase;
		int uppercase;
		int period;
    int startquote;
    int endquote;
    int startparen;
    int endparen;
    int comma;
		std::string* token;
	} token_data;
	int maxK;
	std::map<kgram, std::map<int, token_data*>* >* stats;
  malaprop mstats;
};

void printKgram(kgram k);

#endif