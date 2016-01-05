#include "kgramstats.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "malaprop.h"

query wildcardQuery(querytype_sentence);

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
    if (token[token.length()-1] == '\n')
    {
      if ((token[token.length()-2] != '.') && (token[token.length()-2] != '!') && (token[token.length()-2] != '?') && (token[token.length()-2] != ','))
      {
        token.insert(token.length()-1, ".");
      }
         
      token.resize(token.length()-1);
    }
       
    if (token.compare("") && token.compare("."))
    {
      mstats.addWord(token);
      tokens.push_back(token);
    }

    start = ((end > (std::string::npos - 1) ) ? std::string::npos : end + 1);
  }
	
  std::map<kgram, std::map<token, token_data> > tstats;
  std::map<token, std::map<termstats, int> > tendings;
  for (int k=1; k<maxK; k++)
  {
    for (int i=0; i<(tokens.size() - k); i++)
    {
      std::list<std::string> seq(tokens.begin()+i, tokens.begin()+i+k);
      kgram prefix;
      
      for (std::list<std::string>::iterator it = seq.begin(); it != seq.end(); it++)
      {
        token word(canonize(*it));
        
        if (it->find_first_of(".?!,") != std::string::npos)
        {
          word.terminating = true;
        }
        
        prefix.push_back(word);
      }
      
      std::string f = tokens[i+k];
		  std::string canonical = canonize(f);
      
      token word(canonical);
      if (f.find_first_of(".?!,") != std::string::npos)
      {
        word.terminating = true;
        
        char terminator = f[f.find_last_of(".?!,")];
        int occurrences = std::count(f.begin(), f.end(), terminator);
        
        tendings[word][termstats(terminator, occurrences)]++;
      }
			
      token_data& td = tstats[prefix][word];
      td.word = word;
      td.all++;

      if (std::find_if(f.begin(), f.end(), ::islower) == f.end())
      {
        td.uppercase++;
      } else if (isupper(f[0]))
      {
        td.titlecase++;
      }
      
      if (prefix.front().word.terminating)
      {
        prefix.front() = wildcardQuery;
        
        token_data& td2 = tstats[prefix][word];
        td2.word = word;
        td2.all++;

        if (std::find_if(f.begin(), f.end(), ::islower) == f.end())
        {
          td2.uppercase++;
        } else if (isupper(f[0]))
        {
          td2.titlecase++;
        }
      }
    }
  }
	
  for (std::map<kgram, std::map<token, token_data> >::iterator it = tstats.begin(); it != tstats.end(); it++)
  {
    kgram klist = it->first;
    std::map<token, token_data>& probtable = it->second;
    std::map<int, token_data>& distribution = stats[klist];
    int max = 0;
		
    for (std::map<token, token_data>::iterator kt = probtable.begin(); kt != probtable.end(); kt++)
    {
      max += kt->second.all;
			
      distribution[max] = kt->second;
    }
  }
  
  for (std::map<token, std::map<termstats, int> >::iterator it = tendings.begin(); it != tendings.end(); it++)
  {
    token word = it->first;
    std::map<termstats, int>& probtable = it->second;
    std::map<int, termstats>& distribution = endings[word];
    int max = 0;
    
    for (std::map<termstats, int>::iterator kt = probtable.begin(); kt != probtable.end(); kt++)
    {
      max += kt->second;
      
      distribution[max] = kt->first;
    }
  }
}

void printKgram(kgram k)
{
  for (kgram::iterator it = k.begin(); it != k.end(); it++)
  {
    query& q = *it;
    if (q.type == querytype_sentence)
    {
      std::cout << "#.# ";
    } else if (q.type == querytype_literal)
    {
      if (q.word.terminating)
      {
        std::cout << q.word.canon << ". ";
      } else {
        std::cout << q.word.canon << " ";
      }
    }
  }
}

// runs in O(n log t) time where n is the input number of sentences and t is the number of tokens in the input corpus
std::vector<std::string> kgramstats::randomSentence(int n)
{
  std::vector<std::string> result;
  kgram cur(1, wildcardQuery);
  int cuts = 0;
	
  for (int i=0; i<n; i++)
  {
    if (cur.size() == maxK)
    {
      cur.pop_front();
    }
    
    if (cur.size() > 0)
    {
      if (rand() % (maxK - cur.size() + 1) == 0)
      {
        while (cur.size() > 2)
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
    
    // Gotta circumvent the last line of the input corpus
    // https://twitter.com/starla4444/status/684222271339237376
    if (stats.count(cur) == 0)
    {
      cur = kgram(1, wildcardQuery);
    }

    std::map<int, token_data>& distribution = stats[cur];
    int max = distribution.rbegin()->first;
    int r = rand() % max;
    token_data& next = distribution.upper_bound(r)->second;
    std::string nextToken(next.word.canon);
    
    bool mess = (rand() % 100) == 0;
    if (mess)
    {
      nextToken = mstats.alternate(nextToken);
    }
    
    // Determine the casing of the next token. We randomly make the token all
    // caps based on the markov chain. Otherwise, we check if the previous
    // token is the end of a sentence (terminating token or a wildcard query).
    int casing = rand() % next.all;
    if (casing < next.uppercase)
    {
      std::transform(nextToken.begin(), nextToken.end(), nextToken.begin(), ::toupper);
    } else if ((((cur.rbegin()->type == querytype_sentence)
          || ((cur.rbegin()->type == querytype_literal)
            && (cur.rbegin()->word.terminating)))
        && (rand() % 2 > 0))
      || (casing - next.uppercase < next.titlecase))
    {
      nextToken[0] = toupper(nextToken[0]);
    }

    if (next.word.terminating)
    {
      std::map<int, termstats>& ending = endings[next.word];
      int emax = ending.rbegin()->first;
      int er = rand() % emax;
      termstats& nextend = ending.upper_bound(er)->second;
      
      nextToken.append(std::string(nextend.occurrences, nextend.terminator));
    }
		
    /* DEBUG */
    printKgram(cur);
		
    std::cout << "-> \"" << nextToken << "\" (" << next.all << "/" << max << ")";
    
    if (mess)
    {
      std::cout << " mala " << next.word.canon;
    }
    
    std::cout << std::endl;

    cur.push_back(next.word);
		
    result.push_back(nextToken);
  }
	
  return result;
}

bool removeIf(char c)
{
  return !((c != '.') && (c != '?') && (c != '!') && (c != ',') /*&& (c != '"') && (c != '(') && (c != ')') && (c != '\n')*/);
}

std::string canonize(std::string f)
{
  std::string canonical(f);
  std::transform(canonical.begin(), canonical.end(), canonical.begin(), ::tolower);
  
  std::string result;
  std::remove_copy_if(canonical.begin(), canonical.end(), std::back_inserter(result), removeIf);
	
  return result;
}
