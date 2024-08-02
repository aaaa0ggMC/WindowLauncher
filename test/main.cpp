#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "console_io.h"
#include <conio.h>
#include <iostream>

using namespace conio;

int main() {
    Console console;

    console.writeEx("H`yell`do `rworld!`d\nWhat's your name:");

    std::string s = console.input();
    std::cout << "I know,your name is [" << s << "]" << std::endl;

    return 0;
}
