//    _____ _                                             _                      _____     _           _     _ _     _                      _ 
//   |_   _( )                                           | |                    |_   _|   (_)         | |   | (_)   | |                    | |
//     | | |/ _ __ ___     __ _   _ __ ___  _   _ _ __ __| | ___ _ __ ___ _ __    | |      _ _   _ ___| |_  | |_ ___| |_ ___ _ __   ___  __| |
//     | |   | '_ ` _ \   / _` | | '_ ` _ \| | | | '__/ _` |/ _ \ '__/ _ \ '__|   | |     | | | | / __| __| | | / __| __/ _ \ '_ \ / _ \/ _` |
//    _| |_  | | | | | | | (_| | | | | | | | |_| | | | (_| |  __/ | |  __/ |_    _| |_    | | |_| \__ \ |_  | | \__ \ ||  __/ | | |  __/ (_| |
//   |_____| |_| |_| |_|  \__,_| |_| |_| |_|\__,_|_|  \__,_|\___|_|  \___|_(_)  |_____|   | |\__,_|___/\__| |_|_|___/\__\___|_| |_|\___|\__,_|
//    _                                   _  __              _             _   _         _/ |   __             _   _                          
//   | |                                 | |/ _|            (_)           | | (_)       |__/   / _|           | | | |                         
//   | |_ ___    _ __ ___  _   _ ___  ___| | |_   _   _ _ __ _ _ __   __ _| |_ _ _ __   __ _  | |_ ___  _ __  | |_| |__  _ __ ___  ___        
//   | __/ _ \  | '_ ` _ \| | | / __|/ _ \ |  _| | | | | '__| | '_ \ / _` | __| | '_ \ / _` | |  _/ _ \| '__| | __| '_ \| '__/ _ \/ _ \       
//   | || (_) | | | | | | | |_| \__ \  __/ | |   | |_| | |  | | | | | (_| | |_| | | | | (_| | | || (_) | |    | |_| | | | | |  __/  __/       
//    \__\___/  |_| |_| |_|\__, |___/\___|_|_|    \__,_|_|  |_|_| |_|\__,_|\__|_|_| |_|\__, | |_| \___/|_|     \__|_| |_|_|  \___|\___|       
//    _                     __/ |    ______          _      _   _     _         _ _     __/ |               _               _                 
//   | |                   |___/    |  ____|        | |    | | | |   (_)       (_| )   |___/               (_)             | |                
//   | |__   ___  _ __ _   _ ___    | |__ _   _  ___| | __ | |_| |__  _ ___     _|/ _ __ ___     __ _  ___  _ _ __   __ _  | |_ ___           
//   | '_ \ / _ \| '__| | | / __|   |  __| | | |/ __| |/ / | __| '_ \| / __|   | | | '_ ` _ \   / _` |/ _ \| | '_ \ / _` | | __/ _ \          
//   | | | | (_) | |  | |_| \__ \_  | |  | |_| | (__|   <  | |_| | | | \__ \_  | | | | | | | | | (_| | (_) | | | | | (_| | | || (_) |         
//   |_| |_|\___/|_|   \__,_|___(_) |_|   \__,_|\___|_|\_\  \__|_| |_|_|___(_) |_| |_| |_| |_|  \__, |\___/|_|_| |_|\__, |  \__\___/          
//                                                                                               __/ |               __/ |                    
//                   _                                              _ _   _                     |___/               |___/                     
//                  | |                                            (_) | | |                                       | |                        
//    _ __ ___ _ __ | | __ _  ___ ___    ___  __ _  __ _  __      ___| |_| |__     ___  __ _  __ _   _ __ ___ _ __ | | __ _  ___ ___ _ __     
//   | '__/ _ \ '_ \| |/ _` |/ __/ _ \  / _ \/ _` |/ _` | \ \ /\ / / | __| '_ \   / _ \/ _` |/ _` | | '__/ _ \ '_ \| |/ _` |/ __/ _ \ '__|    
//   | | |  __/ |_) | | (_| | (_|  __/ |  __/ (_| | (_| |  \ V  V /| | |_| | | | |  __/ (_| | (_| | | | |  __/ |_) | | (_| | (_|  __/ |       
//   |_|  \___| .__/|_|\__,_|\___\___|  \___|\__, |\__, |   \_/\_/ |_|\__|_| |_|  \___|\__, |\__, | |_|  \___| .__/|_|\__,_|\___\___|_|       
//            | |     _             _ _ _     __/ | __/ | _   _                     _   __/ | __/ |          | |                              
//            |_|    | |           (_) | |   |___/ |___(_) | | |                   | | |___/ |___/           |_|                              
//     __ _ _ __   __| |  _ __ ___  _| | | __ __      ___| |_| |__   __      ____ _| |_                                                       
//    / _` | '_ \ / _` | | '_ ` _ \| | | |/ / \ \ /\ / / | __| '_ \  \ \ /\ / / _` | __|                                                      
//   | (_| | | | | (_| | | | | | | | | |   <   \ V  V /| | |_| | | |  \ V  V / (_| | |_                                                       
//    \__,_|_| |_|\__,_| |_| |_| |_|_|_|_|\_\   \_/\_/ |_|\__|_| |_|   \_/\_/ \__,_|\__|                                                      
//                                                                                                                                            
//  

