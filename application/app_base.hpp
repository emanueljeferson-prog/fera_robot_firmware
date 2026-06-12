#include "core/middleware.hpp"
#include <iostream>

namespace app { 

class IApplication {
public:
    virtual ~IApplication() = default;
    virtual void init() = 0;    
};

}