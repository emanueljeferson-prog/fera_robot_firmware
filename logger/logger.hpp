#pragma once

#include <cstdarg>
#include <cstdio>
#include <string>

#ifdef DEBUG

inline void log_info(const char* msg) {
    printf("[INFO] %s\n", msg);
}

inline void log_info(const std::string& msg) {
    printf("[INFO] %s\n", msg.c_str());
}

template<typename... Args>
inline void log_info(const char* fmt, Args... args) {
    printf("[INFO] ");
    printf(fmt, args...);
    printf("\n");
}

inline void log_warn(const char* msg) {
    printf("[WARN] %s\n", msg);
}

inline void log_warn(const std::string& msg) {
    printf("[WARN] %s\n", msg.c_str());
}

template<typename... Args>
inline void log_warn(const char* fmt, Args... args) {
    printf("[WARN] ");
    printf(fmt, args...);
    printf("\n");
}

inline void log_error(const char* msg) {
    printf("[ERROR] %s\n", msg);
}

inline void log_error(const std::string& msg) {
    printf("[ERROR] %s\n", msg.c_str());
}

template<typename... Args>
inline void log_error(const char* fmt, Args... args) {
    printf("[ERROR] ");
    printf(fmt, args...);
    printf("\n");
}

#define LOG_INFO(...)  log_info(__VA_ARGS__)
#define LOG_WARN(...)  log_warn(__VA_ARGS__)
#define LOG_ERROR(...) log_error(__VA_ARGS__)

#else

#define LOG_INFO(...)  ((void)0)
#define LOG_WARN(...)  ((void)0)
#define LOG_ERROR(...) ((void)0)

#endif