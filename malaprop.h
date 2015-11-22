#ifndef MALAPROP_H_8F382336
#define MALAPROP_H_8F382336

#include <string>
#include <map>
#include <set>

class malaprop
{
public:
  void addWord(std::string word);
  void stats();
  std::string alternate(std::string word);
  
private:
  struct soundex {
    char prefix;
    int code;
    
    bool operator<(const soundex& other) const
    {
      return (prefix < other.prefix) || (code < other.code);
    }
  };
  
  std::map<soundex, std::set<std::string> > dict;
  
  soundex soundify(std::string l);
};

#endif /* end of include guard: MALAPROP_H_8F382336 */
