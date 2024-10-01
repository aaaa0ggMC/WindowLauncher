#ifndef EXECUTER_H_INCLUDED
#define EXECUTER_H_INCLUDED
#include <string>
#include <unordered_map>
#include <vector>
#include "console_io.h"
#include "config.h"

#define EXEC_EXIT 0xc0000005

using dstring = const std::string &;
template<class T> using dvector = const std::vector<T> &;
using dsvector = const std::vector<std::string> &;

class Executer{
public:
    Executer(Config&cfg);

    int RunCommand(dstring head,dstring args,dsvector sep_args);

    ///native commands
    int n_help(dsvector args);
    int n_switchlan(int id);

    ///others
    int o_err(dstring data);
    int o_run(dstring cmd);

    conio::Console console;
    std::unordered_map<std::string,std::string> * helper;
    Config * cfg;
};

#endif // EXECUTER_H_INCLUDED
