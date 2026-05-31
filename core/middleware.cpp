#include <iostream>
#include "middleware.hpp"

namespace core {

Middleware::Middleware() {
    std::cout << "[CORE] [MIDDLEWARE] [START]" << std::endl;
}

void Middleware::subscribe(Callback cb, const Topics tp) {
    subcribers.push_back(Subscriber{ std::move(cb), tp });
}

void Middleware::publish(Message& msg) {
    for(auto const& subscriber: subcribers) {
        if(msg.compareTopic(subscriber.topic)) {
            subscriber.callback(msg);
        }
    }
}

}