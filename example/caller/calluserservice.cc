#include <iostream>
#include "mprpcapplication.hpp"
#include "user.pb.h"

int main(int argc, char* argv[]) {
    MprpcApplication::Init(argc, argv);

    // fixbug::UserServiceRpc_Stub stud(RpcChannel);

    return 0;
}
