#include "config.h"

Config* Config::m_ptr = nullptr;

Config::Config()
{

}

Config* Config::getPtr()
{
    if(m_ptr == nullptr)
        m_ptr = new Config();
    return m_ptr;
}

bool Config::init(std::__cxx11::string fileName)
{
    return true;
}

