#include "rpcprovider.hpp"
#include <functional>
#include <string>
#include "mprpcapplication.hpp"

void RpcProvider::NotifyService(google::protobuf::Service* service) {
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
