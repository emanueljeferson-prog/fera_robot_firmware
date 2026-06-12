#pragma once 

#include "core/middleware.hpp"
#include <iostream>

namespace service {

class IService {
public:
    virtual ~IService() = default;
    virtual void init() = 0;    
};

}