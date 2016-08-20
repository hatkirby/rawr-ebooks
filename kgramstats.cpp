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
#include <iostream>
#include <cstring>
#include <algorithm>
#include <set>
#include <stack>
#include "prefix_search.h"
#include <aspell.h>
#include <fstream>

const rawr::query rawr::wildcardQuery = {querytype::sentence};
const rawr::word rawr::blank_word = {""};

void rawr::addCorpus(std::string corpus)
{
  _corpora.push_back(corpus);
}

// runs in O(t^2) time where t is the number of tokens in the input corpus
// We consider maxK to be fairly constant
void rawr::compile(int maxK)
{
  _maxK = maxK;
  
  std::vector<std::vector<token>> tokens;
  std::set<std::string> thashtags;
  std::set<std::string> fv_emoticons;
  
  std::ifstream fvefile("emoticons.txt");
  if (fvefile)
  {
    std::string line;
    while (getline(fvefile, line))
    {
      fv_emoticons.insert(line);
      emoticons.forms.add(line);
    }
  }
  
  fvefile.close();
  
  std::map<std::string, std::string> canonical_form;
  
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
      if (rawmojis.back() == '\r')
      {
        rawmojis.pop_back();
      }
    
      emojis.add(rawmojis);
    }
    
    emoji_file.close();
  }

  std::cout << "Tokenizing corpus...   0%" << std::flush;
  int len = 0;
  for (auto c : _corpora)
  {
    len += c.length();
  }
  
  int startper = 0;
  int per = 0;
  int perprime = 0;
  std::cout.fill(' ');
  for (int i = 0; i < _corpora.size(); i++)
  {
    size_t start = 0;
    int end = 0;
    std::vector<token> tkcor;
    
    while (end != std::string::npos)
    {
      perprime = (startper + end) * 100 / len;
      if (perprime != per)
      {
        per = perprime;
      
        std::cout << "\b\b\b\b" << std::right;
        std::cout.width(3);
        std::cout << per << "%" << std::flush;
      }
    
      end = _corpora[i].find_first_of(" \n", start);

      bool emoji = false;
      std::string te = _corpora[i].substr(start, (end == std::string::npos) ? std::string::npos : end - start + 1);
      std::string t = "";
    
      if (te.compare("") && te.compare(".") && te.compare(" "))
      {
        if (te.back() == ' ')
        {
          te.pop_back();
        }
        
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
        int dst = tc.find_last_not_of("\")]*.,?!\n;:");
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
            if (fv_emoticons.count(emoticon_canon) == 1)
            {
              emoticons.forms.add(emoticon_canon);
          
              return emoticons;
            }
          }
        
          // Basically any other word
          if (canonical_form.count(canonical) == 0)
          {
            if (
              // Legacy freevars should be distinct from tokens containing similar words
              (canonical.find("$name$") != std::string::npos)
              // Words with no letters will be mangled by the spell checker
              || (canonical.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") == std::string::npos)
              )
            {
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
                  std::string sugrep(replacement);
                  canonical_form[canonical] = sugrep;
          
                  if (words.count(sugrep) == 0)
                  {
                    words.emplace(sugrep, sugrep);
                  }
                } else {
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
      
        int backtrack = t.find_last_not_of(".,?!])\"*\n;:") + 1;
        if (backtrack != t.length())
        {
          std::string ending = t.substr(backtrack);
          std::string suffix;
          bool newline = false;
          bool terminating = false;
        
          for (char c : ending)
          {
            if ((c == '.') || (c == ',') || (c == '?') || (c == '!') || (c == ';') || (c == ':'))
            {
              suffix += c;
              terminating = true;
            
              continue;
            } else if (c == '\n')
            {
              newline = true;
              terminating = true;
              
              continue;
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
        
          if (terminating)
          {
            if ((suffix == ",") && (!newline))
            {
              tk.suffix = suffixtype::comma;
            } else {
              tk.suffix = suffixtype::terminating;
              
              w.terms.add({suffix, newline});
            }
          }
        }
      
        tkcor.push_back(tk);
      }

      start = ((end > (std::string::npos - 1) ) ? std::string::npos : end + 1);
    }
    
    tokens.push_back(tkcor);
    
    startper += _corpora[i].length();
  }
  
  std::cout << "\b\b\b\b100%" << std::endl;
  
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
  std::cout << "Creating markov chain...   0%" << std::flush;
  std::map<kgram, std::map<token, token_data> > tstats;

  len = 0;
  for (auto c : tokens)
  {
    len += (maxK-1) * c.size();
  }
  
  startper = 0;
  per = 0;
  perprime = 0;
  int corpid = 0;
  for (auto corpus : tokens)
  {
    for (int k=1; k<maxK; k++)
    {
      for (int i=0; i<(corpus.size() - k); i++)
      {
        perprime = (startper+i) * 100 / len;
        if (perprime != per)
        {
          per = perprime;
      
          std::cout << "\b\b\b\b" << std::right;
          std::cout.width(3);
          std::cout << per << "%" << std::flush;
        }
      
        kgram prefix(corpus.begin()+i, corpus.begin()+i+k);
        token f = corpus[i+k];

        if (tstats[prefix].count(f) == 0)
        {
          tstats[prefix].emplace(f, f);
        }
			
        token_data& td = tstats[prefix].at(f);
        td.all++;
        td.corpora.insert(corpid);

        if (std::find_if(f.raw.begin(), f.raw.end(), ::islower) == f.raw.end())
        {
          td.uppercase++;
        } else if (isupper(f.raw[0]))
        {
          td.titlecase++;
        }
      
        if (std::begin(prefix)->tok.suffix == suffixtype::terminating)
        {
          kgram term_prefix(prefix);
          term_prefix.pop_front();
          term_prefix.push_front(wildcardQuery);
        
          if (tstats[term_prefix].count(f) == 0)
          {
            tstats[term_prefix].emplace(f, f);
          }
        
          token_data& td2 = tstats[term_prefix].at(f);
          td2.all++;
          td2.corpora.insert(corpid);

          if (std::find_if(f.raw.begin(), f.raw.end(), ::islower) == f.raw.end())
          {
            td2.uppercase++;
          } else if (isupper(f.raw[0]))
          {
            td2.titlecase++;
          }
        }
      }
      
      startper += corpus.size();
    }
    
    corpid++;
  }
  
  std::cout << "\b\b\b\b100%" << std::endl;

  // Condense the kgram distribution
  std::cout << "Compiling kgram distributions...   0%";
  len = tstats.size();
  per = 0;
  perprime = 0;
  int indicator = 0;
  for (auto& it : tstats)
  {
    indicator++;
    perprime = indicator * 100 / len;
    if (per != perprime)
    {
      per = perprime;
    
      std::cout << "\b\b\b\b" << std::right;
      std::cout.width(3);
      std::cout << per << "%" << std::flush;
    }
    
    kgram klist = it.first;
    auto& probtable = it.second;
    auto& distribution = _stats[klist];
    int max = 0;
		
    for (auto& kt : probtable)
    {
      max += kt.second.all;
			
      distribution.emplace(max, kt.second);
    }
  }
  
  std::cout << "\b\b\b\b100%" << std::endl;
  
  _compiled = true;
}

std::ostream& operator<<(std::ostream& os, rawr::kgram k)
{
  for (auto& q : k)
  {
    os << q << " ";
  }
  
  return os;
}

std::ostream& operator<<(std::ostream& os, rawr::query q)
{
  if (q.type == rawr::querytype::sentence)
  {
    return os << "#.#";
  } else if (q.type == rawr::querytype::literal)
  {
    return os << q.tok;
  }
  
  return os;
}

std::ostream& operator<<(std::ostream& os, rawr::token t)
{
  os << t.w.canon;
  
  if (t.suffix == rawr::suffixtype::terminating)
  {
    return os << ".";
  } else if (t.suffix == rawr::suffixtype::comma)
  {
    return os << ",";
  } else {
    return os;
  }
}

std::ostream& operator<<(std::ostream& os, rawr::terminator t)
{
  os << t.form;
  
  if (t.newline)
  {
    os << "↵";
  }
  
  return os;
}

void rawr::setTransformCallback(transform_callback _arg)
{
  _transform = _arg;
}

void rawr::setMinCorpora(int _arg)
{
  _min_corpora = _arg;
}

// runs in O(n log t) time where n is the input number of sentences and t is the number of tokens in the input corpus
std::string rawr::randomSentence(int maxL) const
{
  if (!_compiled)
  {
    return "";
  }
  
  std::string result;
  kgram cur(1, wildcardQuery);
  int cuts = 0;
  std::stack<parentype> open_delimiters;
  std::set<int> used_corpora;
	
  for (;;)
  {
    if (cur.size() == _maxK)
    {
      cur.pop_front();
    }
    
    if (cur.size() > 0)
    {
      if (rand() % (_maxK - cur.size() + 1) == 0)
      {
        while ((cur.size() > 2) && (cuts > 0))
        {
          if ((rand() % cuts) > 2)
          {
            cur.pop_front();
            cuts--;
          } else {
            break;
          }
        }
      }
    }
    
    // Gotta circumvent the last line of the input corpus
    // https://twitter.com/starla4444/status/684222271339237376
    if (_stats.count(cur) == 0)
    {
      cur = kgram(1, wildcardQuery);
    }

    auto& distribution = _stats.at(cur);
    int max = distribution.rbegin()->first;
    int r = rand() % max;
    const token_data& next = distribution.upper_bound(r)->second;
    std::string nextToken = next.tok.w.forms.next();
    
    // Apply user-specified transforms
    if (_transform)
    {
      nextToken = _transform(next.tok.w.canon, nextToken);
    }
  
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
      auto term = next.tok.w.terms.next();
      nextToken.append(term.form);
      
      if (term.newline)
      {
        nextToken.append("\n");
      } else {
        nextToken.append(" ");
      }
    } else if (next.tok.suffix == suffixtype::comma)
    {
      nextToken.append(", ");
    } else {
      nextToken.append(" ");
    }
    
    // If this pick was guaranteed, increase cut chance
    if (next.all == max)
    {
      cuts++;
    }
    
    if (next.corpora.size() == 1)
    {
      used_corpora.insert(*next.corpora.begin());
    }
		
    /* DEBUG */
    std::cout << cur << "-> \"" << nextToken << "\" (" << next.all << "/" << max << ")" << " in corp";
    for (auto cor : next.corpora)
    {
      std::cout << " " << cor;
    }
    std::cout << std::endl;

    cur.push_back(next.tok);
    result.append(nextToken);
        
    if ((next.tok.suffix == suffixtype::terminating) && ((result.length() > maxL) || (rand() % 4 == 0)))
    {
      break;
    }
  }
  
  // Ensure that enough corpora are used
  if (used_corpora.size() < _min_corpora)
  {
    return randomSentence(maxL);
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
