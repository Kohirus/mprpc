#include <iostream>
#include "mprpcapplication.hpp"
#include "friend.pb.h"
#include "mprpcchannel.hpp"

int main(int argc, char* argv[]) {
    MprpcApplication::Init(argc, argv);

    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    fixbug::GetFriendsListRequest request;
    request.set_userid(1000);
    fixbug::GetFriendsListResponse response;
    // 同步RPC调用
    stub.GetFriendList(nullptr, &request, &response, nullptr);

    // 一次RPC调用完成 读调用的结果
    if (response.result().errcode() == 0) {
        std::cout << "RPC GetFriendsList response success: " << std::endl;
        int size = response.friends_size();
        for (int i = 0; i < size; i++) {
            std::cout << "[" << i + 1 << "] " << response.friends(i) << std::endl;
        }
    } else {
        std::cout << "RPC GetFriendsList response error: " << response.result().errmsg() << std::endl;
    }

    return 0;
}
