#include "console_io.h"
#include <windows.h>
#include <conio.h>
#include <iostream>

using namespace conio;

CONSOLE_SCREEN_BUFFER_INFO info;
bool inited = false;

///About cursors
void cgotoxy(const Pos p){
    COORD coord;
    coord.X = p.x;
    coord.Y = p.y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

Pos cgetxy() {
    Pos ret = {0};
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)){
        ret.x = csbi.dwCursorPosition.X;
        ret.y = csbi.dwCursorPosition.Y;
    }
    return ret;
}

void cmovexy(const Pos p){
    Pos px = cgetxy();
    cgotoxy({px.x + p.x,px.y + p.y});
}

Console::Console(size_t sz){
    inputBuffer = "";
    inputBuffer.reserve(sz);
    inputKeycodeProc = defaultInputKeycodeProc;
}

void Console::write(const std::string & out){
    std::cout << out;
}

void cset(int c){
    if(!inited){
        inited = true;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&info);
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(WORD)c);
}

void cuset(){
    if(!inited){
        inited = true;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&info);
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),info.wAttributes);
}

#define STA_DEF 0
#define STA_FORE 1
#define STA_BACK 2
void Console::writeEx(const std::string& out){
    int status = STA_DEF;
    for(auto & ch : out){
        if(status == STA_DEF){
            if(ch == '`'){
                status = STA_FORE;
                continue;
            }else if(ch == '^'){
                status = STA_BACK;
                continue;
            }
            putchar(ch);
        }else{
            int colorcode = -1;
            //get color opt
            switch(ch){
            case '0':
                colorcode = 0;
                break;
            case 'b':
                colorcode = 1;
                break;
            case 'g':
                colorcode = 2;
                break;
            case 'c':
                colorcode = 3;
                break;
            case 'r':
                colorcode = 4;
                break;
            case 'm':
                colorcode = 5;
                break;
            case 'y':
                colorcode = 6;
                break;
            case 'w':
                colorcode = 7;
                break;
            case 'a':
                colorcode = 8;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                colorcode = (int)(ch - '1') + 9;
                break;
            }
            if((status == STA_FORE && ch == '`') ||
               (status == STA_BACK && ch == '^')){
                putchar(ch);
                status = STA_DEF;
                continue;
            }else if(colorcode == -1 && ch != 'd'){
                putchar(status==STA_FORE?'`':'^');
                putchar(ch);
                status = STA_DEF;
                continue;
            }
            if(ch == 'd'){
                colorcode = (status==STA_FORE)?(info.wAttributes & 0xf):(info.wAttributes & 0xf0);
            }else colorcode = (status==STA_FORE)?colorcode:(colorcode << 4);
            cset(colorcode);
            status = STA_DEF;
        }
    }
    cuset();
}

std::string Console::input(Console::FnInputKeycodeProc fn,char sig,bool completeDirs,std::vector<std::string> * completionKeys){
    char last = 0;
    if(!fn){
        while((last = getch()))continue;
        return "";
    }
    inputBuffer = "";
    void* state = NULL;
    fn(0,inputBuffer,state,false,*this);
    while((last = getch()) != sig){
        fn(last,inputBuffer,state,false,*this);
    }
    if(last == CKEY_ENTER)std::cout << '\n';
    else std::cout << last;
    fn(0,inputBuffer,state,true,*this);
    return inputBuffer;
}

std::string Console::input(char sig,bool completeDirs,std::vector<std::string> * completionKeys){
    return input(inputKeycodeProc,sig,completeDirs,completionKeys);
}

std::string Console::inputm(Console::FnInputKeycodeProc fn,const std::string & sig,bool completeDirs,std::vector<std::string> * completionKeys){
    char last = 0;
    if(!fn){
        while((last = getch()))continue;
        return "";
    }
    inputBuffer = "";
    void* state = NULL;
    //init states
    fn(0,inputBuffer,state,false,*this);
    while(true){
        last = getch();
        if(sig.find(last) != sig.size())break;
        fn(last,inputBuffer,state,false,*this);
    }
    if(last == CKEY_ENTER)std::cout << '\n';
    else std::cout << last;
    //delete states
    fn(0,inputBuffer,state,true,*this);
    return inputBuffer;
}

std::string Console::inputm(const std::string& sig,bool completeDirs,std::vector<std::string> * completionKeys){
    return inputm(inputKeycodeProc,sig,completeDirs,completionKeys);
}

void Console::defaultInputKeycodeProc(char code,std::string&appender,void* & pstate,bool del,Console & con){
    static std::vector<std::string> histories;
    [[maybe_unused]] static unsigned int his_index = 0;
    struct State{
        bool action;
        Pos left;
        Pos right;
        unsigned int beg;
        unsigned int count;
        unsigned int posIndex;
    };
    if(del && pstate){
        delete (State*)(pstate);
        return;
    }
    if(pstate == NULL){
        pstate = new State();
        State& s = *((State*)(pstate));
        s.right = s.left = cgetxy();
        s.beg = appender.size();
        s.count = 0;
        s.posIndex = 0;
        return;
    }

    State& s = *((State*)(pstate));

    if(code == -32){
        s.action = true;
        return;
    }
    if(s.action){
        switch(code){
        case 75:
            if(s.posIndex != 0){
                s.posIndex--;
                cmovexy({-1,0});
            }
            break;
        case 77:
            if(s.posIndex != s.count){
                s.posIndex++;
                cmovexy({1,0});
            }
            break;
        case 83:
            if(s.posIndex < s.count){
                appender.erase(s.posIndex + s.beg + 1,1);
                s.count--;

                unsigned int rew = 0;
                for(unsigned int i = s.beg + s.posIndex;i < appender.size();++i){
                    putchar(appender[i]);
                    rew++;
                }
                putchar(' ');
                cmovexy({-1 * ((int)(rew + 1)),0});
                break;
            }
        }
        s.action = false;
        return;
    }

    //special actions
    switch(code){
    case CKEY_ENTER:
        //print enter(13) will return to the beg
        putchar(code);
        code = '\n';
        break;
    case CKEY_BACKSPACE:
        if(s.posIndex > 0){
            appender.erase(s.posIndex + s.beg - 1,1);
            s.count--;

            unsigned int rew = 0;
            cmovexy({-1,0});
            for(unsigned int ixx = s.beg + s.posIndex - 1;ixx < appender.size();++ixx){
                putchar(appender[ixx]);
                rew++;
            }
            putchar(' ');
            cmovexy({-1 * ((int)(rew + 1)),0});
            s.posIndex--;
        }
        return;
    }

    if(s.posIndex == s.count){
        putchar(code);
        appender += code;
    }else{
        appender.insert(s.beg + s.posIndex,&code,1);
        unsigned int rew = 0;
        for(unsigned int b = s.posIndex + s.beg;b < appender.size();++b){
            putchar(appender[b]);
            rew++;
        }
        cmovexy({-1 * ((int)(rew) - 1),0});
    }
    s.count++;
    s.posIndex++;
}
