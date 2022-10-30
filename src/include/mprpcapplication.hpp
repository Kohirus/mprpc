#ifndef __MPRPC_APPLICATION_H__
#define __MPRPC_APPLICATION_H__

#include "mprpcconfig.hpp"

/// @brief mprpc 框架的初始化类
class MprpcApplication {
public:
    // 初始化配置
    static void Init(int argc, char* argv[]);
    // 获取单例对象
    static MprpcApplication& GetInstance();
    // 获取配置
    static MprpcConfig& GetConfig();

private:
    MprpcApplication() {}
    MprpcApplication(const MprpcApplication&)            = delete;
    MprpcApplication& operator=(const MprpcApplication&) = delete;

private:
    static MprpcConfig _config;
};

#endif // !__MPRPC_APPLICATION_H__
