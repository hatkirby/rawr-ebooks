#include "malaprop.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cstdio>

bool removeIfM(char c)
{
  return !isalpha(c);
}

char soundID(char l)
{
  switch (l)
  {
    case 'b':
    case 'f':
    case 'p':
    case 'v':
      return '1';
      
    case 'c':
    case 'g':
    case 'j':
    case 'k':
    case 'q':
    case 's':
    case 'x':
    case 'z':
      return '2';
      
    case 'd':
    case 't':
      return '3';
      
    case 'l':
      return '4';
      
    case 'm':
    case 'n':
      return '5';
      
    case 'r':
      return '6';
  }
  
  return l;
}

std::string canonizetwo(std::string f)
{
	std::string canonical(f);
	std::transform(canonical.begin(), canonical.end(), canonical.begin(), ::tolower);
  
  std::string result;
  std::remove_copy_if(canonical.begin(), canonical.end(), std::back_inserter(result), removeIfM);
  
  return result;
}

malaprop::soundex malaprop::soundify(std::string f)
{
	std::string result(canonizetwo(f));
  
  soundex ex;
  ex.prefix = result[0];
  
  std::string output;
  
  for (int i = 1; i<result.length(); i++)
  {
    int c = soundID(result[i]);
    if (
      (isdigit(c)) // Not a vowel
      && (c != soundID(result[i-1])) // Not the same as the previous character
      && ((i < 2) || ((result[i-1] = 'h' || result[i-1] == 'w') && (c != soundID(result[i-2])))) // Not same as before h/w
        )
    {
      output += c;
    }
  }
  
  output.resize(3, '0');
  ex.code = atoi(output.c_str());
	
	return ex;
}

void malaprop::addWord(std::string word)
{
  soundex ex = soundify(word);

  if (ex.prefix != 0)
  {  
    dict[ex].insert(canonizetwo(word));
  }
}

void malaprop::stats()
{
  for (std::map<soundex, std::set<std::string> >::iterator it = dict.begin(); it != dict.end(); it++)
  {
    printf("%c%03d (%d): ", it->first.prefix, it->first.code, it->second.size());
    
    for (std::set<std::string>::iterator jt = it->second.begin(); jt != it->second.end(); jt++)
    {
      std::cout << *jt << ", ";
    }
    
    std::cout << std::endl;
  }
  
  exit(0);
}

std::string malaprop::alternate(std::string word)
{
  soundex ex = soundify(word);
  std::set<std::string>& opts = dict[ex];
  if (opts.size() == 0)
  {
    return word;
  }
  
  int opt = rand() % opts.size();
  for (std::set<std::string>::iterator it = opts.begin(); it != opts.end(); it++)
  {
    if (opt == 0)
    {
      return *it;
    }
    
    opt--;
  }
  
  return word;
}
