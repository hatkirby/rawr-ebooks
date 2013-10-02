#include "kgramstats.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>

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
	
	stats = new map<kgram, map<string, token_data*>* >();
	for (int k=0; k<=maxK; k++)
	{
		for (int i=0; i<(tokens.size() - k); i++)
		{
			kgram seq(tokens.begin()+i, tokens.begin()+i+k);
			transform(seq.begin(), seq.end(), seq.begin(), canonize);
			string f = tokens[i+k];
			string canonical = canonize(f);
			
			if ((*stats)[seq] == NULL)
			{
				(*stats)[seq] = new map<string, token_data*>();
			}
			
			if ((*(*stats)[seq])[canonical] == NULL)
			{
				(*(*stats)[seq])[canonical] = (token_data*) calloc(1, sizeof(token_data));
			}

			token_data* td = stats->at(seq)->at(canonical);
			td->all++;
			
			if ((f.length() > 0) && (f[f.length()-1] == '.'))
			{
				td->period++;
			}
			
			if (std::find_if(f.begin(), f.end(), ::islower) == f.end())
			{
				td->uppercase++;
			} else if (isupper(f[0]))
			{
				td->titlecase++;
			}
		}
	}
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

		map<string, token_data*>* probtable = (*stats)[cur];
		int max = 0;
		for (map<string, token_data*>::iterator it = probtable->begin(); it != probtable->end(); ++it)
		{
			max += it->second->all;
		}

		int r = rand() % (max+1);
		map<string, token_data*>::iterator next = probtable->begin();
		for (map<string, token_data*>::iterator it = probtable->begin(); it != probtable->end(); ++it)
		{
			if (it->second->all > r)
			{
				break;
			} else {
				next = it;
				r -= it->second->all;
			}
		}

		string nextToken(next->first);
		int casing = rand() % next->second->all;
		int period = rand() % next->second->all;
		if (casing < next->second->uppercase)
		{
			transform(nextToken.begin(), nextToken.end(), nextToken.begin(), ::toupper);
		} else if ((casing - next->second->uppercase) < next->second->titlecase)
		{
			nextToken[0] = toupper(nextToken[0]);
		}
		
		if (period < next->second->period)
		{
			nextToken += ".";
		}
		
		cout << next->first << " | " << nextToken << endl;

		if (cur.size() == maxK)
		{
			cur.pop_front();
		}

		cur.push_back(next->first);
		result.push_back(nextToken);
	}
	
	return result;
}

std::string canonize(std::string f)
{
	string canonical(f);
	transform(canonical.begin(), canonical.end(), canonical.begin(), ::tolower);
	if (canonical[canonical.length()-1] == '.')
	{
		canonical.resize(canonical.find('.'));
	}
	
	return canonical;
}