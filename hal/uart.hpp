#pragma once 

#include <cstdint>

namespace hal {

class Uart {
public: 
    Uart();
    void init(); 
    void sendByte(const uint8_t byte);
    void getByte(uint8_t& byte);
    void getByteWithTimeOut(uint8_t& byte, const uint64_t timeout);
};    

}