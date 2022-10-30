#ifndef __MPRPC_CONFIG_H__
#define __MPRPC_CONFIG_H__

#include <unordered_map>
#include <string>

// 读取配置文件
class MprpcConfig {
public:
    // 负责解析加载peizhiwenj
    void LoadConfigFile(const char* config_file);
    // 查询配置项信息
    std::string Load(const std::string& key);

private:
    // 去掉字符串前后的空格
    void Trim(std::string& src_buf);

private:
    std::unordered_map<std::string, std::string> _configMap;
};

#endif // !__MPRPC_CONFIG_H__
