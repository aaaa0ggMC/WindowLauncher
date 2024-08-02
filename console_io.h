#ifndef CONSOLE_IO_H_INCLUDED
#define CONSOLE_IO_H_INCLUDED
#include <string>
#include <vector>

struct Pos{
    int x;
    int y;
};
///about cursors
void cgotoxy(const Pos p);
Pos cgetxy();
void cmovexy(const Pos p);

///colors
void cset(int c);
void cuset();

#define CKEY_ENTER (char)13
#define CKEY_CTRL_ENTER (char)10
#define CKEY_BACKSPACE (char)8

namespace conio{

    ///about input
    struct Console{
        //keycode
        typedef void (*FnInputKeycodeProc)(char code,std::string&appender,void* & state,bool del,Console&);
        void setInputKeycodeProc(FnInputKeycodeProc);
        static void defaultInputKeycodeProc(char code,std::string&appender,void* & state,bool del,Console&);

        FnInputKeycodeProc inputKeycodeProc;

        //do not support multi lines
        std::string input(char sig = CKEY_ENTER,bool completeDirs = false,std::vector<std::string> * completionKeys = NULL);
        std::string input(FnInputKeycodeProc fn,char sig = CKEY_ENTER,bool completeDirs = false,std::vector<std::string> * completionKeys = NULL);

        std::string inputm(const std::string& sig,bool completeDirs = false,std::vector<std::string> * completionKeys = NULL);
        std::string inputm(FnInputKeycodeProc fn,const std::string& sig,bool completeDirs = false,std::vector<std::string> * completionKeys = NULL);

        void write(const std::string & out);
        //support colors
        /**
       "content `rred color(foreground) ^bblue background"

       `X foreground
       ^X background

       X:
       d default
       0 black
       b blue
       r red
       y rellow
       g green
       w white
       c cyan
       a gray
       m magenta
       1 light blue(number 1,not the smaller case of "L")
       2 light green
       3 light cyan
       4 light red
       5 light magenta
       6 light yellow
       7 light white

       ^^ = ^
       `` = `
        */
        void writeEx(const std::string & out);

        Console(size_t reserve_size = 4096);
        std::string inputBuffer;
    };
}

#endif // CONSOLE_IO_H_INCLUDED
