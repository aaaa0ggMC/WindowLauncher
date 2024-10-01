#include <iostream>
#include <alib/autil.h>
#include "console_io.h"

#define MAKE_ARGS std::make_format_args(args,path)

#include "executer.h"
#include "parser.h"
#include "config.h"

#define $wl_config "_wl.cfg"

using namespace alib;
using namespace conio;

int main(){
    Config config;
    config.file($wl_config);
    config.loadConfig();

    Executer executer(config);
    Parser parser;

    std::vector<std::string> sep_args;
    std::string args;
    std::string head;
    std::string in;
    int exec = 0;

    while(exec != (int)EXEC_EXIT){
        std::cout << ">>>";
        getline(std::cin,in);
        parser.ParseCommand(in,head,args,sep_args);
        exec = executer.RunCommand(head,args,sep_args);
    }
    return 0;
}
