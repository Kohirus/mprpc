#ifndef __ZOOKEEPER_UTIL_H__
#define __ZOOKEEPER_UTIL_H__

#include <zookeeper/zookeeper.h>
#include <string>

class ZkClient{
public:
    ZkClient();
    ~ZkClient();
    // 链接 Zookeeper server
    void Start();
    // 在 Zookeeper Server 上根据指定的 path 创建 znode 节点
    // state 表示永久性节点还是临时性节点
    void Create(const char* path, const char* data, int datalen, int state = 0);
    // 根据指定的 znode 节点获取其值
    std::string GetData(const char* path);

private:
    zhandle_t *_handler;
};

#endif // !__ZOOKEEPER_UTIL_H__
