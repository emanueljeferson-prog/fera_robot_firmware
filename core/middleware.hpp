#pragma once 

#include <vector>
#include <queue>
#include "subscriber.hpp"
#include "rtos.hpp"

namespace core {

class IMiddleware {
public:
    virtual ~IMiddleware() = default; 
    virtual void subscribe(Callback cb, const Topics tp, bool delete_flag) = 0;
    virtual void publish(Message& msg) = 0;
    virtual void enqueueTask(const TaskDescription desc) = 0;
    virtual void run() = 0;
private: 
    virtual void registerTask(const TaskDescription desc) = 0;
};

class Middleware: public IMiddleware {
public:
    Middleware();
    void subscribe(Callback cb, const Topics tp, bool delete_flag) override;
    void publish(Message& msg) override;
    void enqueueTask(const TaskDescription desc) override;
    void run() override;
private:
    void registerTask(const TaskDescription desc) override;
    std::vector<Subscriber> subcribers;
    std::queue<TaskDescription> task_queue;
    core::Rtos rtos;
}; 

}