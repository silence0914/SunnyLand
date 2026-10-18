//
// Created by wangm on 26-10-18.
//

#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H
#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

namespace mylog {
class LogManager final {
public:
    LogManager(const std::string& name) {
        // 1. 创建 Sink (接收器)
        // 控制台 Sink (带颜色)
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // 循环文件 Sink 每天凌晨换日志文件
        auto rotating_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(name, 0, 0);

        // 2. 可选：为不同的 Sink 设置不同的格式
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        rotating_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread:%t] %v");

        // 3. 将多个 Sink 组合成一个 Logger
        std::vector<spdlog::sink_ptr> sinks{console_sink, rotating_sink};
        logger_ = std::make_shared<spdlog::logger>("name", sinks.begin(), sinks.end());

        // 5. 注册该 Logger 并设置为默认
        spdlog::register_logger(logger_);
        spdlog::set_default_logger(logger_);
    }

    ~LogManager() = default;

private:
    std::shared_ptr<spdlog::logger> logger_;
};
} // mylog

#endif //LOG_MANAGER_H
