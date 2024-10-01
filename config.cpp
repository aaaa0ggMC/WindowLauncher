#include "config.h"
#include <alib/adata.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace alib;

void Config::loadConfig(){
    ng::GDoc & doc = *(new ng::GDoc());
    doc.read_parseFileTOML(fp);
    auto str = doc.get("language");
    if(!str || strcmp(*str,"zh_cn")){
        language = 0;
    }else language = 1;

    delete (&doc);
}

void Config::saveConfig(){
    std::ofstream ofs(fp);
    if(ofs.bad()){
        std::cerr << "Cannot save _wl.cfg." << std::endl;
        return;
    }
    ofs << "language = ";
    switch(language){
    case 1:
        ofs << "\'zh_cn\'";
        break;
    default:
        ofs << "\'en_us\'";
    }
    ofs.close();
}

void Config::file(const std::string & f){
    fp = f;
}

Config::~Config(){
    saveConfig();
}
