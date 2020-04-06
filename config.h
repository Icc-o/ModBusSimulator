#ifndef CONFIG_H
#define CONFIG_H
#include <string>

struct ServerConfig
{
    int port=1502;
};

struct ClientConfig
{
    int type = 0;
    std::string servevrIp="127.0.0.1";
    int serverPort= 1502;
    bool isConnected;
};

// 一些配置参数
class Config
{
public:
    ~Config(){
        m_isServer = false;
    }
    bool init(std::string fileName = "modbus.ini"); //使用文件初始化一下
    ServerConfig* get_svr_cfg(){return &m_server;}
    ClientConfig* get_cli_cfg(){return &m_client;}
    void set_is_server(bool isServer){m_isServer = isServer;}
    static Config* getPtr();

private:
    Config();
    ServerConfig m_server;
    ClientConfig m_client;
    bool m_isServer;
    static Config* m_ptr;
};

#endif // CONFIG_H
