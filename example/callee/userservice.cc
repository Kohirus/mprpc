#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.hpp"
#include "rpcprovider.hpp"

class UserService : public fixbug::UserServiceRpc {
public:
    bool Login(std::string name, std::string pwd) {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << ", pwd: " << pwd << std::endl;
        return true;
    }

    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id: " << id << ", name: " << name << ", pwd: " << pwd << std::endl;
        return true;
    }

    // 重写基类 UserServiceRpc 的虚函数
    /*
       1. caller --> Login(LoginRequest) --> muduo --> callee
       2. callee --> Login(LoginRequest) --> 如下重写的Login()方法
     */
    void Login(::google::protobuf::RpcController* controller,
        const ::fixbug::LoginRequest*             request,
        ::fixbug::LoginResponse*                  response,
        ::google::protobuf::Closure*              done) {
        // 框架给业务上报了请求参数 LoginRequest 应用获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd  = request->pwd();

        bool login_result = Login(name, pwd); // 本地业务

        // 把响应写入
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调操作
        done->Run();
    }

    void Register(::google::protobuf::RpcController* controller,
        const ::fixbug::RegisterRequest*             request,
        ::fixbug::RegisterResponse*                  response,
        ::google::protobuf::Closure*                 done) {
        uint32_t    id   = request->id();
        std::string name = request->name();
        std::string pwd  = request->pwd();

        bool ret = Register(id, name, pwd);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);

        done->Run();
    }
};

int main(int argc, char* argv[]) {
    MprpcApplication::Init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new UserService);

    provider.Run();
    return 0;
}
