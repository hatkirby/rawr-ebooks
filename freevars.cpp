#include "freevars.h"
#include <fstream>
#include "kgramstats.h"

freevar::freevar(word& w, std::string file) : w(w)
{
  std::ifstream infile(file);
  if (infile)
  {
    std::string line;
    while (getline(infile, line))
    {
      instances.insert(line);
      w.forms.add(line);
    }
  }
}

bool freevar::check(std::string f) const
{
  return (instances.count(f) == 1);
}

void freevar::add(std::string f)
{
  instances.insert(f);
}

word& freevar::getWord()
{
  return w;
}
