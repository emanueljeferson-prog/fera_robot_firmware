#include "core/middleware.hpp"

namespace app { 

class IApplication {
public:
    virtual ~IApplication() = default;
    virtual void init() = 0;    
};

}