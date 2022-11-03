#include "zookeeperutil.hpp"
#include "mprpcapplication.hpp"
#include <iostream>
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include "logger.hpp"
#include <stdio.h>

void g_watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx) {
    if (type == ZOO_SESSION_EVENT) {        // 判断回调的消息类型是否是会话相关的消息类型
        if (state == ZOO_CONNECTED_STATE) { // 判断 zkclient 与 zkserver 是否连接成功
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient()
    : _handler(nullptr) {
}

ZkClient::~ZkClient() {
    if (_handler) {
        zookeeper_close(_handler);
    }
}

void ZkClient::Start() {
    std::string host    = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port    = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;

    /*
     * zookeeeper_mt: 多线程版本 zookeeper_st: 单线程版本
     * 有如下三个线程:
     * 1. API 调用线程
     * 2. 网络 I/O 线程(poll)
     * 3. watcher 回调线程
     **/
    _handler = zookeeper_init(connstr.c_str(), g_watcher, 30000, nullptr, nullptr, 0);
    // 此处并不表示与 zk Server 链接成功 仅仅表示资源初始化完成
    // 如上的 g_watcher 回调函数则是用来判断 zkclient 和 zkserver 是否连接成功
    // 如果 zkserver 返回消息，则会调用该回调函数以改变信号量 唤醒当前线程
    if (nullptr == _handler) {
        LOG_ERR("Failed to initialize zookeeper!");
        exit(EXIT_FAILURE);
    }
    // 此处使用信号量来阻塞监听 zk server 链接成功
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(_handler, &sem);
    // 阻塞直到 zk server 响应链接创建成功
    sem_wait(&sem);
    LOG_INFO("Initialize zookeeper successfully!");
}

void ZkClient::Create(const char* path, const char* data, int datalen, int state) {
    char path_buffer[128];
    int  bufferlen = sizeof(path_buffer);
    int  flag;
    // 先判断 path 表示的 znode 节点是否存在 如果存在 就无需重复创建
    flag = zoo_exists(_handler, path, 0, nullptr);
    if (ZNONODE == flag) {
        // 创建指定 path 的 znode 节点
        flag = zoo_create(_handler, path, data, datalen,
            &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if (flag == ZOK) {
            LOG_INFO("Create znode successfully! Path = %s", path);
        } else {
            LOG_ERR("Failed to create znode! Path = %s, Flag = %d", path, flag);
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZkClient::GetData(const char* path) {
    char buffer[64];
    int  bufferlen = sizeof(buffer);
    int  flag      = zoo_get(_handler, path, 0, buffer, &bufferlen, nullptr);
    if (flag != ZOK) {
        LOG_ERR("Failed to get value of znode! Path = %s", path);
        return "";
    }
    return buffer;
}
