#include "kgramstats.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>

// runs in O(t^2) time where t is the number of tokens in the input corpus
// We consider maxK to be fairly constant
kgramstats::kgramstats(string corpus, int maxK)
{
	this->maxK = maxK;
	
	vector<string> tokens;
    int start = 0;
	int end = 0;

	while (end != string::npos)
	{
	   end = corpus.find(" ", start);

       string token = corpus.substr(start, (end == string::npos) ? string::npos : end - start);
       if (token.compare(""))
       {
           tokens.push_back(token);
       }

	   start = ((end > (string::npos - 1) ) ? string::npos : end + 1);
	}
	
	map<kgram, map<string, token_data*>* > tstats;
	for (int k=0; k<=maxK; k++)
	{
		for (int i=0; i<(tokens.size() - k); i++)
		{
			kgram seq(tokens.begin()+i, tokens.begin()+i+k);
			transform(seq.begin(), seq.end(), seq.begin(), canonize);
			string f = tokens[i+k];
			string canonical = canonize(f);
			
			if (tstats[seq] == NULL)
			{
				tstats[seq] = new map<string, token_data*>();
			}
			
			if ((*tstats[seq])[canonical] == NULL)
			{
				(*tstats[seq])[canonical] = (token_data*) calloc(1, sizeof(token_data));
			}

			token_data* td = tstats[seq]->at(canonical);
			td->token = new string(canonical);
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
	
	stats = new map<kgram, map<int, token_data*>* >();
	for (map<kgram, map<string, token_data*>* >::iterator it = tstats.begin(); it != tstats.end(); it++)
	{
		kgram klist = it->first;
		map<string, token_data*>* probtable = it->second;
		map<int, token_data*>* distribution = new map<int, token_data*>();
        int max = 0;
		
		for (map<string, token_data*>::iterator kt = probtable->begin(); kt != probtable->end(); kt++)
		{
			max += kt->second->all;
			
			(*distribution)[max] = kt->second;
		}
		
		(*stats)[klist] = distribution;
	}
}

void printKgram(kgram k)
{
	for (kgram::iterator it = k.begin(); it != k.end(); it++)
	{
		cout << *it << " ";
	}
}

// runs in O(n log t) time where n is the input number of sentences and t is the number of tokens in the input corpus
vector<string> kgramstats::randomSentence(int n)
{
	vector<string> result;
	list<string> cur;
	
	for (int i=0; i<n; i++)
	{
		if ((rand() % (maxK - cur.size() + 1)) == 0)
		{
			for (int i=0; i<cur.size(); i++)
			{
				if ((rand() % 3) == 0)
				{
					cur.pop_front();
				} else {
					break;
				}
			}
		}

		map<int, token_data*> distribution = *(*stats)[cur];
		int max = distribution.rbegin()->first;
		int r = rand() % max;
		token_data* next = distribution.upper_bound(r)->second;

		string nextToken(*(next->token));
		int casing = rand() % next->all;
		int period = rand() % next->all;
		if (casing < next->uppercase)
		{
			transform(nextToken.begin(), nextToken.end(), nextToken.begin(), ::toupper);
		} else if ((casing - next->uppercase) < next->titlecase)
		{
			nextToken[0] = toupper(nextToken[0]);
		}
		
		if (period < next->period)
		{
			nextToken += ".";
		}

		if (cur.size() == maxK)
		{
			cur.pop_front();
		}
		
		/* DEBUG */
		for (kgram::iterator it = cur.begin(); it != cur.end(); it++)
		{
			cout << *it << " ";
		}
		
		cout << "-> \"" << nextToken << "\" (" << next->all << "/" << max << ")" << endl;
		
		cur.push_back(*(next->token));
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