#include "kgramstats.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <stack>

query wildcardQuery {querytype::sentence};
word blank_word {""};

// runs in O(t^2) time where t is the number of tokens in the input corpus
// We consider maxK to be fairly constant
kgramstats::kgramstats(std::string corpus, int maxK)
{
  this->maxK = maxK;
  
  std::vector<token> tokens;
  size_t start = 0;
  int end = 0;
  std::set<std::string> thashtags;

  while (end != std::string::npos)
  {
    end = corpus.find(" ", start);

    std::string t = corpus.substr(start, (end == std::string::npos) ? std::string::npos : end - start);
    if (t.compare("") && t.compare("."))
    {
      std::string tc(t), canonical;
      std::transform(tc.begin(), tc.end(), tc.begin(), ::tolower);
      std::remove_copy_if(tc.begin(), tc.end(), std::back_inserter(canonical), [=] (char c) {
        return !((c != '.') && (c != '?') && (c != '!') && (c != ',') && (c != '"') && (c != '(') && (c != ')') && (c != '\n') && (c != '[') && (c != ']') && (c != '*'));
      });
      
      word& w = ([&] () -> word& {
        // Hashtag freevar
        if (canonical[0] == '#')
        {
          thashtags.insert(canonical);
          canonical = "#hashtag";
          
          return hashtags;
        }
        
        // Basically any other word
        if (words.count(canonical) == 0)
        {
          words.emplace(canonical, canonical);
        }
        
        word& tw = words.at(canonical);
        tw.forms.add(canonical);
        
        return tw;
      })();
      
      token tk(w);
      tk.raw = t;
      
      for (char c : t)
      {
        if (c == '*')
        {
          tk.delimiters[{parentype::asterisk, doublestatus::opening}]++;
        } else if (c == '[')
        {
          tk.delimiters[{parentype::square_bracket, doublestatus::opening}]++;
        } else if (c == '(')
        {
          tk.delimiters[{parentype::paren, doublestatus::opening}]++;
        } else if (c == '"')
        {
          tk.delimiters[{parentype::quote, doublestatus::opening}]++;
        } else {
          break;
        }
      }
      
      int backtrack = t.find_last_not_of(".,?!])\"*\n") + 1;
      if (backtrack != t.length())
      {
        std::string ending = t.substr(backtrack);
        std::string suffix;
        
        for (char c : ending)
        {
          if ((c == '.') || (c == ',') || (c == '?') || (c == '!'))
          {
            suffix += c;
            
            continue;
          } else if (c == '\n')
          {
            // At least the end is coming
            if (suffix.empty())
            {
              suffix = ".";
            }
            
            break;
          }
          
          parentype pt = ([&] {
            switch (c)
            {
              case ']': return parentype::square_bracket;
              case ')': return parentype::paren;
              case '*': return parentype::asterisk;
              case '"': return parentype::quote;
            }
          })();
          
          if (tk.delimiters[{pt, doublestatus::opening}] > 0)
          {
            tk.delimiters[{pt, doublestatus::opening}]--;
            tk.delimiters[{pt, doublestatus::both}]++;
          } else {
            tk.delimiters[{pt, doublestatus::closing}]++;
          }
        }
        
        if (suffix == ",")
        {
          tk.suffix = suffixtype::comma;
        } else if (!suffix.empty()) {
          tk.suffix = suffixtype::terminating;
          
          w.terms.add(suffix);
        }
      }
      
      tokens.push_back(tk);
    }

    start = ((end > (std::string::npos - 1) ) ? std::string::npos : end + 1);
  }
  
  // Time to condense the distribution stuff for the words
  for (auto& it : words)
  {
    it.second.forms.compile();
    it.second.terms.compile();
  }
  
  // Hashtag freevar is not frequency distributed
  for (auto& it : thashtags)
  {
    hashtags.forms.add(it);
  }
  
  hashtags.forms.compile();
  hashtags.terms.compile();

  // kgram distribution
  std::map<kgram, std::map<token, token_data> > tstats;
  for (int k=1; k<maxK; k++)
  {
    for (int i=0; i<(tokens.size() - k); i++)
    {
      kgram prefix(tokens.begin()+i, tokens.begin()+i+k);
      token f = tokens[i+k];

      if (tstats[prefix].count(f) == 0)
      {
        tstats[prefix].emplace(f, f);
      }
			
      token_data& td = tstats[prefix].at(f);
      td.all++;

      if (std::find_if(f.raw.begin(), f.raw.end(), ::islower) == f.raw.end())
      {
        td.uppercase++;
      } else if (isupper(f.raw[0]))
      {
        td.titlecase++;
      }
      
      kgram term_prefix;
      bool changed = false;
      std::transform(prefix.begin(), prefix.end(), std::back_inserter(term_prefix), [&] (query& q) {
        if (q.tok.suffix == suffixtype::terminating)
        {
          changed = true;
          
          return wildcardQuery;
        } else {
          return q;
        }
      });
      
      if (changed)
      {
        if (tstats[term_prefix].count(f) == 0)
        {
          tstats[term_prefix].emplace(f, f);
        }
        
        token_data& td2 = tstats[term_prefix].at(f);
        td2.all++;

        if (std::find_if(f.raw.begin(), f.raw.end(), ::islower) == f.raw.end())
        {
          td2.uppercase++;
        } else if (isupper(f.raw[0]))
        {
          td2.titlecase++;
        }
      }
    }
  }
	
  // Condense the kgram distribution
  for (auto& it : tstats)
  {
    kgram klist = it.first;
    auto& probtable = it.second;
    auto& distribution = stats[klist];
    int max = 0;
		
    for (auto& kt : probtable)
    {
      max += kt.second.all;
			
      distribution.emplace(max, kt.second);
    }
  }
}

