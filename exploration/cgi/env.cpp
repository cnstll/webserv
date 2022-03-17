# include <iostream>

class env
{
private:
    char **_environment;
    int  strlen_list(char **);
public:
    env(char **env) {
        _environment = new (char)[(strlen_list(env))];
    };
    env(const env& newenv);
    ~env();

    env& operator=(const env& newenv);
};


int env::strlen_list(char **strList) {
    int i = 0;
    while (strList[i])
        i++;
    return i;
}


env::env(void)
{
    return ;
}

env::env(const env& newenv)
{
    return ;
}

env::~env(void)
{
    return ;
}

env& env::operator=(const env& newenv)
{
    return *this;
}