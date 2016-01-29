#include "freevars.h"
#include <fstream>
#include <cstdlib>

freevars::freevars()
{
    vars = new std::map<std::string, std::vector<std::string>* >();
}

void freevars::addVar(std::string name, std::string filename)
{
    std::vector<std::string>* eltlist = new std::vector<std::string>();
    
    std::ifstream infile(filename.c_str());
    if (infile)
    {
        std::string line;
        
        while (getline(infile, line))
        {
            eltlist->push_back(line);
        }
    } else {
        eltlist->push_back("");
    }
    
    (*vars)[name] = eltlist;
}

std::string freevars::parse(std::string in)
{
    std::string res(in);
    
    for (std::map<std::string, std::vector<std::string>* >::iterator it = vars->begin(); it != vars->end(); it++)
    {
        std::string tofind = "$" + it->first + "$";
        size_t fpos;
        while ((fpos = res.find(tofind)) != std::string::npos)
        {
            int r = rand() % it->second->size();
            res.replace(fpos, tofind.length(), (*it->second)[r], 0, std::string::npos);
        }
    }
    
    return res;
}