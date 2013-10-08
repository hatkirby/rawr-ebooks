#include "freevars.h"
#include <fstream>

freevars::freevars()
{
    vars = new map<string, vector<string>* >();
}

void freevars::addVar(string name, string filename)
{
    vector<string>* eltlist = new vector<string>();
    
    ifstream infile(filename.c_str());
    if (infile)
    {
        string line;
        
        while (getline(infile, line))
        {
            eltlist->push_back(line);
        }
    } else {
        eltlist->push_back("");
    }
    
    (*vars)[name] = eltlist;
}

string freevars::parse(string in)
{
    string res(in);
    
    for (map<string, vector<string>* >::iterator it = vars->begin(); it != vars->end(); it++)
    {
        string tofind = "$" + it->first + "$";
        size_t fpos = res.find(tofind);
        if (fpos != string::npos)
        {
            int r = rand() % it->second->size();
            res.replace(fpos, tofind.length(), (*it->second)[r], 0, string::npos);
        }
    }
    
    return res;
}