#include "mprpcprovider.hpp"
#include <functional>
#include <google/protobuf/message.h>
#include <google/protobuf/stubs/callback.h>
#include <string>
#include "mprpcapplication.hpp"
#include <google/protobuf/descriptor.h>
#include "rpcheader.pb.h"
#include "logger.hpp"
#include "zookeeperutil.hpp"

void RpcProvider::NotifyService(google::protobuf::Service* service) {
    ServiceInfo service_info;
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();
    // 获取服务的名字
    std::string service_name = pserviceDesc->name();
    // 获取服务数量
    int methodCnt = pserviceDesc->method_count();

    LOG_INFO("service name: %s", service_name.c_str());

    for (int i = 0; i < methodCnt; i++) {
        // 获取服务方法的名字
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string                               method_name = pmethodDesc->name();
        service_info._methodMap.insert({ method_name, pmethodDesc });
        LOG_INFO("method_name: %s", method_name.c_str());
    }
    service_info._service = service;
    _serviceMap.insert({ service_name, service_info });
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

    // 将当前RPC节点上要发布的服务器全部注册到zk上 让 RPC Client 可以从zk上发现服务
    ZkClient zkCli;
    zkCli.Start();
    // service_name 为永久性节点 method_name 为临时性节点
    for (auto& sp : _serviceMap) {
        // /service_name
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto& mp : sp.second._methodMap) {
            // /service_name/method_name
            std::string method_path           = service_path + "/" + mp.first;
            char        method_path_data[128] = { 0 };
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    std::cout << "RpcProvider start service at IP[" << ip << "]:Port[" << port << "]" << std::endl;
    LOG_INFO("RpcProvider start service at IP[%s]:Port[%d]", ip.c_str(), port);

    // 启动网络服务
    server.start();
    _eventLoop.loop();
}

void RpcProvider::OnConnention(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        conn->shutdown();
    }
}

/*
 * ————————————————————————————————————————————————————————————————————————————————
 * |  header_size(4)  |                   rpc_head_str               |  args_str  |
 * ————————————————————————————————————————————————————————————————————————————————
 * |  header_size(4)  |  service_name  |  method_name  |  args_size  |  args_str  |
 * ————————————————————————————————————————————————————————————————————————————————
 **/
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,
    muduo::net::Buffer* buffer, muduo::Timestamp time) {
    // 网络上接受的远程 RPC 调用请求的字符流
    std::string recv_buf = buffer->retrieveAllAsString();

    // 读取前 4 字节
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    // 根据 header_size 读取数据头的原始字符流
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    // 反序列化数据 得到 RPC 请求的详细信息
    mprpc::RpcHeader rpcHeader;
    std::string      service_name, method_name;
    uint32_t         args_size = 0;
    if (rpcHeader.ParseFromString(rpc_header_str)) {
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name  = rpcHeader.method_name();
        args_size    = rpcHeader.args_size();
    } else {
        // 数据头反序列化失败
        LOG_ERR("rpc_header_str: %s parse error!", rpc_header_str.c_str());
    }

    // 获取 RPC 方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    std::cout << "================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "================================================" << std::endl;

    // 获取 service 对象和 method 对象
    auto svc_it = _serviceMap.find(service_name);
    if (svc_it == _serviceMap.end()) {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto met_it = svc_it->second._methodMap.find(method_name);
    if (met_it == svc_it->second._methodMap.end()) {
        std::cout << service_name << ":" << method_name << " is not exist!" << std::endl;
        return;
    }

    google::protobuf::Service*                service = svc_it->second._service;
    const google::protobuf::MethodDescriptor* method  = met_it->second;

    // 生成RPC方法调用的请求Request和响应Response参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        std::cout << "request parse error, content: " << args_str << std::endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // 创建回调函数
    google::protobuf::Closure* done = google::protobuf::NewCallback<
        RpcProvider,
        const muduo::net::TcpConnectionPtr&,
        google::protobuf::Message*>(this, &RpcProvider::sendRpcResponse, conn, response);

    // 根据远端RPC请求，调用当前RPC节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    // 对 response 进行序列化
    if (response->SerializeToString(&response_str)) {
        // 序列化成功后 通过网络把 RPC 方法执行的结果发送回 RPC 的调用方
        conn->send(response_str);
    } else {
        std::cout << "Failed to serialize response_str!" << std::endl;
    }
    conn->shutdown(); // 模拟HTTP的短链接服务 由RpcProvider主动断开链接
}
