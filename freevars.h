#include <string>
#include <set>

#ifndef FREEVARS_H
#define FREEVARS_H

class word;

class freevar
{
  public:
    freevar(word& w, std::string file);
    bool check(std::string f) const;
    void add(std::string f);
    word& getWord();
    
  private:
    word& w;
    std::set<std::string> instances;
};

#endif