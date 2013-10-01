#include "kgramstats.h"
#include <vector>
#include <iostream>
#include <cstdlib>

kgramstats::kgramstats(string corpus, int maxK)
{
	this->maxK = maxK;
	
	vector<string> tokens;
    int start = 0;
	int end = 0;

	while (end != string::npos)
	{
	   end = corpus.find(" ", start);

	   tokens.push_back(corpus.substr(start, (end == string::npos) ? string::npos : end - start));

	   start = ((end > (string::npos - 1) ) ? string::npos : end + 1);
	}
	
	stats = new map<kgram, map<string, int>* >();
	for (int k=0; k<=maxK; k++)
	{
		for (int i=0; i<(tokens.size() - k); i++)
		{
			kgram seq(tokens.begin()+i, tokens.begin()+i+k);
			string f = tokens[i+k];
			
			if ((*stats)[seq] == NULL)
			{
				(*stats)[seq] = new map<string, int>();
			}
			
			(*((*stats)[seq]))[f]++;
		}
	}
}

map<string, int>* kgramstats::lookupExts(kgram tk)
{
	return (*stats)[tk];
}

int kgramstats::getMaxK()
{
	return maxK;
}

void printKgram(kgram k)
{
	for (kgram::iterator it = k.begin(); it != k.end(); it++)
	{
		cout << *it << " ";
	}
	cout << endl;
}

vector<string> kgramstats::randomSentence(int n)
{
	vector<string> result;
	list<string> cur;
	
	for (int i=0; i<n; i++)
	{
		if ((rand() % 4) != 0)
		{
			for (int i=0; i<cur.size(); i++)
			{
				if ((rand() % 3) != 0)
				{
					cur.pop_front();
				} else {
					break;
				}
			}
		}
		
		map<string, int>* probtable = lookupExts(cur);
		int max = 0;
		for (map<string, int>::iterator it = probtable->begin(); it != probtable->end(); ++it)
		{
			max += it->second;
		}
		
		int r = rand() % (max+1);
		string next = probtable->begin()->first;
		for (map<string, int>::iterator it = probtable->begin(); it != probtable->end(); ++it)
		{
			if (it->second > r)
			{
				break;
			} else {
				next = it->first;
				r -= it->second;
			}
		}

		if (cur.size() == maxK)
		{
			cur.pop_front();
		}

		cur.push_back(next);
		result.push_back(next);
	}
	
	return result;
}