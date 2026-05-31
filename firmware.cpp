#include <iostream>
#include "core/middleware.hpp"
#include "application/motor.hpp"

int main() {
    auto broker = core::Middleware(); 
    auto motor = app::Motor(1, broker);

    std::cout << "FERA ROBOT" << std::endl;
}