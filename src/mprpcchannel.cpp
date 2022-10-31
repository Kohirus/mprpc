#include "mprpcchannel.hpp"
#include "rpcheader.pb.h"
#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "mprpcapplication.hpp"

void MprpcChannel::CallMethod(
    const google::protobuf::MethodDescriptor* method,
    google::protobuf::RpcController*          controller,
    const google::protobuf::Message*          request,
    google::protobuf::Message*                response,
    google::protobuf::Closure*                done) {
    // 获取服务描述对象
    const google::protobuf::ServiceDescriptor* sd = method->service();
    // 获取服务名和方法名
    std::string service_name = sd->name();
    std::string method_name  = method->name();

    // 获取参数的序列化字符串长度 args_size
    std::string args_str;
    int         args_size = 0;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        std::cout << "Failed to serialize request!" << std::endl;
    }

    // 定义RPC的请求Header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    std::string rpc_header_str;
    uint32_t    header_size = 0;
    if (rpcHeader.SerializeToString(&rpc_header_str)) {
        header_size = rpc_header_str.size();
    } else {
        std::cout << "Failed to serialize rpc header!" << std::endl;
    }

    // 组织待发送的 rpc 请求的字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    std::cout << "================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "================================================" << std::endl;

    // 使用简单的 TCP 编程 完成 RPC 方法的远程调用即可
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        std::cout << "Failed to create socket! Errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string ip   = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t    port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        std::cout << "Failed to connect! Errno: " << errno << std::endl;
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    // 发送 RPC 请求
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0)) {
        std::cout << "Failed to send rpc str! Errno: " << errno << std::endl;
        close(clientfd);
        return;
    }

    // 接受 RPC 的响应值
    char recv_buf[1024] = { 0 };
    int  recv_size      = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))) {
        std::cout << "Failed to recv! Errno: " << errno << std::endl;
        close(clientfd);
        return;
    }

    // 反序列化 rpc 调用的响应数据
    std::string response_str(recv_buf, 0, recv_size);
    if (response->ParseFromString(response_str)) {
        std::cout << "Failed to parse response str: " << response_str << std::endl;
        close(clientfd);
        return;
    }

    close(clientfd);
}
