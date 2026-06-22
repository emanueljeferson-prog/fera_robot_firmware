#pragma once 

#include "core/middleware.hpp"
#include "logger/logger.hpp"

namespace service {

class IService {
public:
    virtual ~IService() = default;
    virtual void init() = 0;    
};

}