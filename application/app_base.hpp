#pragma once
#include "core/middleware.hpp"
#include "pico/stdlib.h"
#include "pico/stdlib.h"
#include "logger/logger.hpp"
#include "config.hpp"

namespace app { 

class IApplication {
public:
    virtual ~IApplication() = default;
    virtual void init() = 0;    
};

}