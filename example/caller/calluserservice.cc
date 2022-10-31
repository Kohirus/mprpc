#include <iostream>
#include "mprpcapplication.hpp"
#include "user.pb.h"
#include "mprpcchannel.hpp"

int main(int argc, char* argv[]) {
    MprpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest        request;
    request.set_name("Zhang San");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    // 同步RPC调用
    stub.Login(nullptr, &request, &response, nullptr);
    // 一次RPC调用完成 读调用的结果
    if (response.result().errcode() == 0) {
        std::cout << "RPC Login response success: " << response.success() << std::endl;
    } else {
        std::cout << "RPC Login response error: " << response.result().errmsg() << std::endl;
    }
    return 0;
}
