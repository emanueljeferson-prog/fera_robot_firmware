#pragma once 

#include <vector>
#include "subscriber.hpp"

namespace core {

class IMiddleware {
public:
    virtual ~IMiddleware() = default; 
    virtual void subscribe(Callback cb, const Topics tp, bool delete_flag) = 0;
    virtual void publish(Message& msg) = 0;
    virtual void run() = 0;
};

class Middleware: public IMiddleware {
public:
    Middleware();
    void subscribe(Callback cb, const Topics tp, bool delete_flag);
    void publish(Message& msg);
    void run();
private:
    std::vector<Subscriber> subcribers;
}; 

}