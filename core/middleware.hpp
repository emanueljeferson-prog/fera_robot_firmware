#pragma once 

#include <vector>
#include "subscriber.hpp"

namespace core {

class Middleware {
public:
    Middleware();
    void subscribe(Callback cb, const Topics tp);
    void publish(Message& msg);
private:
    std::vector<Subscriber> subcribers;
}; 

}