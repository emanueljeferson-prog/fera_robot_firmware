#pragma once 

#include "core/middleware.hpp"

namespace service {

class IService {
public:
    virtual ~IService() = default;
    virtual void init() = 0;    
};

}