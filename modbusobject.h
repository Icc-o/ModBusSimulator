#ifndef MODBUSOBJECT_H
#define MODBUSOBJECT_H
#include "modbus.h"
#include <vector>
#include <string>
#include <thread>
#include <functional>
#include <QStringList>

enum{
    TCP = 0,
    TCPPI,
    RTU
};

const int MAX_REG_COUNT = 1024;

class ModBusObject
{
public:
    ModBusObject(int type);
    ~ModBusObject(){}

    bool connect(std::string ip, int port);
    bool write_register(int startAddr , std::vector<uint16_t>& valueArr);
    bool read_register(int startAddr, int count, std::vector<uint16_t>& valueArr);
    bool read_coil(int startAddr, int count, std::vector<uint8_t> &values);
    bool write_coil(int startAddr, std::vector<uint8_t> values);
    bool disconnect();

private:
    modbus_t*   m_ctx;
    int         m_type;
    uint32_t    m_timeout_sec;
    uint32_t    m_timeout_usec;

    uint8_t     m_coilReadBuffer[MAX_REG_COUNT];
    uint8_t     m_coilWriteBuffer[MAX_REG_COUNT];

    uint16_t    m_regReadBuffer[MAX_REG_COUNT];
    uint16_t    m_regWriteBuffer[MAX_REG_COUNT];
};

class ModBusServer
{
public:
    ModBusServer(int type);
    ~ModBusServer();

    void init(std::string ip, int port);

    void set_update_func(std::function<void(QStringList)> func){m_updateFunc = func;}   //更新显示的
    QStringList generate_msg();                                                         //更具当前的寄存器值生成需要显示的内容

    void get_start_addrs(std::vector<uint16_t>& addrs); //获取起始地址到界面显示

    void run();
    void close();
private:
    int         m_type;
    modbus_t*   m_ctx;

    uint16_t    m_bit_addr;
    uint16_t    m_bit_nb;
    uint16_t    m_input_bit_addr;
    uint16_t    m_input_bit_nb;

    uint16_t    m_reg_addr;
    uint16_t    m_reg_nb;
    uint16_t    m_input_reg_addr;
    uint16_t    m_input_reg_nb;

    modbus_mapping_t *m_mapping;
    int         m_clientFd;

    uint8_t*    m_query;
    int         m_head_len;

    bool m_runStatus;
    std::thread m_runThread;

    std::function<void(QStringList)> m_updateFunc;
};

#endif // MODBUSOBJECT_H
