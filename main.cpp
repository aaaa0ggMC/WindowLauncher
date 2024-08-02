#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <map>
#include <format>
#include <toml++/toml.hpp>
#include <unordered_map>
#include <cmath>
#include <windows.h>
#include <shellapi.h>

#include <alib/autil.h>
#include "console_io.h"

#define MAKE_ARGS std::make_format_args(args,path)

using namespace std;
using namespace alib::ng;
using namespace conio;


struct Alias{
    string name;

    //not unescaped
    string cmd;
    string jmp;
    string launch;
    string launchAdmin;

    string restContent;

    Alias(){
        name = cmd = jmp = launch = launchAdmin = restContent = "";
    }
};
std::unordered_map<string,Alias> dataAl;
///Reading toml files
toml::table config;
toml::table aliases;
bool suc = false;
Console console;

const char * help_str =
R"(`rWindows Launcher `dby `1aaaa0ggmc
`dVersion:`g0.2
`gCommand that begins with ">" will be sent directly to the Windows system.
`dNative commands:
`6#help                        `dget some help
`6#list `a[count] [opts...]      `dlist all the aliases,count:the maximum count of aliases showed in a row(def 16)
`6#info `a[alias_name]           `dget the info of an alias
`6#add `a[alias_name]            `dadd a new alias
`6#delete `a[alias_name]         `ddelete an alias
`6#exit                        `dexit the launcher
`6#reload                      `dexit the launcher
`6#save                        `dsave alias configuration manly
`6#ref `a[ref_name] `a[alias]      `dcreate a "shortcut" to an alias
`6#edit `a[alias]                `dcreate a "shortcut" to an alias

`d#list options:
`asort                         `dsort the aliases from 1 to a to z
`arsort                        `dsort the aliases from z to a to 1
`aalign                        `dmake these aliases printed neatly
)";

string tolower(const string & dat);
void load_config();
void save();
void addAlias(vector<string>& args);
void help_page();
void editAlias(vector<string>& args);
void infoAlias(vector<string>&args);
void deleteAlias(vector<string>& args);
void refAlias(vector<string> & args);
void listAlias(vector<string> & args);

