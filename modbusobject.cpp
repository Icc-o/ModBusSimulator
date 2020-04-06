#include "modbusobject.h"
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <QString>


using namespace std;

ModBusObject::ModBusObject(int type):m_type(type)
{
    memset(m_coilReadBuffer, 0, sizeof(m_coilReadBuffer));
    memset(m_coilWriteBuffer, 0, sizeof(m_coilWriteBuffer));
    memset(m_regReadBuffer, 0, sizeof(m_regReadBuffer));
    memset(m_regWriteBuffer, 0, sizeof(m_regWriteBuffer));
}

bool ModBusObject::connect(std::string ip, int port)
{
    if(m_type == TCP)
        m_ctx = modbus_new_tcp(ip.c_str(),port);
    else if(m_type == TCPPI)
        m_ctx = modbus_new_tcp_pi("::1","1502");
    else
        m_ctx = modbus_new_rtu("/dev/ttyUSB1", 115200,'N',8,1);

    if(!m_ctx)
    {
        cout<<"new context failed!"<<endl;
        return false;
    }
    modbus_get_response_timeout(m_ctx, &m_timeout_sec, &m_timeout_usec);

    modbus_set_debug(m_ctx, TRUE);

    if(modbus_connect(m_ctx) == -1)
        return false;
    return true;
}

bool ModBusObject::write_register(int startAddr,std::vector<uint16_t> &valueArr)
{
    memset(m_regWriteBuffer, 0, sizeof(m_regWriteBuffer));
    int count = valueArr.size();
    if(count > MAX_REG_COUNT)
        return false;

    for(int i=0; i<count; i++)
    {
        m_regWriteBuffer[i] = valueArr[i];
    }
    modbus_write_registers(m_ctx,startAddr,count,m_regWriteBuffer);
    return true;
}

bool ModBusObject::read_register(int startAddr, int count, std::vector<uint16_t> &valueArr)
{
    if(count > MAX_REG_COUNT)
        return false;
    memset(m_regReadBuffer, 0, sizeof(m_regReadBuffer));
    modbus_read_registers(m_ctx, startAddr, count, m_regReadBuffer);
    for(int i=0; i<count; i++)
    {
        valueArr.push_back(m_regReadBuffer[i]);
    }

    return true;
}

bool ModBusObject::read_coil(int startAddr, int count, std::vector<uint8_t> &values)
{
    memset(m_coilReadBuffer, 0, sizeof(m_coilReadBuffer));
    modbus_read_bits(m_ctx,startAddr, count, m_coilReadBuffer);
    for(int i=0; i<count; i++)
    {
        values.push_back(m_coilReadBuffer[i]);
    }
    return true;
}

bool ModBusObject::write_coil(int startAddr, std::vector<uint8_t> values)
{
    int count = values.size();
    if(count > MAX_REG_COUNT)
        return false;
    for(int i=0; i<count; i++)
    {
        m_coilWriteBuffer[i] = values[i];
    }
    modbus_write_bits(m_ctx, startAddr,count, m_coilWriteBuffer);
    return true;
}

bool ModBusObject::disconnect()
{
    modbus_close(m_ctx);
}


ModBusServer::ModBusServer(int type):m_type(type),m_runStatus(false)
{

}

ModBusServer::~ModBusServer()
{}

void ModBusServer::init(std::string ip, int port)
{
    //可以从config里面进行初始化
    m_bit_addr          = 0x130;
    m_bit_nb            = 0x25;
    m_input_bit_addr    = 0x1c4;
    m_input_bit_nb      = 0x16;

    m_reg_addr          = 0x160;
    m_reg_nb            = 0x3;
    m_input_reg_addr    = 0x108;
    m_input_reg_nb      = 0x1;

    if(m_type == TCP)
    {
        m_ctx = modbus_new_tcp(ip.c_str(), port);
        m_query = (uint8_t*)malloc(MODBUS_TCP_MAX_ADU_LENGTH);
    }
    else if(m_type == TCPPI)
    {
        m_ctx = modbus_new_tcp_pi("::0","1052");
        m_query = (uint8_t*)malloc(MODBUS_TCP_MAX_ADU_LENGTH);
    }
    else if(m_type == RTU)
    {
        m_ctx = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1);
    }

    modbus_set_debug(m_ctx, TRUE);
    m_head_len = modbus_get_header_length(m_ctx);

    m_mapping = modbus_mapping_new_start_address(
                m_bit_addr, m_bit_nb,
                m_input_bit_addr, m_input_bit_nb,
                m_reg_addr, m_reg_nb,
                m_input_reg_addr, m_input_reg_nb);
    if(m_mapping == nullptr)
    {
        std::cout<<"map failed!"<<std::endl;
    }

    //开启单独的线程去处理请求
    m_runStatus = true;
    m_runThread = std::thread(&ModBusServer::run, this);
}

void ModBusServer::run()
{
    m_updateFunc(generate_msg());
    m_clientFd = -1;
    if(m_type == TCP)
    {
        m_clientFd = modbus_tcp_listen(m_ctx, 1);
        int ret = modbus_tcp_accept(m_ctx, &m_clientFd);
        if(ret == -1)
            return;
    }
    else if(m_type == TCPPI)
    {

    }
    else if(m_type == RTU)
    {

    }

    int ret;
    while(m_runStatus)
    {
        do{
            ret = modbus_receive(m_ctx, m_query);
        }while(ret == 0);

        if(ret == -1 && errno != EMBBADCRC)
            break;

        ret = modbus_reply(m_ctx, m_query, ret, m_mapping);
        if(ret == -1)
            break;
        m_updateFunc(generate_msg());
    }
}

void ModBusServer::close()
{
    m_runStatus = false;
    if(m_runThread.joinable())
        m_runThread.join();
    if(m_type == TCP)
        ::close(m_clientFd);

    try{
    modbus_mapping_free(m_mapping);
    }
    catch(...)
    {

    }
    free(m_query);
    modbus_close(m_ctx);
    modbus_free(m_ctx);
}

QStringList ModBusServer::generate_msg()
{
    QStringList ret;
    QString content;
    for(int i=0; i<m_bit_nb-1; i++)
    {
        content+= QString::number(m_mapping->tab_bits[i]);
        content+=".";
    }
    content+=QString::number(m_mapping->tab_bits[m_bit_nb-1]);
    ret<<content;

    content.clear();
    for(int i=0; i<m_input_bit_nb-1; i++)
    {
        content+= QString::number(m_mapping->tab_input_bits[i]);
        content+=".";
    }
    content+=QString::number(m_mapping->tab_input_bits[m_input_bit_nb-1]);
    ret<<content;

    content.clear();
    for(int i=0; i<m_reg_nb-1; i++)
    {
        content+= QString::number(m_mapping->tab_registers[i]);
        content+=".";
    }
    content+=QString::number(m_mapping->tab_registers[m_reg_nb-1]);
    ret<<content;

    content.clear();
    for(int i=0; i<m_input_reg_nb-1; i++)
    {
        content+= QString::number(m_mapping->tab_input_registers[i]);
        content+=".";
    }
    content+=QString::number(m_mapping->tab_input_registers[m_input_reg_nb-1]);
    ret<<content;

    return ret;
}

void ModBusServer::get_start_addrs(std::vector<uint16_t> &addrs)
{
    addrs.push_back(m_bit_addr);
    addrs.push_back(m_input_bit_addr);
    addrs.push_back(m_reg_addr);
    addrs.push_back(m_input_reg_addr);
}











