#include "executer.h"
#include <unordered_map>
#include <stdlib.h>
#include <string.h>
#include <alib/astring.h>

#include "help_en_us.h"
#include "help_zh_cn.h"

using namespace alib;

int Executer::RunCommand(dstring head,dstring args,dsvector sep_args){
    int ret = 0;
    if(!head.compare(""))return -1;
    if(!head.compare("/help")){
        ret = n_help(sep_args);
    }else if(!head.compare("/exit")){
        ret = EXEC_EXIT;
    }else if(!head.compare("/lan")){
        //switch language
        if(!sep_args.size()){
            n_help({"/lan"});
            return -1;
        }
        if(isdigit(sep_args[0][0])){
            //id
            n_switchlan(atoi(sep_args[0].c_str()));
        }else{
            if(!sep_args[0].compare("ch_cn"))n_switchlan(1);
            else n_switchlan(0);
        }
    }else{
        std::string dcmd = head;
        dcmd += " ";
        dcmd += args;
        o_run(dcmd);
    }
    return ret;
}

int Executer::o_err(dstring data){
    std::string composed = "`r";
    composed += data;
    console.writeEx(composed);
    return 0;
}

int Executer::o_run(dstring cmd){
    system(cmd.c_str());
    return 0;
}

///need trimmed string
int Executer::n_help(dsvector args){
    if(args.size()){
        auto iter = helper->find(args[0]);
        if(iter != helper->end()){
            console.writeEx(ng::converter::utf8_to_ansi(iter->second));
            return 0;
        }
    }
    console.writeEx(ng::converter::utf8_to_ansi((*helper)[""]));
    return 0;
}

Executer::Executer(Config&cfg){
    this->cfg = &cfg;
    n_switchlan(this->cfg->language);
}

int Executer::n_switchlan(int id){
    switch(id){
    case 1:
        this->helper = &helps_zh_cn;
        break;
    default:
        this->helper = &helps_en_us;
    }
    return 0;
}
