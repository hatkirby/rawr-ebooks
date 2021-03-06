#ifndef KGRAMSTATS_H
#define KGRAMSTATS_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include "histogram.h"
#include "identifier.h"
#include <functional>
#include <set>
#include <random>

class rawr {
  public:
    typedef std::function<std::string(std::string, std::string)> transform_callback;
    
    void addCorpus(std::string corpus);
    void compile(int maxK);
    
    void setTransformCallback(transform_callback _arg);
    void setMinCorpora(int _arg);
  	std::string randomSentence(int maxL, std::mt19937& rng) const;
	
  private:
    struct terminator {
      std::string form;
      bool newline = false;
      
      terminator(std::string form, bool newline) : form(form), newline(newline) {}
      
      bool operator<(const terminator& other) const
      {
        return std::tie(form, newline) < std::tie(other.form, other.newline);
      }
    };
    
    struct word {
      std::string canon;
      histogram<std::string> forms;
      histogram<terminator> terms;
  
      word(std::string canon) : canon(canon) {}
  
      bool operator<(const word& other) const
      {
        return canon < other.canon;
      }
    };

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
      bool newline = false;
    
      token(const word& w) : w(w), suffix(suffixtype::none) {}
  
      bool operator<(const token& other) const
      {
        return std::tie(w, delimiters, suffix) < std::tie(other.w, other.delimiters, other.suffix);
      }
    };

    using tokenstore = identifier<token>;
    using token_id = tokenstore::key_type;

    enum class querytype {
      literal,
      sentence
    };

    struct query {
      querytype type;
      token_id tok;

      query(token_id tok) : tok(tok), type(querytype::literal) {}

      query(querytype type) : tok(0), type(type) {}

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
    
    static const query wildcardQuery;
    static const word blank_word;

    typedef std::list<query> kgram;
    
  	struct token_data
  	{
  		int all;
  		int titlecase;
  		int uppercase;
      token_id tok;
      std::set<int> corpora;

      token_data(token_id tok) : tok(tok), all(0), titlecase(0), uppercase(0) {}
  	};
    
    friend std::ostream& operator<<(std::ostream& os, kgram k);
    friend std::ostream& operator<<(std::ostream& os, query q);
    friend std::ostream& operator<<(std::ostream& os, token t);
    friend std::ostream& operator<<(std::ostream& os, terminator t);
  
  	int _maxK;
    bool _compiled = false; 
    std::vector<std::string> _corpora;
    tokenstore _tokenstore;
  	std::map<kgram, std::map<int, token_data>> _stats;
    transform_callback _transform;
    int _min_corpora = 1;
  
    // Words
    std::map<std::string, word> words;
    word hashtags {"#hashtag"};
    word emoticons {"👌"};
};

#endif