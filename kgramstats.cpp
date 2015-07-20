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
  bool newSentence = true;
  bool newClause = false;
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
      
      if (newSentence)
      {
        kgram newKgram(1, ".");
        if (tstats[newKgram] == NULL)
        {
          tstats[newKgram] = new map<string, token_data*>();
        }
        
        (*tstats[newKgram])[canonical] = td;
        
        newSentence = false;
      }
      
      if (newClause)
      {
        kgram commaKgram(1, ",");
        if (tstats[commaKgram] == NULL)
        {
          tstats[commaKgram] = new map<string, token_data*>();
        }
        
        (*tstats[commaKgram])[canonical] = td;
        
        newClause = false;
      }
			
			if ((f.length() > 0) && (f[f.length()-1] == '.'))
			{
				td->period++;
        newSentence = true;
			}
      
      if (f.length() > 0)
      {
        if (f[0] == '"')
        {
          td->startquote++;
        }
        
        if (f[f.length()-1] == '"')
        {
          td->endquote++;
          
          if ((f.length() > 1) && (f[f.length()-2] == ','))
          {
            td->comma++;
            newClause = true;
          }
        }
        
        if (f[f.length()-1] == ',')
        {
          td->comma++;
          newClause = true;
          
          if ((f.length() > 1) && (f[f.length()-2] == '"'))
          {
            td->endquote++;
          }
          
          if ((f.length() > 1) && (f[f.length()-2] == ')'))
          {
            td->endparen++;
          }
        }
        
        if (f[0] == '(')
        {
          td->startparen++;
        }
        
        if (f[f.length()-1] == ')')
        {
          td->endparen++;
        }
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
  kgram newKgram(1, ".");
  kgram commaKgram(1, ",");
	list<string> cur = newKgram;
  int cuts = 0;
	
	for (int i=0; i<n; i++)
	{
    if ((cur.size() > 0) && (cur != newKgram))
    {
      if (rand() % (maxK - cur.size() + 1) == 0)
      {
        while (cur.size() > 0)
        {
          if ((rand() % (n)) < cuts)
          {
            cur.pop_front();
            cuts--;
          } else {
            break;
          }
        }
      }
      
      cuts++;
    }

		map<int, token_data*> distribution = *(*stats)[cur];
		int max = distribution.rbegin()->first;
		int r = rand() % max;
		token_data* next = distribution.upper_bound(r)->second;

		string nextToken(*(next->token));
		int casing = rand() % next->all;
		int period = rand() % next->all;
    int startparen = rand() % next->all;
    int endparen = rand() % next->all;
    int startquote = rand() % next->all;
    int endquote = rand() % next->all;
    int comma = rand() % next->all;
		if (casing < next->uppercase)
		{
			transform(nextToken.begin(), nextToken.end(), nextToken.begin(), ::toupper);
		} else if ((casing - next->uppercase) < next->titlecase)
		{
			nextToken[0] = toupper(nextToken[0]);
		}
    
    if ((cur == newKgram) && (rand() % 3 < 2))
    {
      nextToken[0] = toupper(nextToken[0]);
    }
    
    if (startquote < next->startquote)
    {
      nextToken = "\"" + nextToken;
    } else if (startparen < next->startparen)
    {
      nextToken = "(" + nextToken;
    }
		
		if (period < next->period)
		{
      if (endquote < next->endquote)
      {
        nextToken += "\"";
      } else if (endparen < next->endparen)
      {
        nextToken += ")";
      }
      
      int type = rand() % 6;
      
      if (type < 3)
      {
        nextToken += ".";
      } else if (type < 5)
      {
        nextToken += "!";
      } else {
        nextToken += "?";
      }
		} else if (comma < next->comma)
    {
      if (endquote < next->endquote)
      {
        nextToken += "\"";
      } else if (endparen < next->endparen)
      {
        nextToken += ")";
      }
      
      nextToken += ",";
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
    
    if ((cur == newKgram) || (cur == commaKgram))
    {
      cur.pop_front();
    }
		
    if ((period < next->period) && ((rand() % 2) == 0))
    {
      cur = newKgram;
    } else if ((comma < next->comma) && ((rand() % 3) == 0))
    {
      cur = commaKgram;
    } else {
      cur.push_back(*(next->token));
    }
		
		result.push_back(nextToken);
	}
	
	return result;
}

std::string canonize(std::string f)
{
	string canonical(f);
	transform(canonical.begin(), canonical.end(), canonical.begin(), ::tolower);
  
  string result;
  remove_copy_if(canonical.begin(), canonical.end(), std::back_inserter(result), [] (char c) {
    return !((c != '.') && (c != '"') && (c != '(') && (c != ')') && (c != ','));
  });
	
	return result;
}