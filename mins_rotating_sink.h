#ifndef MINI_ROTATING_SINK_H_
#define MINI_ROTATING_SINK_H_

/**
 * @brief 扩展开源库spdlog的写日志功能，支持按指定时间间隔，轮转写日志，保证每条日志实时刷新至磁盘
 * 如:  日志文件名前缀为 traffic-user-profile，文件存储间隔60s，
 *      那么若当前时间为2023-04-19-15-28
 *      则当前正在写的文件为 traffic-user-profile
 *      而上一分钟写的文件名 traffic-user-profile_2023-04-19-15-27
 * 
 * 原理： 时间达到时，阻塞写，将traffic-user-profile 改名为 traffic-user-profile_2023-04-19-15-27, 然后新建traffic-user-profile继续写
 */

#include <spdlog/common.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/fmt/fmt.h>

#include <chrono>
#include <mutex>
#include <sys/stat.h>

namespace spdlog {
namespace sinks {
static const int MAX_SUFFIX_FILE_NUM = 100;

struct mins_rotating_filename_calculator
{
  static filename_t calc_filename(const filename_t &filename, const tm &now_tm)
  {
    filename_t basename, ext;
    std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
    return fmt_lib::format(SPDLOG_FILENAME_T("{}_{:04d}-{:02d}-{:02d}-{:02d}-{}"), basename, now_tm.tm_year + 1900, now_tm.tm_mon + 1,
        now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min, ext);
  }
};

template<typename Mutex, typename FileNameCalc = mins_rotating_filename_calculator>
class mins_rotating_sink final : public base_sink<Mutex>
{
public:
  mins_rotating_sink( filename_t base_filename, 
                        int interval_mins,
                        bool truncate = false, 
                        const file_event_handlers &event_handlers = {})
    : base_filename_(base_filename),
      truncate_(truncate),
      file_helper_(event_handlers),
      interval_mins_(interval_mins)

  {
    auto now = log_clock::now();
    file_helper_.open(base_filename_, truncate_);
    rotation_tp_ = next_rotation_tp_();
    remove_init_file_ = file_helper_.size() == 0;
  }

  filename_t filename()
  {
      std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
      return file_helper_.filename();
  }

protected:
  void sink_it_(const details::log_msg &msg) override
  {
      auto time = msg.time;
      bool should_rotate = time >= rotation_tp_;
      if (should_rotate)
      {
          flush_();
          file_helper_.close();
          if (remove_init_file_)
          {
              details::os::remove(file_helper_.filename());
          }
          auto filename = FileNameCalc::calc_filename(base_filename_, now_tm(time));

          // 当应用重启时，move文件为包含日期后缀时，会判断该后缀文件时候存在
          int suffix = 0;
          struct stat buffer;
          filename_t new_filename = filename;
          while (suffix < MAX_SUFFIX_FILE_NUM) {
            if (suffix != 0) {
              filename_t new_filename = fmt::format("{}.{}", filename, suffix);
            }
            // 添加like or unlikely帮助分支预测

            if (stat(new_filename.c_str(), &buffer) != 0) {
              std::rename(base_filename_.c_str(), new_filename.c_str());
              break;
            }
            suffix++;
          }
          file_helper_.open(base_filename_, truncate_);
          rotation_tp_ = next_rotation_tp_();
      }
      remove_init_file_ = false;
      memory_buf_t formatted;
      base_sink<Mutex>::formatter_->format(msg, formatted);
      file_helper_.write(formatted);
      flush_();
  }

  void flush_() override
  {
      file_helper_.flush();
  }

private:
  tm now_tm(log_clock::time_point tp)
  {
      time_t tnow = log_clock::to_time_t(tp);
      return spdlog::details::os::localtime(tnow);
  }

  log_clock::time_point next_rotation_tp_()
  {
      auto now = log_clock::now();
      tm date = now_tm(now);
      date.tm_sec = 0;
      auto rotation_time = log_clock::from_time_t(std::mktime(&date));
      if (rotation_time > now)
      {
          return rotation_time;
      }
      return {rotation_time + std::chrono::minutes(interval_mins_)};
  }
private:
    filename_t base_filename_;
    log_clock::time_point rotation_tp_;
    details::file_helper file_helper_;
    int interval_mins_;
    bool truncate_;
    bool remove_init_file_;
};

using mins_rotating_sink_mt = mins_rotating_sink<std::mutex>;
using mins_rotating_sink_st = mins_rotating_sink<details::null_mutex>;

} // namespace sinks
} // namespace spdlog

#endif // MINI_ROTATING_SINK_H_
