#include "middleware.hpp"
#include "logger/logger.hpp"

namespace core {

Middleware::Middleware(): rtos() {
    LOG_INFO("[CORE] [MIDDLEWARE] [START]");
}

void Middleware::subscribe(Callback cb, const Topics tp, bool delete_flag) {
    subcribers.push_back(Subscriber{ std::move(cb), tp, delete_flag });
}

void Middleware::publish(Message& msg) {
    LOG_INFO("[CORE] [MIDDLEWARE] [PUBLISH TOPIC] [START]");
    for(auto const& subscriber: subcribers) {
        if(msg.compareTopic(subscriber.topic)) {
            subscriber.callback(msg);
        }
    }
    LOG_INFO("[CORE] [MIDDLEWARE] [PUBLISH TOPIC] [DONE]");
}

void Middleware::enqueueTask(const TaskDescription desc) {
    task_queue.push(desc);
}

void Middleware::registerTask(const TaskDescription desc) {
    rtos.createTask(desc);
}

void Middleware::run() {
    subcribers.erase(
        std::remove_if(subcribers.begin(), subcribers.end(), [](const Subscriber& sub) {
            return sub.delete_flag;
        }),
        subcribers.end()
    );  
    while(!task_queue.empty()) {
        registerTask(task_queue.front());
        task_queue.pop();
    }
    rtos.startScheduler();
}

}