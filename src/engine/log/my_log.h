//
// Created by wangm on 26-10-18.
//

#ifndef MY_logH
#define MY_logH

#include <string_view>
#include <source_location>
#include <spdlog/spdlog.h>

namespace mylog {
class MyLog final {
public:
    // 构造函数 noexcept，并使用 string_view 零拷贝存储
    explicit MyLog(const std::source_location& source = std::source_location::current()) noexcept
        : file_(source.file_name()),
          func_(source.function_name()),
          line_(static_cast<int>(source.line())) {
        // 优化：在构造时一次性提取纯文件名，避免每次打日志都去搜索 '/'
        std::string_view sv(file_);
        const size_t pos = sv.find_last_of("/\\");
        filename_only_ = pos != std::string_view::npos ? sv.substr(pos + 1) : sv;

        const size_t end = func_.find_first_of("(");
        const size_t start = func_.rfind(":", end);
        func_ = func_.substr(start + 1,end - start -1);
    }

    ~MyLog() = default;

    // 禁止拷贝赋值，避免误用导致开销
    MyLog(const MyLog&) = delete;
    MyLog& operator=(const MyLog&) = delete;

    // ==================== 对外暴露的日志接口 ====================

    template<typename... Args>
    void trace(fmt::format_string<Args...> fmt_str, Args&&... args) {
        log_ex(spdlog::level::trace, fmt_str, std::forward<Args>(args)...);
    }

    template<typename T>
    void trace(const T& msg) {
        log_ex(spdlog::level::trace, msg);
    }

    template<typename... Args>
    void debug(fmt::format_string<Args...> fmt_str, Args&&... args) {
        log_ex(spdlog::level::debug, fmt_str, std::forward<Args>(args)...);
    }

    template<typename T>
    void debug(const T& msg) {
        log_ex(spdlog::level::debug, msg);
    }

    template<typename... Args>
    void info(fmt::format_string<Args...> fmt_str, Args&&... args) {
        log_ex(spdlog::level::info, fmt_str, std::forward<Args>(args)...);
    }

    template<typename T>
    void info(const T& msg) {
        log_ex(spdlog::level::info, msg);
    }

    template<typename... Args>
    void warn(fmt::format_string<Args...> fmt_str, Args&&... args) {
        log_ex(spdlog::level::warn, fmt_str, std::forward<Args>(args)...);
    }

    template<typename T>
    void warn(const T& msg) {
        log_ex(spdlog::level::warn, msg);
    }

    template<typename... Args>
    void error(fmt::format_string<Args...> fmt_str, Args&&... args) {
        log_ex(spdlog::level::err, fmt_str, std::forward<Args>(args)...);
    }

    template<typename T>
    void error(const T& msg) {
        log_ex(spdlog::level::err, msg);
    }

    template<typename... Args>
    void critical(fmt::format_string<Args...> fmt_str, Args&&... args) {
        log_ex(spdlog::level::critical, fmt_str, std::forward<Args>(args)...);
    }

    template<typename T>
    void critical(const T& msg) {
        log_ex(spdlog::level::critical, msg);
    }

private:
    template<typename... Args>
    constexpr auto makeLogFormat(fmt::string_view fmt_str, Args... args) {
        return fmt::format(fmt::runtime(fmt_str), args...);
    }

    // 核心优化：统一的底层日志分发函数
    template<typename... Args>
    void log_ex(spdlog::level::level_enum lvl, fmt::format_string<Args...> fmt_str, Args&&... args) {
        if (!spdlog::should_log(lvl)) { return; }
        log(lvl, makeLogFormat(fmt_str, std::forward<Args>(args)...));
    }

    template<typename T>
    void log_ex(spdlog::level::level_enum lvl, const T& msg) {
        if (!spdlog::should_log(lvl)) { return; }
        log(lvl, msg);
    }

    template<typename T>
    void log(spdlog::level::level_enum lvl, const T& msg) {
        switch (lvl) {
            case spdlog::level::trace:
                spdlog::trace("[{} : {} : {}] : {} ", filename_only_, func_, line_, msg);
                break;
            case spdlog::level::debug:
                spdlog::debug("[{} : {} : {}] : {} ", filename_only_, func_, line_, msg);
                break;
            case spdlog::level::info:
                spdlog::info("[{} : {} : {}] : {} ", filename_only_, func_, line_, msg);
                break;
            case spdlog::level::warn:
                spdlog::warn("[{} : {} : {}] : {} ", filename_only_, func_, line_, msg);
                break;
            case spdlog::level::err:
                spdlog::error("[{} : {} : {}] : {} ", filename_only_, func_, line_, msg);
                break;
            case spdlog::level::critical:
                spdlog::critical("[{} : {} : {}] : {} ", filename_only_, func_, line_, msg);
                break;
            default:
                break;
        }
    }

private
:
    std::string_view file_;          // 完整路径 (由 source_location 提供)
    std::string_view filename_only_; // 纯文件名 (构造时提取)
    std::string_view func_;          // 函数名
    int line_;                       // 行号
};
} // namespace mylog


#endif //MY_logH
