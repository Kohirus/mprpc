#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "lockqueue.hpp"

enum LogLevel {
    INFO,  // 普通信息
    ERROR, // 错误消息
};

// Mprpc框架提供的日志系统
class Logger {
public:
    // 获取日志单例
    static Logger& GetInstance();
    // 设置日志级别
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(const std::string& msg);

private:
    Logger();
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

private:
    // 日志等级
    int _logLevel;
    // 日志缓存队列
    LockQueue<std::string> _lckQue;
};

#define LOG_INFO(logmsgformat, ...)                          \
    do {                                                     \
        Logger& logger = Logger::GetInstance();               \
        logger.SetLogLevel(INFO);                            \
        char c[1024] = { 0 };                                \
        snprintf(c, sizeof(c), logmsgformat, ##__VA_ARGS__); \
        logger.Log(c);                                       \
    } while (0);

#define LOG_ERR(logmsgformat, ...)                          \
    do {                                                     \
        Logger& logger = Logger::GetInstance();               \
        logger.SetLogLevel(ERROR);                            \
        char c[1024] = { 0 };                                \
        snprintf(c, sizeof(c), logmsgformat, ##__VA_ARGS__); \
        logger.Log(c);                                       \
    } while (0);

#endif // !__LOGGER_H__
