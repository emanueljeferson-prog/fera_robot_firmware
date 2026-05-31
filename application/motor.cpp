#include "motor.hpp"
#include <iostream>

namespace app {

Motor::Motor(uint8_t id, core::IMiddleware& middleware)
: id(id), middleware(middleware) {
    std::cout << "[APP] [MOTOR: " << (int)id << "] [START]" << std::endl;
} 
void Motor::init() {
    middle

}
void Motor::control() {

}
void Motor::readSpeed() {

}


}