int main(){
    console.writeEx("`rWindows Launcher`d                            `aEnter #help to get some help.\n");
    load_config();
    atexit([]{
        save();
    });
    {
        HWND consoleWnd = GetConsoleWindow();
        HMENU systemMenu = NULL;
        if(consoleWnd)systemMenu = GetSystemMenu(consoleWnd,FALSE);

        if(systemMenu){
            EnableMenuItem(systemMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
        }
    }

    string command,path = "";
    vector<string> sep_args;

    sep_args.resize(32);
    system("title WindowsLauncher");
    ///shell menu
    while(true){
        path = filesystem::current_path().string();
        console.writeEx(string("`b~`6") + path + "`r>>>");

        getline(cin,command);

        ///analyze
        if(command.compare("")){
            if(command[0] == '>'){
                ///do not use alias by def
                string dta = Util::str_unescape(command.substr(1));
                if(!dta.compare("exit")){
                    exit(0);
                }
                system(dta.c_str());
                continue;
            }
            //extract the first word and check
            auto pos = command.find(' ');
            string tit,args;
            if(pos != std::string::npos){
                tit = tolower(command.substr(0,pos));
                args = command.substr(pos);
            }else{
                tit = tolower(command);
                args = "";
            }

            //find
            int jmps = 0;
            bool jump = false;

            while(jmps < 64){
                {
                    sep_args.clear();
                    vector<string> tmp_args;
                    Util::str_split(args," ",tmp_args);
                    for(auto&y:tmp_args){
                        if(Util::str_trim_rt(y).compare("")){
                            sep_args.push_back(y);
                        }
                    }
                }
                if(!tit.compare("#help")){
                    help_page();
                    break;
                }else if(!tit.compare("#list")){
                    listAlias(sep_args);
                    break;
                }else if(!tit.compare("#exit")){
                    exit(0);
                }else if(!tit.compare("#save")){
                    save();
                    break;
                }else if(!tit.compare("#add")){
                    addAlias(sep_args);
                    break;
                }else if(!tit.compare("#edit")){
                    editAlias(sep_args);
                    break;
                }else if(!tit.compare("#delete")){
                    deleteAlias(sep_args);
                    break;
                }else if(!tit.compare("#reload")){
                    load_config();
                    break;
                }else if(!tit.compare("#info")){
                    infoAlias(sep_args);
                    break;
                }else if(!tit.compare("#ref")){
                    refAlias(sep_args);
                    break;
                }

                auto t = dataAl.find(tit);
                if(t != dataAl.end()){
                    auto & v = (t->second);
                    if(v.cmd.compare("")){
                        string buf = std::vformat(v.cmd,MAKE_ARGS);
                        system(buf.c_str());
                    }
                    if(v.launchAdmin.compare("")){
                        SHELLEXECUTEINFO se;
                        memset(&se,0,sizeof(se));
                        se.cbSize = sizeof(se);
                        se.lpVerb = "runas";
                        se.lpFile = v.launchAdmin.c_str();
                        se.nShow = SW_SHOW;
                        se.lpParameters = args.c_str();

                        ShellExecuteEx(&se);
                    }
                    if(v.launch.compare("")){
                        ShellExecute(NULL,"open",v.launch.c_str(),args.c_str(),NULL,SW_SHOW);
                    }
                    if(v.jmp.compare("")){
                        ++jmps;
                        jump = true;
                        pos = v.jmp.find(' ');
                        if(pos != std::string::npos){
                            tit = tolower(v.jmp.substr(0,pos));
                            args = std::vformat(v.jmp.substr(pos),MAKE_ARGS);
                        }else{
                            tit = tolower(v.jmp);
                            args = "";
                        }
                    }
                }else{
                    string dta = Util::str_unescape(command);
                    if(!dta.compare("exit")){
                        exit(0);
                    }
                    system(dta.c_str());
                }
                if(!jump){
                    break;
                }
                jump = false;
            }
        }
    }

    return 0;
}

string tolower(const string & dat){
    string data = dat;
    std::transform(data.begin(), data.end(), data.begin(),
    [](unsigned char c){ return std::tolower(c); });
    return data;
}

void load_config(){

    dataAl.clear();

    try{
        config = toml::parse_file("_wl_config.dat");
    }catch(const toml::parse_error& err){
        cout << "Unable to parse toml file _wl_config.dat:" << err << endl;
    }catch(...){
        cout << "Unable to parse toml file _wl_config.dat." << endl;
    }


    try{
        aliases = toml::parse_file("_wl.dat");
        for(auto& [ehead,ele] : aliases){
            if(ele.is_table()){
                Alias a;
                a.name = ehead;
                //cout << ehead << endl;
                ///twice
                for(auto &[title,node] : *(ele.as_table())){
                    string value = (node.as_string()==nullptr?"":**(node.as_string()));
                    if(!title.str().compare("cmd")){
                        a.cmd = value;
                    }else if(!title.str().compare("jmp")){
                        a.jmp = value;
                    }else if(!title.str().compare("launch")){
                        a.launch = value;
                    }else if(!title.str().compare("launchAdmin")){
                        a.launchAdmin = value;
                    }else{
                        //save
                        a.restContent += title.str();
                        a.restContent += " = \"" + value + "\" \n";
                    }
                }
                dataAl.try_emplace(tolower(a.name),a);
            }
        }
        suc = true;//哪个傻逼没写这个东西，害的_wl.dat全空了
    }catch(const toml::parse_error& err){
        cout << "Unable to parse toml file _wl_config.dat:" << err << endl;
    }catch(...){
        cout << "Unable to parse toml file _wl.dat." << endl;
    }

}

void save(){
    if(suc){
        toml::table svtb;

        for(auto& [_,as] : dataAl){
            toml::table sub;
            sub.insert_or_assign("cmd",as.cmd);
            sub.insert_or_assign("launchAdmin",as.launchAdmin);
            sub.insert_or_assign("launch",as.launch);
            sub.insert_or_assign("jmp",as.jmp);
            try{
                toml::table sb = toml::parse(as.restContent);
                for(auto & v : sb){
                    sub.insert_or_assign(v.first,v.second);
                }
            }catch(...){}
            svtb.insert_or_assign(as.name,sub);
        }

        ofstream wl("_wl.dat");
        if(wl.good()){
            wl << svtb;
            wl.close();
        }
    }
}

void addAlias(vector<string>& args){
    Alias a;
    string word,tmp;
    if(args.size() < 1){
        cout << "Name(Cases will be ignored):";
        cin >> word;
        getline(cin,tmp);
        if(!word.compare("")){
            Util::io_printColor("Word is empty!\n",APCF_RED);
            return;
        }
    }else word = args[0];
    if(dataAl.find(tolower(word)) != dataAl.end()){
        Util::io_printColor("The name exists!\n",APCF_RED);
        return;
    }
    a.name = word;
    cout << "Command(Support escaped string):";
    getline(cin,tmp);
    Util::str_trim(tmp);
    a.cmd = tmp;
    cout << "LaunchAdmin(Support escaped string):";
    getline(cin,tmp);
    Util::str_trim(tmp);
    a.launchAdmin = tmp;
    cout << "Launch(Support escaped string):";
    getline(cin,tmp);
    Util::str_trim(tmp);
    a.launch = tmp;
    cout << "JumpTo:";
    getline(cin,tmp);
    Util::str_trim(tmp);
    a.jmp = tmp;

    dataAl.emplace(tolower(word),a);

    Util::io_printColor("Successfully added the alias.\n",APCF_YELLOW);
}

void help_page(){
    console.writeEx(help_str);
}

void editAlias(vector<string>& args){
    string word,tmp;
    if(args.size() < 1){
        cout << "Name(Cases will be ignored):";
        cin >> word;
        getline(cin,tmp);
        if(!word.compare("")){
            Util::io_printColor("Word is empty!\n",APCF_RED);
            return;
        }
    }else word = args[0];
    auto it = dataAl.find(tolower(word));
    if(it == dataAl.end()){
        Util::io_printColor("Can't find the alias!\n",APCF_RED);
        return;
    }
    Alias a = it->second;
    dataAl.erase(it);
    cout << "Press enter will keep a field the same.\n";
    cout << "New Name:";
    cin >> word;
    getline(cin,tmp);
    if(dataAl.find(tolower(word)) != dataAl.end()){
        Util::io_printColor("The name exists!\n",APCF_RED);
        return;
    }
    if(Util::str_trim_rt(word).compare("")){
        a.name = word;
    }

    cout << "Command(Support escaped string):";
    getline(cin,tmp);
    Util::str_trim(tmp);
    if(tmp.compare("")){
        a.cmd = tmp;
    }
    cout << "LaunchAdmin(Support escaped string):";
    getline(cin,tmp);
    Util::str_trim(tmp);
    if(tmp.compare("")){
        a.launchAdmin = tmp;
    }
    cout << "Launch(Support escaped string):";
    getline(cin,tmp);
    Util::str_trim(tmp);
    if(tmp.compare("")){
        a.launch = tmp;
    }
    cout << "JumpTo:";
    getline(cin,tmp);
    Util::str_trim(tmp);
    if(tmp.compare("")){
        a.jmp = tmp;
    }

    dataAl.emplace(tolower(word),a);
    Util::io_printColor("Successfully edited the alias.\n",APCF_YELLOW);
}

void infoAlias(vector<string>&args){
    string searchv = "";
    if(args.size() < 1){
        cout << "Alias name:";
        cin >> searchv;
        string tmp;
        getline(cin,tmp);
    }else searchv = args[0];
    auto t = dataAl.find(tolower(searchv));
    if(t != dataAl.end()){
        auto& v = t->second;
        string p = "[";
        p += v.name;
        p += "]\n";
        Util::io_printColor(p,APCF_LIGHT_BLUE);

        p = "Command:";
        p += v.cmd;
        p += "\n";
        Util::io_printColor(p,APCF_LIGHT_CYAN);


        p = "LaunchAdmin:";
        p += v.launchAdmin;
        p += "\n";
        Util::io_printColor(p,APCF_LIGHT_CYAN);


        p = "Launch:";
        p += v.launch;
        p += "\n";
        Util::io_printColor(p,APCF_LIGHT_CYAN);


        p = "JumpTo:";
        p += v.jmp;
        p += "\n";
        Util::io_printColor(p,APCF_LIGHT_CYAN);
    }else{
        Util::io_printColor("Can't find the alias!\n",APCF_RED);
    }
}

void deleteAlias(vector<string>& args){
    string sv = "";
    if(args.size() < 1){
        cout << "Alias name:";
        cin >> sv;
        string tmp;
        getline(cin,tmp);
    }else sv = args[0];
    auto t = dataAl.find(tolower(sv));
    if(t != dataAl.end()){
        dataAl.erase(t);
        Util::io_printColor("Successfully erased the alias.\n",APCF_YELLOW);
    }else{
        Util::io_printColor("Can't find the alias!\n",APCF_RED);
    }
}

void refAlias(vector<string> & args){
    string a = "",b = "";
    if(args.size() < 1){
        cout << "New alias name:";
        cin >> a;
        string tmp;
        getline(cin,tmp);
    }else a = args[0];
    if(args.size() < 2){
        cout << "Ref to:";
        getline(cin,b);
        Util::str_trim(b);
    }else b = args[1];

    string tit;
    auto pos = b.find(' ');
    if(pos != std::string::npos){
        tit = tolower(b.substr(0,pos));
    }else{
        tit = tolower(b);
    }

    if(dataAl.find(tolower(a)) != dataAl.end()){
        Util::io_printColor("The name exists!\n",APCF_RED);
        return;
    }
    //skip native ref
    //if(dataAl.find(tolower(tit)) == dataAl.end()){
    //    Util::io_printColor(string("Cant find the alias:") + tit + "\n",APCF_RED);
    //    return;
    //}
    Alias t;
    t.name = a;
    t.jmp = b;

    dataAl.emplace(tolower(a),t);

    Util::io_printColor("Successfully refed the alias.\n",APCF_YELLOW);
}

void listAlias(vector<string> & sep_args){
    console.writeEx("`6Here are all the aliases:\n");
    int count = 0;
    int mxc = 16;
    if(sep_args.size() >= 1){
        mxc = atoi(sep_args[0].c_str());
        sep_args.erase(sep_args.begin());
        if(mxc == 0)mxc = 1;
    }
    int sortv = 0;
    bool align = false;
    for(auto& c : sep_args){
        if(!c.compare("sort")){
            sortv = 1;
        }else if(!c.compare("rsort")){
            sortv = -1;
        }else if(!c.compare("align")){
            align = true;
        }
    }

    vector<Alias*> mpData;

    for(auto & [_,v] : dataAl){
        mpData.push_back(&v);
    }

    if(sortv == 1){
        sort(mpData.begin(),mpData.end(),[](Alias* a,Alias* b){
            return (a->name.compare(b->name) < 0);
        });
    }else if(sortv == -1){
        sort(mpData.begin(),mpData.end(),[](Alias* a,Alias* b){
            return (a->name.compare(b->name) > 0);
        });
    }

    if(align && mxc >= 2){
        //step 1:generate strings
        vector<string> outputs;
        for(auto & pa : mpData){
            Alias & a = *pa;
            if(a.jmp.compare("")){
                outputs.push_back(a.name + string("(") + a.jmp + ")");
            }
            else outputs.push_back(a.name);
        }

        size_t sz = ceil(outputs.size() / (float)mxc);
        unsigned int spaces[mxc];
        for(unsigned int i = 0;i < (unsigned int)mxc;++i){
            size_t maxi = 0;
            for(unsigned int j = 0;j < sz && (j * mxc + i)<outputs.size();++j){
                size_t len = outputs[j * mxc + i].size();
                if(len > maxi)maxi = len;
            }
            spaces[i] = maxi;
        }

//        for(int xx = 0 ;xx < mxc;++xx){
//            cout << spaces[xx] << endl;
//        }

        //print results
        for(unsigned int i = 0;i < sz;++i){
            for(unsigned int j = 0;j < (unsigned int)mxc && (i*mxc+j)<outputs.size();++j){
                Alias & a = *mpData[i * mxc +j];
                if(a.jmp.compare("")){
                    console.writeEx(a.name + string("(`b") + a.jmp + "`d)");
                }
                else console.writeEx(a.name);
                int spacex = spaces[j] - (unsigned int)outputs[i*mxc+j].size();
                if(spacex <= 0)spacex = 0;
//                cout << spacex << endl;
                for(unsigned int fe = 0;fe < (unsigned int)spacex;++fe){
                    putchar(' ');
                }
                console.writeEx("^w ");
            }
            putchar('\n');
        }

        return;
    }

    for(auto & pa : mpData){
        Alias & a = *pa;
        if(a.jmp.compare("")){
            console.writeEx(a.name + string("(`b") + a.jmp + "`d)^w ");
        }
        else console.writeEx(a.name + "^w ");
        ++count;
        if(count >= mxc){
            count = 0;
            cout << endl;
        }
    }
    cout << endl;
}
