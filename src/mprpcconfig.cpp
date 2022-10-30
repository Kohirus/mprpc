#include "mprpcconfig.hpp"
#include <iostream>
#include <string>

void MprpcConfig::LoadConfigFile(const char* config_file) {
    FILE* pf = fopen(config_file, "r");
    if (nullptr == pf) {
        std::cout << config_file << " is not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!feof(pf)) {
        char buf[512] = { 0 };
        fgets(buf, sizeof(buf), pf);

        // 去掉字符串前多余的空格
        std::string read_buf(buf);
        Trim(read_buf);

        // 判断 # 处的注释
        if (read_buf[0] == '#' || read_buf.empty()) {
            continue;
        }

        // 解析配置项
        int idx = read_buf.find('=');
        if (idx == -1) {
            // 配置项不合法
            continue;
        }

        std::string key, value;
        key = read_buf.substr(0, idx);
        Trim(key);
        int end_idx = read_buf.find('\n', idx);
        value       = read_buf.substr(idx + 1, end_idx - idx - 1);
        Trim(value);
        _configMap.insert({ key, value });
    }
}

std::string MprpcConfig::Load(const std::string& key) {
    auto it = _configMap.find(key);
    if (it != _configMap.end()) {
        return it->second;
    }
    return "";
}

void MprpcConfig::Trim(std::string& src_buf) {
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1) {
        // 说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    // 去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1) {
        // 说明字符串后面有空格
        src_buf = src_buf.substr(0, idx + 1);
    }
}
