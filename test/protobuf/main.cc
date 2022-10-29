#include "test.pb.h"
#include <iostream>
using namespace std;
using namespace fixbug;

int main(int argc, char* argv[]) {
    // LoginResponse rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("Failed to login!");

    GetFrinedListsResponse rsp;
    ResultCode*            rc = rsp.mutable_result();
    rc->set_errcode(0);

    User* user1 = rsp.add_friend_list();
    user1->set_name("Zhang San");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    User* user2 = rsp.add_friend_list();
    user2->set_name("Li Si");
    user2->set_age(22);
    user2->set_sex(User::WOMAN);

    cout << rsp.friend_list_size() << endl;
    return 0;
}

int main1(int argc, char* argv[]) {
    LoginRequest req;
    req.set_name("Zhang San");
    req.set_pwd("123456");

    // 序列化
    string send_str;
    if (req.SerializeToString(&send_str)) {
        cout << send_str << endl;
    }

    // 反序列化
    LoginRequest reqB;
    if (reqB.ParseFromString(send_str)) {
        cout << reqB.name() << endl;
        cout << reqB.pwd() << endl;
    }
    return 0;
}
