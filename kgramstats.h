#include <string>
#include <map>
#include <list>
#include <vector>
#include "malaprop.h"

#ifndef KGRAMSTATS_H
#define KGRAMSTATS_H

enum tokentype {
  tokentype_literal,
  tokentype_hashtag
};

struct token {
  tokentype type;
  std::string canon;
  bool terminating;
  
  token(std::string canon) : type(tokentype_literal), canon(canon), terminating(false) {}
  token(tokentype type) : type(type), canon(""), terminating(false) {}
  
  bool operator<(const token& other) const
  {
    if (type != other.type)
    {
      return type < other.type;
    } else if (type == tokentype_literal)
    {
      if (canon == other.canon)
      {
        return !terminating && other.terminating;
      } else {
        return canon < other.canon;
      }
    } else {
      return !terminating && other.terminating;
    }
  }
};

enum querytype {
  querytype_literal,
  querytype_sentence
};

struct query {
  querytype type;
  token word;
  
  query(token word) : word(word), type(querytype_literal) {}
  
  query(querytype type) : word(""), type(type) {}
  
  bool operator<(const query& other) const
  {
    if (type == other.type)
    {
      return word < other.word;
    } else {
      return type < other.type;
    }
  }
};

typedef std::list<query> kgram;

struct termstats {
  char terminator;
  int occurrences;
  
  termstats() : terminator('.'), occurrences(1) {}
  
  termstats(char terminator, int occurrences)
  {
    this->terminator = terminator;
    this->occurrences = occurrences;
  }
  
  bool operator<(const termstats& other) const
  {
    if (terminator == other.terminator)
    {
      return occurrences < other.occurrences;
    } else {
      return terminator < other.terminator;
    }
  }
};

class kgramstats
{
public:
	kgramstats(std::string corpus, int maxK);
	std::vector<std::string> randomSentence(int n);
	
private:
	struct token_data
	{
		int all;
		int titlecase;
		int uppercase;
    token word;
    
    token_data() : word(""), all(0), titlecase(0), uppercase(0) {}
	};
  
	int maxK;
	std::map<kgram, std::map<int, token_data> > stats;
  malaprop mstats;
  std::map<token, std::map<int, termstats> > endings;
  std::vector<std::string> hashtags;
};

void printKgram(kgram k);

#endif