#include "middleware.hpp"
#include "logger/logger.hpp"

namespace core {

Middleware::Middleware() {
    logger::info("[CORE] [MIDDLEWARE] [START]");
}

void Middleware::subscribe(Callback cb, const Topics tp, bool delete_flag) {
    subcribers.push_back(Subscriber{ std::move(cb), tp, delete_flag });
}

void Middleware::publish(Message& msg) {
    for(auto const& subscriber: subcribers) {
        if(msg.compareTopic(subscriber.topic)) {
            subscriber.callback(msg);
        }
    }
}

void Middleware::run() {
    subcribers.erase(
        std::remove_if(subcribers.begin(), subcribers.end(), [](const Subscriber& sub) {
            return sub.delete_flag;
        }),
        subcribers.end()
    );  
}

}