void printKgram(kgram k)
{
  for (auto& q : k)
  {
    if (q.type == querytype::sentence)
    {
      std::cout << "#.# ";
    } else if (q.type == querytype::literal)
    {
      if (q.tok.suffix == suffixtype::terminating)
      {
        std::cout << q.tok.w.canon << ". ";
      } else if (q.tok.suffix == suffixtype::comma)
      {
        std::cout << q.tok.w.canon << ", ";
      } else {
        std::cout << q.tok.w.canon << " ";
      }
    }
  }
}

// runs in O(n log t) time where n is the input number of sentences and t is the number of tokens in the input corpus
std::string kgramstats::randomSentence(int n)
{
  std::string result;
  kgram cur(1, wildcardQuery);
  int cuts = 0;
  std::stack<parentype> open_delimiters;
	
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

    auto& distribution = stats[cur];
    int max = distribution.rbegin()->first;
    int r = rand() % max;
    token_data& next = distribution.upper_bound(r)->second;
    std::string nextToken = next.tok.w.forms.next();
  
    // Determine the casing of the next token. We randomly make the token all
    // caps based on the markov chain. Otherwise, we check if the previous
    // token is the end of a sentence (terminating token or a wildcard query).
    int casing = rand() % next.all;
    if (casing < next.uppercase)
    {
      std::transform(nextToken.begin(), nextToken.end(), nextToken.begin(), ::toupper);
    } else if ((((cur.rbegin()->type == querytype::sentence)
          || ((cur.rbegin()->type == querytype::literal)
            && (cur.rbegin()->tok.suffix == suffixtype::terminating)))
        && (rand() % 2 > 0))
      || (casing - next.uppercase < next.titlecase))
    {
      nextToken[0] = toupper(nextToken[0]);
    }
    
    // Delimiters
    for (auto& dt : next.tok.delimiters)
    {
      if (dt.first.status == doublestatus::both)
      {
        switch (dt.first.type)
        {
          case parentype::paren: nextToken = std::string("(", dt.second) + nextToken + std::string(")", dt.second); break;
          case parentype::square_bracket: nextToken = std::string("[", dt.second) + nextToken + std::string("]", dt.second); break;
          case parentype::asterisk: nextToken = std::string("*", dt.second) + nextToken + std::string("*", dt.second); break;
          case parentype::quote: nextToken = std::string("\"", dt.second) + nextToken + std::string("\"", dt.second); break;
        }
      } else if (dt.first.status == doublestatus::opening)
      {
        for (int i=0; i<dt.second; i++)
        {
          open_delimiters.push(dt.first.type);
        }
        
        switch (dt.first.type)
        {
          case parentype::paren: nextToken = std::string("(", dt.second) + nextToken; break;
          case parentype::square_bracket: nextToken = std::string("[", dt.second) + nextToken; break;
          case parentype::asterisk: nextToken = std::string("*", dt.second) + nextToken; break;
          case parentype::quote: nextToken = std::string("\"", dt.second) + nextToken; break;
        }
      } else if (dt.first.status == doublestatus::closing)
      {
        for (int i=0; i<dt.second; i++)
        {
          while (!open_delimiters.empty() && (open_delimiters.top() != dt.first.type))
          {
            switch (open_delimiters.top())
            {
              case parentype::paren: nextToken.append(")"); break;
              case parentype::square_bracket: nextToken.append("]"); break;
              case parentype::asterisk: nextToken.append("*"); break;
              case parentype::quote: nextToken.append("\""); break;
            }
            
            open_delimiters.pop();
          }
          
          if (open_delimiters.empty())
          {
            switch (dt.first.type)
            {
              case parentype::paren: result = "(" + result; break;
              case parentype::square_bracket: result = "[" + result; break;
              case parentype::asterisk: result = "*" + result; break;
              case parentype::quote: result = "\"" + result; break;
            }
          }
          
          switch (dt.first.type)
          {
            case parentype::paren: nextToken.append(")"); break;
            case parentype::square_bracket: nextToken.append("]"); break;
            case parentype::asterisk: nextToken.append("*"); break;
            case parentype::quote: nextToken.append("\""); break;
          }
        }
      }
    }
    
    // Terminators
    if (next.tok.suffix == suffixtype::terminating)
    {
      nextToken.append(next.tok.w.terms.next());
    } else if (next.tok.suffix == suffixtype::comma)
    {
      nextToken.append(",");
    }
		
    /* DEBUG */
    printKgram(cur);
    std::cout << "-> \"" << nextToken << "\" (" << next.all << "/" << max << ")" << std::endl;

    cur.push_back(next.tok);
		
    result.append(nextToken + " ");
    
    if ((next.tok.suffix == suffixtype::terminating) && (rand() % 4 == 0))
    {
      break;
    }
  }
  
  // Remove the trailing space
  if (result.back() == ' ')
  {
    result.pop_back();
  }
  
  // Close any open delimiters
  while (!open_delimiters.empty())
  {
    switch (open_delimiters.top())
    {
      case parentype::paren: result.append(")"); break;
      case parentype::square_bracket: result.append("]"); break;
      case parentype::asterisk: result.append("*"); break;
      case parentype::quote: result.append("\""); break;
    }
    
    open_delimiters.pop();
  }
	
  return result;
}
