#include "rpcprovider.hpp"
#include <functional>
#include <string>
#include "mprpcapplication.hpp"
#include <google/protobuf/descriptor.h>

void RpcProvider::NotifyService(google::protobuf::Service* service) {
    ServiceInfo service_info;
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();
    // 获取服务的名字
    std::string service_name = pserviceDesc->name();
    // 获取服务数量
    int methodCnt = pserviceDesc->method_count();

    std::cout << "service_name: " << service_name << std::endl;

    for (int i = 0; i < methodCnt; i++) {
        // 获取服务方法的名字
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string                               method_name = pmethodDesc->name();
        service_info._methodMap.insert({ method_name, pmethodDesc });
        std::cout << "method_name: " << method_name << std::endl;
    }
    service_info._service = service;
    _serviceMap.insert({service_name, service_info});
}

void RpcProvider::Run() {
    std::string ip   = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t    port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    muduo::net::InetAddress address(ip, port);
    // 创建 TcpServer 对象
    muduo::net::TcpServer server(&_eventLoop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调方法
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnention, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));
    // 设置muduo库线程数量
    server.setThreadNum(4);

    std::cout << "RpcProvider start service at IP[" << ip << "]:Port[" << port << "]" << std::endl;

    // 启动网络服务
    server.start();
    _eventLoop.loop();
}

void RpcProvider::OnConnention(const muduo::net::TcpConnectionPtr&) {
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp) {
}
