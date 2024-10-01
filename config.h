#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED
#include <string>

struct Config{
    ~Config();

    void loadConfig();
    void saveConfig();
    void file(const std::string & f);

    std::string fp;

    ///attributes
    int language;
};


#endif // CONFIG_H_INCLUDED
