#include <string>
#include <map>
#include <list>
#include <vector>
#include "histogram.h"

#ifndef KGRAMSTATS_H
#define KGRAMSTATS_H

struct word {
  std::string canon;
  histogram<std::string> forms;
  histogram<std::string> terms;
  
  word(std::string canon) : canon(canon) {}
  
  bool operator<(const word& other) const
  {
    return canon < other.canon;
  }
};

extern word blank_word;

enum class suffixtype {
  none,
  terminating,
  comma
};

enum class parentype {
  paren,
  square_bracket,
  asterisk,
  quote
};

enum class doublestatus {
  opening,
  closing,
  both
};

struct delimiter {
  parentype type;
  doublestatus status;
  
  delimiter(parentype type, doublestatus status) : type(type), status(status) {}
  
  bool operator<(const delimiter& other) const
  {
    return std::tie(type, status) < std::tie(other.type, other.status);
  }
};

struct token {
  const word& w;
  std::map<delimiter, int> delimiters;
  suffixtype suffix;
  std::string raw;
    
  token(const word& w) : w(w), suffix(suffixtype::none) {}
  
  bool operator<(const token& other) const
  {
    return std::tie(w, delimiters, suffix) < std::tie(other.w, other.delimiters, other.suffix);
  }
};

enum class querytype {
  literal,
  sentence
};

struct query {
  querytype type;
  token tok;
  
  query(token tok) : tok(tok), type(querytype::literal) {}
  
  query(querytype type) : tok(blank_word), type(type) {}
  
  bool operator<(const query& other) const
  {
    if (type == other.type)
    {
      return tok < other.tok;
    } else {
      return type < other.type;
    }
  }
};

typedef std::list<query> kgram;

class kgramstats
{
public:
	kgramstats(std::string corpus, int maxK);
	std::string randomSentence(int maxL);
	
private:
	struct token_data
	{
		int all;
		int titlecase;
		int uppercase;
    token tok;
    
    token_data(token tok) : tok(tok), all(0), titlecase(0), uppercase(0) {}
	};
  
	int maxK;
	std::map<kgram, std::map<int, token_data> > stats;
  
  // Words
  std::map<std::string, word> words;
  word hashtags {"#hashtag"};
  word emoticons {"👌"};
};

void printKgram(kgram k);

#endif