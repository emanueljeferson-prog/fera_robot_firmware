#pragma once 

#include <vector>
#include "subscriber.hpp"

namespace core {

class IMiddleware {
public:
    virtual ~IMiddleware() = default; 
    virtual void subscribe(Callback cb, const Topics tp) = 0;
    virtual void publish(Message& msg) = 0;
};

class Middleware: public IMiddleware {
public:
    Middleware();
    void subscribe(Callback cb, const Topics tp);
    void publish(Message& msg);
private:
    std::vector<Subscriber> subcribers;
}; 

}