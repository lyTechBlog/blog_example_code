#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/dist_sink.h"
#include "mins_rotating_sink.h"

#include <map>

#define LOG_DEBUG(...) get_logger()->debug(__VA_ARGS__)
#define LOG_INFO(...) get_logger()->info(__VA_ARGS__)
#define LOG_WARN(...) get_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) get_logger()->error(__VA_ARGS__)

#define LOG_DEBUG_TT(...) get_logger_tt()->debug(__VA_ARGS__)
#define LOG_INFO_TT(...) get_logger_tt()->info(__VA_ARGS__)
#define LOG_WARN_TT(...) get_logger_tt()->warn(__VA_ARGS__)
#define LOG_ERROR_TT(...) get_logger_tt()->error(__VA_ARGS__)

static std::map<std::string, spdlog::level::level_enum> level_map = {
    {"trace", spdlog::level::level_enum::trace},
    {"debug", spdlog::level::level_enum::debug},
    {"info", spdlog::level::level_enum::info},
    {"warn", spdlog::level::level_enum::warn},
    {"error", spdlog::level::level_enum::err},
    {"critical", spdlog::level::level_enum::critical}
};

std::shared_ptr<spdlog::async_logger> & get_logger() {
  static std::shared_ptr<spdlog::async_logger> logger;
  return logger;
}

std::shared_ptr<spdlog::async_logger> & get_logger_tt() {
  static std::shared_ptr<spdlog::async_logger> logger_tt;
  return logger_tt;
}

void initLogger(const std::string& log_path, uint16_t get_file_interval_mins, const std::string& log_level,
                const std::string& log_path_tt, uint16_t get_file_interval_mins_tt, const std::string& log_level_tt)
{
  spdlog::init_thread_pool(10000000, 2);
  spdlog::flush_on(spdlog::level::debug);

  // 初始化 logger 
  auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_st>();
  auto daily_file = std::make_shared<spdlog::sinks::mins_rotating_sink_mt> (log_path, get_file_interval_mins, false);
  dist_sink->add_sink(daily_file);
  
  std::shared_ptr<spdlog::async_logger> & logger = get_logger(); 
  logger = std::make_shared<spdlog::async_logger>("", dist_sink, spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest); 
  logger->set_level(level_map[log_level]);

  // 初始化 logger_tt
  auto dist_sink_tt = std::make_shared<spdlog::sinks::dist_sink_st>();
  auto daily_file_tt = std::make_shared<spdlog::sinks::mins_rotating_sink_mt> (log_path_tt, get_file_interval_mins_tt, false);
  dist_sink_tt->add_sink(daily_file_tt);
  
  std::shared_ptr<spdlog::async_logger> & logger_tt = get_logger_tt();
  logger_tt = std::make_shared<spdlog::async_logger>("", dist_sink_tt, spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);  
  logger_tt->set_level(level_map[log_level_tt]);
}

int main() {
  initLogger("./log", 1, "debug", "./log_tt", 1, "debug");
  int thead_num = 1;

  // 测试多线程写日志，建立线程放入vector中, 然后join
  std::string str;
  for (int i = 0; i < 1000; i++) {
    str += "a";
  }

  std::vector<std::thread> threads;

  for (int i = 0; i < thead_num; ++i) {
    std::thread t([i, str]() {
      for (int j = 0; j < 90000 * 10000; ++j) {
        LOG_INFO("test info {} {} {}", i, j, str);
        LOG_INFO_TT("test info tt {} {} {}", i, j, str);
      }
    });
    threads.push_back(std::move(t));
  }

  for (auto &t : threads) {
    t.join();
  }
}