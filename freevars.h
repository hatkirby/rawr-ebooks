#include <map>
#include <string>
#include <vector>

#ifndef FREEVARS_H
#define FREEVARS_H

class freevars
{
public:
    freevars();
    void addVar(std::string name, std::string filename);
    std::string parse(std::string in);
    
private:
    std::map<std::string, std::vector<std::string>* >* vars;
};

#endif