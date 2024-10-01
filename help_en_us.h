#define HELP_EN_US
std::unordered_map<std::string,std::string> helps_en_us = {
    {"",
R"(`b/help `y[tag]`d
-------------Windows Launcher created by aaaa0ggmc---------------
NativeCommands:
    `y[General]
    `b/help `b/exit `b/lan

    `y[Aliases]
    `b/add    `b/del    `b/ref
    `b/edit   `b/import `b/export
    `b/reload `b/temp   `b/search
    `b/info   `b/list

Doc:
    `baliases

Examples for /help:
    `b/help `c/help
    `b/help `c/lan
    `b/help `caliases
)"},
{"/exit",
R"(`b/exit:
   exit the program,take no arguments
)"
},{"/lan",
R"(`b/lan `c[language_name or language_id]
   `yCurrently supported languages:
       `r[ID] `g[NAME] `d[Descriptions]
       `r 0   `gen_us  `dUS English
       `r 1   `gzh_cn  `d简体中文
)"
}
};
