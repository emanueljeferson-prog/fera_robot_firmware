#include "service_base.hpp"

namespace service {

class Encoder: public IService {
public:
    Encoder(core::IMiddleware& middleware);
    void init() override;    
    void readSpeed(double& speed);       
private:    
    core::IMiddleware& middleware;  
};

}