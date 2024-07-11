// 实现一个输出到文件的静态类，方便全局调用
// 基于spdlog实现，写入到文件

#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

class DebugHelper {
  public:
    static void init(std::string name = "default", std::string level = "error") {
        auto log_level = getLevel(level);
        auto path = std::format("logs/{}/{}.log", level, name);
        auto logger = spdlog::basic_logger_mt("basic_logger", path);
        spdlog::set_default_logger(logger);
        spdlog::set_level(log_level);
        spdlog::set_pattern("[%l] %v");
    }

    static spdlog::level::level_enum getLevel(std::string &level) {
        if (level == "trace") {
            return spdlog::level::trace;
        } else if (level == "debug") {
            return spdlog::level::debug;
        } else if (level == "info") {
            return spdlog::level::info;
        } else if (level == "warn") {
            return spdlog::level::warn;
        } else if (level == "error") {
            return spdlog::level::err;
        } else if (level == "critical") {
            return spdlog::level::critical;
        } else {
            level = "off";
            return spdlog::level::off;
        }
    }
};
