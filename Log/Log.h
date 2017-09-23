/*
 * =============================================================================
 *
 *       Filename:  LoggerHelper.h
 *        Created:  07/29/17 10:41:28
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =============================================================================
 */

#pragma once

#include <unistd.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ostream>
#include <iostream>

class LoggerHelper {
 public:
  typedef char TimeBuffer[128];
  explicit LoggerHelper(std::ostream& os,
                        const char* file,
                        int line,
                        const char* func)
      : os_(os) {
    TimeBuffer buffer = {0};
    os_ << "[" << format_time(buffer) << " " << const_basename(file) << " "
        << func << ":" << line << "] ";
  }

  ~LoggerHelper() { os_ << std::endl; }

  std::ostream& stream() { return os_; }

 private:
  static const char* const_basename(const char* filename) {
    const char* p = strrchr(filename, '/');
    if (p == NULL) {
      return filename;
    } else {
      ++p;
      return p;
    }
  }

  static const char* format_time(TimeBuffer buffer) {
    int tid = syscall(SYS_gettid);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm storage;
    struct tm* t = localtime_r(&tv.tv_sec, &storage);
    const size_t offset = 64;
    const size_t len = 64;
    size_t n = strftime(buffer + offset, len, "%Y-%m-%d %H:%M:%S.%%06u %%d", t);
    assert(n < len);
    buffer[n] = '\0';

    n = snprintf(buffer, len, buffer + offset, tv.tv_usec, tid);
    buffer[n] = '\0';
    return buffer;
  }
  std::ostream& os_;
};

#ifndef SLOG
#define SLOG(S) LoggerHelper(S, __FILE__, __LINE__, __FUNCTION__).stream()
#endif

#ifndef LOG
#define LOG SLOG(std::cout)
#endif
