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
#include <cstring>
#include <algorithm>
#include <set>
#include <stack>
#include "freevars.h"
#include <fstream>
#include "prefix_search.h"
#include <aspell.h>

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
  freevar fv_emoticons {emoticons, "emoticons.txt"};
  std::map<std::string, std::string> canonical_form;
  
  // Ensure the old-style freevars exist
  canonical_form["$name$"] = "$name$";
  words.emplace("$name$", std::string("$name$"));
  canonical_form["$noun$"] = "$noun$";
  words.emplace("$noun$", std::string("$noun$"));
  
  AspellConfig* spell_config = new_aspell_config();
  AspellCanHaveError* possible_err = new_aspell_speller(spell_config);
  if (aspell_error_number(possible_err) != 0)
  {
    std::cout << "aspell error: " << aspell_error_message(possible_err) << std::endl;
    exit(1);
  }
  
  AspellSpeller* spell_checker = to_aspell_speller(possible_err);
  
  std::cout << "Reading emojis..." << std::endl;
  prefix_search emojis;
  std::ifstream emoji_file("emojis.txt");
  if (emoji_file)
  {
    while (!emoji_file.eof())
    {
      std::string rawmojis;
      getline(emoji_file, rawmojis);
      emojis.add(rawmojis);
    }
    
    emoji_file.close();
  }

  std::cout << "Tokenizing corpus..." << std::endl;
  while (end != std::string::npos)
  {
    end = corpus.find(" ", start);

    bool emoji = false;
    std::string te = corpus.substr(start, (end == std::string::npos) ? std::string::npos : end - start);
    std::string t = "";
    
    if (te.compare("") && te.compare("."))
    {
      // Extract strings of emojis into their own tokens even if they're not space delimited
      int m = emojis.match(te);
      emoji = m > 0;
      if (m == 0) m = 1;
      t = te.substr(0,m);
      te = te.substr(m);
      
      while (!te.empty())
      {
        m = emojis.match(te);
        if (emoji == (m > 0))
        {
          if (m == 0) m = 1;
          t += te.substr(0,m);
          te = te.substr(m);
        } else {
          end = start + t.length() - 1;
          break;
        }
      }
      
      std::string tc(t);
      std::transform(tc.begin(), tc.end(), tc.begin(), ::tolower);

      int pst = tc.find_first_not_of("\"([*");
      int dst = tc.find_last_not_of("\")]*.,?!\n");
      std::string canonical("");
      if ((pst != std::string::npos) && (dst != std::string::npos))
      {
        canonical = std::string(tc, pst, dst - pst + 1);
      }
      
      word& w = ([&] () -> word& {
        // Hashtag freevar
        if (canonical[0] == '#')
        {
          thashtags.insert(canonical);
          
          return hashtags;
        }
        
        // Emoticon freevar
        if (emoji)
        {
          emoticons.forms.add(canonical);
          
          return emoticons;
        }
        
        if ((pst != std::string::npos) && (dst != std::string::npos))
        {
          std::string emoticon_canon(t, pst, t.find_last_not_of("\"]*\n.,?!") - pst + 1);
          if (fv_emoticons.check(emoticon_canon))
          {
            emoticons.forms.add(emoticon_canon);
          
            return emoticons;
          }
        }
        
        // Basically any other word
        if (canonical_form.count(canonical) == 0)
        {
          if (canonical.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") == std::string::npos)
          {
            // Words with no letters will be mangled by the spell checker
            canonical_form[canonical] = canonical;
            words.emplace(canonical, canonical);
          } else {
            int correct = aspell_speller_check(spell_checker, canonical.c_str(), canonical.size());
            if (correct)
            {
              words.emplace(canonical, canonical);
              canonical_form[canonical] = canonical;
            } else {
              const AspellWordList* suggestions = aspell_speller_suggest(spell_checker, canonical.c_str(), canonical.size());
              AspellStringEnumeration* elements = aspell_word_list_elements(suggestions);
              const char* replacement = aspell_string_enumeration_next(elements);
              if (replacement != NULL)
              {
                aspell_speller_store_replacement(spell_checker, canonical.c_str(), canonical.size(), replacement, strlen(replacement));
            
                std::string sugrep(replacement);
                canonical_form[canonical] = sugrep;
          
                if (words.count(sugrep) == 0)
                {
                  words.emplace(sugrep, sugrep);
                }
              } else {
                aspell_speller_add_to_session(spell_checker, canonical.c_str(), canonical.size());
            
                words.emplace(canonical, canonical);
                canonical_form[canonical] = canonical;
              }
          
              delete_aspell_string_enumeration(elements);
            }
          }
        }
        
        word& tw = words.at(canonical_form.at(canonical));
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
  
  delete_aspell_speller(spell_checker);
  delete_aspell_config(spell_config);
  
  std::cout << canonical_form.size() << " distinct forms" << std::endl;
  std::cout << words.size() << " distinct words" << std::endl;
  
  // Time to condense the distribution stuff for the words
  std::cout << "Compiling token histograms..." << std::endl;
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
  
  // Compile other freevars
  emoticons.forms.compile();
  emoticons.terms.compile();

  // kgram distribution
  std::cout << "Creating markov chain..." << std::endl;
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
  std::cout << "Compiling kgram distributions..." << std::endl;
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
  bool cut = false;
  std::stack<parentype> open_delimiters;
	
  for (int i=0; i<n; i++)
  {
    if (cur.size() == maxK)
    {
      cur.pop_front();
    }
    
    if ((cur.size() > 0) && cut)
    {
      cur.pop_front();
      cut = false;
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
          } else {
            open_delimiters.pop();
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
    
    if ((next.all == max) && (rand() % 3 == 0))
    {
      cut = true;
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
  
  // Replace old-style freevars while I can't be bothered to remake the corpus yet
  std::vector<std::string> fv_names;
  std::ifstream namefile("names.txt");
  while (!namefile.eof())
  {
    std::string l;
    getline(namefile, l);
    fv_names.push_back(l);
  }
  
  int cpos;
  while ((cpos = result.find("$name$")) != std::string::npos)
  {
    result.replace(cpos, 6, fv_names[rand() % fv_names.size()]);
  }
  
  std::vector<std::string> fv_nouns;
  std::ifstream nounfile("nouns.txt");
  while (!nounfile.eof())
  {
    std::string l;
    getline(nounfile, l);
    fv_nouns.push_back(l);
  }
  
  while ((cpos = result.find("$noun$")) != std::string::npos)
  {
    result.replace(cpos, 6, fv_nouns[rand() % fv_nouns.size()]);
  }
	
  return result;
}
