#include <map>
#include <string>
#include <vector>

using namespace::std;

#ifndef FREEVARS_H
#define FREEVARS_H

class freevars
{
public:
    freevars();
    void addVar(string name, string filename);
    string parse(string in);
    
private:
    map<string, vector<string>* >* vars;
};

#endif FREEVARS_H