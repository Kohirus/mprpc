#include "logger.hpp"
#include <iostream>
#include <time.h>

Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    // 启动写日志线程
    std::thread writeLogTask([&]() {
        for (;;) {
            // 获取当前日期 取日志信息 写入相应的日志文件
            time_t now   = time(nullptr);
            tm*    nowtm = localtime(&now);
            char   file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);

            FILE* pf = fopen(file_name, "a+");
            if (pf == nullptr) {
                std::cout << "Logger file: " << file_name << " Open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = _lckQue.Pop();

            char time_buf[128] = { 0 };
            sprintf(time_buf, "%d:%d:%d => ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
            msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    // 设置分离线程 守护线程
    writeLogTask.detach();
}

void Logger::SetLogLevel(LogLevel level) {
    _logLevel = level;
}

void Logger::Log(const std::string& msg) {
    _lckQue.Push(msg);
}
