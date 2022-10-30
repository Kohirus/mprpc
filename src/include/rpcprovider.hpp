#ifndef __RPC_PROVIDER_H__
#define __RPC_PROVIDER_H__

#include <google/protobuf/service.h>
#include <memory>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/Buffer.h>

/// @brief 框架提供的专门服务发布 RPC 服务的网络对象类
class RpcProvider {
public:
    // 供外部使用 以发布 RPC 方法的函数接口
    void NotifyService(google::protobuf::Service* service);

    // 启动 RPC 服务调用，开始提供 RPC 远程网络服务调用
    void Run();
    
private:
    // 回调函数: 处理连接事件
    void OnConnention(const muduo::net::TcpConnectionPtr&);
    // 回调函数: 处理读写事件
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

private:
    muduo::net::EventLoop _eventLoop;
};

#endif // !__RPC_PROVIDER_H__
