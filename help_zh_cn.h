#define HELP_ZH_CN
std::unordered_map<std::string,std::string> helps_zh_cn = {
    {"",
R"(`b/help `y[标签]`d
-------------Windows Launcher created by aaaa0ggmc---------------
内置命令:
    `y[通用]
    `b/help `b/exit `b/lan

    `y[别名]
    `b/add    `b/del    `b/ref
    `b/edit   `b/import `b/export
    `b/reload `b/temp   `b/search
    `b/info   `b/list

文档:
    `baliases

/help的几个例子:
    `b/help `c/help
    `b/help `c/lan
    `b/help `caliases
)"},
{"/exit",
R"(`b/exit:
   退出程序，不需要参数
)"
},{"/lan",
R"(`b/lan `c[语言名称 或者 语言序号]
   `y目前支持的语言:
       `r[序号] `g[名称]   `d[介绍]
       `r  0    `gen_us    `dUS English
       `r  1    `gzh_cn    `d简体中文
)"
}
};
