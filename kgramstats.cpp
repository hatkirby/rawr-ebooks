#include "kgramstats.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "malaprop.h"

std::string canonize(std::string f);

// runs in O(t^2) time where t is the number of tokens in the input corpus
// We consider maxK to be fairly constant
kgramstats::kgramstats(std::string corpus, int maxK)
{
	this->maxK = maxK;
  
  std::vector<std::string> tokens;
    size_t start = 0;
	int end = 0;

	while (end != std::string::npos)
	{
	   end = corpus.find(" ", start);

       std::string token = corpus.substr(start, (end == std::string::npos) ? std::string::npos : end - start);
       if (token.compare(""))
       {
         mstats.addWord(token);
           tokens.push_back(token);
       }

	   start = ((end > (std::string::npos - 1) ) ? std::string::npos : end + 1);
	}
	
	std::map<kgram, std::map<std::string, token_data*>* > tstats;
  bool newSentence = true;
  bool newClause = false;
	for (int k=0; k<=maxK; k++)
	{
		for (int i=0; i<(tokens.size() - k); i++)
		{
			kgram seq(tokens.begin()+i, tokens.begin()+i+k);
			std::transform(seq.begin(), seq.end(), seq.begin(), canonize);
			std::string f = tokens[i+k];
			std::string canonical = canonize(f);
			
			if (tstats[seq] == NULL)
			{
				tstats[seq] = new std::map<std::string, token_data*>();
			}
			
			if ((*tstats[seq])[canonical] == NULL)
			{
				(*tstats[seq])[canonical] = (token_data*) calloc(1, sizeof(token_data));
			}

			token_data* td = tstats[seq]->at(canonical);
			td->token = new std::string(canonical);
			td->all++;
      
      if (newSentence)
      {
        kgram newKgram(1, ".");
        if (tstats[newKgram] == NULL)
        {
          tstats[newKgram] = new std::map<std::string, token_data*>();
        }
        
        (*tstats[newKgram])[canonical] = td;
        
        newSentence = false;
      }
      
      if (newClause || newSentence)
      {
        kgram commaKgram(1, ",");
        if (tstats[commaKgram] == NULL)
        {
          tstats[commaKgram] = new std::map<std::string, token_data*>();
        }
        
        (*tstats[commaKgram])[canonical] = td;
        
        newClause = false;
      }
      
      if ((f.length() > 0) && (f[f.length()-1] == '\n'))
      {
        td->period++;
        newSentence = true;
        f.resize(f.length()-1);
      }
			
      if (f.length() > 0)
      {
  			if ((f[f.length()-1] == '.') || (f[f.length()-1] == '!') || (f[f.length()-1] == '?'))
  			{
          if (!newSentence)
          {
    				td->period++;
            newSentence = true;
          }
				
          f.resize(f.length()-1);
  			} else if (f[f.length()-1] == ',')
        {
          if (!newSentence)
          {
            td->comma++;
            newClause = true;
          }
          
          f.resize(f.length()-1);
        }
      }
      
      if (f.length() > 0)
      {
        if (f[0] == '"')
        {
          td->startquote++;
        }
        
        if (f[0] == '(')
        {
          td->startparen++;
        }
        
        if ((f[f.length()-1] == '"') || (f[f.length()-1] == ')'))
        {
          if (f[f.length()-1] == '"')
          {
            td->endquote++;
          } else if (f[f.length()-1] == ')')
          {
            td->endparen++;
          }
          
          f.resize(f.length()-1);
          
          if (f.length() > 0)
          {
      			if ((f[f.length()-1] == '.') || (f[f.length()-1] == '!') || (f[f.length()-1] == '?'))
      			{
              if (!newSentence)
              {
        				td->period++;
                newSentence = true;
              }
      			} else if (f[f.length()-1] == ',')
            {
              if (!newSentence && !newClause)
              {
                td->comma++;
                newClause = true;
              }
            }
          }
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
	
	stats = new std::map<kgram, std::map<int, token_data*>* >();
	for (std::map<kgram, std::map<std::string, token_data*>* >::iterator it = tstats.begin(); it != tstats.end(); it++)
	{
		kgram klist = it->first;
		std::map<std::string, token_data*>* probtable = it->second;
		std::map<int, token_data*>* distribution = new std::map<int, token_data*>();
        int max = 0;
		
		for (std::map<std::string, token_data*>::iterator kt = probtable->begin(); kt != probtable->end(); kt++)
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
		std::cout << *it << " ";
	}
}

// runs in O(n log t) time where n is the input number of sentences and t is the number of tokens in the input corpus
std::vector<std::string> kgramstats::randomSentence(int n)
{
	std::vector<std::string> result;
  kgram newKgram(1, ".");
  kgram commaKgram(1, ",");
	std::list<std::string> cur = newKgram;
  int cuts = 0;
	
	for (int i=0; i<n; i++)
	{
    if ((cur.size() > 0) && (cur != newKgram))
    {
      if (rand() % (maxK - cur.size() + 1) == 0)
      {
        while (cur.size() > 1)
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

		std::map<int, token_data*> distribution = *(*stats)[cur];
		int max = distribution.rbegin()->first;
		int r = rand() % max;
		token_data* next = distribution.upper_bound(r)->second;

		std::string nextToken(*(next->token));
		int casing = rand() % next->all;
		int period = rand() % next->all;
    int startparen = rand() % next->all;
    int endparen = rand() % next->all;
    int startquote = rand() % next->all;
    int endquote = rand() % next->all;
    int comma = rand() % next->all;
		if (casing < next->uppercase)
		{
			std::transform(nextToken.begin(), nextToken.end(), nextToken.begin(), ::toupper);
		} else if ((casing - next->uppercase) < next->titlecase)
		{
			nextToken[0] = toupper(nextToken[0]);
		}
    
    if ((cur == newKgram) && (rand() % 3 < 2))
    {
      nextToken[0] = toupper(nextToken[0]);
    }
    
    bool mess = (rand() % 100) == 0;
    if (mess)
    {
      nextToken = mstats.alternate(nextToken);

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
    }
    
		if (cur.size() == maxK)
		{
			cur.pop_front();
		}
		
		/* DEBUG */
		for (kgram::iterator it = cur.begin(); it != cur.end(); it++)
		{
			std::cout << *it << " ";
		}
		
		std::cout << "-> \"" << nextToken << "\" (" << next->all << "/" << max << ")";
    
    if (mess)
    {
      std::cout << " mala " << *(next->token);
    }
    
    std::cout << std::endl;
    
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
      //if (mess && (rand() % 2 == 0))
      if (false)
      {
        // This doesn't work because sometimes the alternate token isn't actually present in the original corpus
        cur.clear();
        cur.push_back(nextToken);
      } else {
        cur.push_back(*(next->token));
      }
    }
		
		result.push_back(nextToken);
	}
	
	return result;
}

bool removeIf(char c)
{
  return !((c != '.') && (c != '?') && (c != '!') && (c != '"') && (c != '(') && (c != ')') && (c != ','));
}

std::string canonize(std::string f)
{
	std::string canonical(f);
	std::transform(canonical.begin(), canonical.end(), canonical.begin(), ::tolower);
  
  std::string result;
  std::remove_copy_if(canonical.begin(), canonical.end(), std::back_inserter(result), removeIf);
	
	return canonical;
}
