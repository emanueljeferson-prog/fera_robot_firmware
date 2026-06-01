#pragma once

#include "message.hpp"

namespace core {

struct Subscriber
{
    Callback callback;
    Topics topic;     
    bool delete_flag; 
};

}