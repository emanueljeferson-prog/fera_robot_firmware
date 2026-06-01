#pragma once 

#include "service_base.hpp"

namespace service {

class RtosService: public IService {
public:
    RtosService(core::IMiddleware& middleware);
    void init() override;
    void createTask(core::TaskDescription desc);
    void startScheduler();
    void delayTask(const uint32_t delay);
private:
    core::IMiddleware& middleware;
};

}