#pragma once 

#include "core/middleware.hpp"

namespace service {

class RtosService {
public:
    RtosService(core::IMiddleware& middleware);
    void init();
    void createTask(core::TaskDescription desc);
    void startScheduler();
    void delayTask(const uint32_t delay);
private:
    core::IMiddleware& middleware;
};

}