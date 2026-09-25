/**
 * @file libcomp/src/BaseLog.h
 * @ingroup libcomp
 *
 * @author COMP Omega <compomega@tutanota.com>
 *
 * @brief Routines to log messages to the console and/or a file.
 *
 * This file is part of the COMP_hack Library (libcomp).
 *
 * Copyright (C) 2012-2020 COMP_hack Team <compomega@tutanota.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBCOMP_SRC_BASELOG_H
#define LIBCOMP_SRC_BASELOG_H

// libcomp Includes
#include "CString.h"
#include "EnumMap.h"
#include "EnumUtils.h"
#include "MessageQueue.h"

// Standard C++11 Includes
#include <chrono>
#include <fstream>
#include <functional>
#include <list>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace libcomp {

/**
 * Integer type every log component enumeration is based on.
 */
typedef int GenericLogComponent_t;

/**
 * First log component ID that a server specific log component may use.
 */
#define LOG_SERVER_SPECIFIC_START_ID (1000)

/**
 * Log components that are common to all applications using libcomp.
 */
enum class BaseLogComponent_t : GenericLogComponent_t {
  Invalid = 0xFF,
  Connection = 0,
  Crypto,
  Database,
  DataStore,
  DataSyncManager,
  General,
  Packet,
  ScriptEngine,
  Server,
};

/**
 * Convert a string into a base log component.
 * @param comp String to convert.
 * @returns Log component the string represents or Invalid.
 */
BaseLogComponent_t StringToBaseLogComponent(const String& comp);

/**
 * Convert a base log component into a string. The General component has no
 * name so that messages for it are not prefixed with a component.
 * @param comp Log component to convert.
 * @returns String representation of the log component.
 */
String BaseLogComponentToString(GenericLogComponent_t comp);

class LogMessage;

/**
 * Logging interface capable of logging messages to the terminal or a file.
 * Messages are queued and written by a dedicated thread. Each log component
 * has its own log level (@ref SetLogLevel); the default is warning. An
 * application derives from this class to add its own log components and to
 * provide a singleton accessor (see libhack::Log).
 */
class BaseLog {
 public:
  /**
   * Level of a log message.
   */
  typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_COUNT,
  } Level_t;

  /**
   * Prototype of a log hook.
   * @param comp Log component of the message.
   * @param level Level of the message.
   * @param msg The formatted message.
   * @param pUserData User data passed to @ref AddLogHook.
   */
  typedef void (*Hook_t)(GenericLogComponent_t comp, Level_t level,
                         const String& msg, void* pUserData);

  /**
   * Stop the log thread and close the log file.
   */
  virtual ~BaseLog();

  /**
   * Get the log singleton (if one has been created).
   * @returns Pointer to the log singleton or nullptr.
   */
  static BaseLog* GetBaseSingletonPtr();

  /**
   * Queue a log message. The log takes ownership of the message.
   * @param pMessage Message to queue.
   */
  void LogMessage(libcomp::LogMessage* pMessage);

  /**
   * Path to the log file.
   * @returns Path to the log file or an empty string.
   */
  String GetLogPath() const;

  /**
   * Set the log file and open it.
   * @param path Path to the log file.
   * @param truncate If the log file should be truncated.
   */
  void SetLogPath(const String& path, bool truncate);

  /**
   * Add a log hook function.
   * @param func Hook function.
   * @param data User data to pass to the hook.
   */
  void AddLogHook(Hook_t func, void* data = nullptr);

  /**
   * Add a log hook lambda.
   * @param func Hook function.
   */
  void AddLogHook(const std::function<void(GenericLogComponent_t comp,
                                           Level_t level, const String& msg)>&
                      func);

  /**
   * Add a hook that writes messages to standard output.
   */
  void AddStandardOutputHook();

  /**
   * Remove all log hooks.
   */
  void ClearHooks();

  /**
   * Get the log level of a component.
   * @param comp Log component.
   * @returns Minimum level logged for the component.
   */
  Level_t GetLogLevel(GenericLogComponent_t comp) const;

  /**
   * Set the log level of a component.
   * @param comp Log component.
   * @param level Minimum level logged for the component.
   */
  void SetLogLevel(GenericLogComponent_t comp, Level_t level);

  /**
   * Check if log file lines have a timestamp.
   * @returns true if log file lines have a timestamp.
   */
  bool GetLogFileTimestampsEnabled() const;

  /**
   * Set if log file lines have a timestamp.
   * @param enabled If log file lines have a timestamp.
   */
  void SetLogFileTimestampsEnabled(bool enabled);

  /**
   * Check if log rotation is enabled.
   * @returns true if log rotation is enabled.
   */
  bool GetLogRotationEnabled() const;

  /**
   * Set if log rotation is enabled.
   * @param enabled If log rotation is enabled.
   */
  void SetLogRotationEnabled(bool enabled);

  /**
   * Check if rotated logs are compressed.
   * @returns true if rotated logs are compressed.
   */
  bool GetLogCompression() const;

  /**
   * Set if rotated logs are compressed.
   * @param enabled If rotated logs are compressed.
   */
  void SetLogCompression(bool enabled);

  /**
   * Get the number of rotated logs to keep.
   * @returns Number of rotated logs to keep.
   */
  int GetLogRotationCount() const;

  /**
   * Set the number of rotated logs to keep.
   * @param count Number of rotated logs to keep.
   */
  void SetLogRotationCount(int count);

  /**
   * Get the number of days between log rotations.
   * @returns Number of days between log rotations.
   */
  int GetLogRotationDays() const;

  /**
   * Set the number of days between log rotations.
   * @param days Number of days between log rotations.
   */
  void SetLogRotationDays(int days);

  /**
   * Check if a message should be logged.
   * @param comp Log component of the message.
   * @param level Level of the message.
   * @returns true if the message should be logged.
   */
  bool ShouldLog(GenericLogComponent_t comp, Level_t level) const;

  /**
   * Convert a string into a log component.
   * @param comp String to convert.
   * @returns Log component the string represents.
   */
  virtual GenericLogComponent_t StringToLogComponent(const String& comp) const;

  /**
   * Convert a log component into a string.
   * @param comp Log component to convert.
   * @returns String representation of the log component.
   */
  virtual String LogComponentToString(GenericLogComponent_t comp) const;

 protected:
  /**
   * Construct the log. Only one log may exist at a time.
   */
  BaseLog();

  /**
   * Format and write a message to the log file and hooks.
   * @param timestamp Time the message was created.
   * @param comp Log component of the message.
   * @param level Level of the message.
   * @param msg The message.
   */
  void LogMessage(const std::chrono::system_clock::time_point& timestamp,
                  GenericLogComponent_t comp, Level_t level,
                  const String& msg);

  /**
   * Rotate the log files.
   */
  void RotateLogs();

  /**
   * Thread function that writes queued messages.
   */
  void MessageLoop();

  /**
   * Move a file.
   * @param oldPath Path to the file.
   * @param newPath New path of the file.
   * @returns true on success.
   */
  static bool FileMove(const String& oldPath, const String& newPath);

  /**
   * Check if a file exists.
   * @param file Path to the file.
   * @returns true if the file exists.
   */
  static bool FileExists(const String& file);

  /**
   * Delete a file.
   * @param file Path to the file.
   * @returns true on success.
   */
  static bool FileDelete(const String& file);

  /// Path to the log file.
  String mLogPath;

  /// If the log file lines should have a timestamp.
  bool mLogFileTimestampEnabled;

  /// If log rotation is enabled.
  bool mLogRotationEnabled;

  /// If rotated logs should be compressed.
  bool mLogCompression;

  /// Number of rotated logs to keep.
  int mLogRotationCount;

  /// Number of days before the logs are rotated.
  int mLogRotationDays;

  /// Log file (if open).
  std::ofstream* mLogFile;

  /// Log hook functions and their user data.
  std::unordered_map<Hook_t, void*> mHooks;

  /// Log level of each component.
  std::unordered_map<GenericLogComponent_t, Level_t> mComponentLogLevels;

  /// Log hook lambdas.
  std::list<std::function<void(GenericLogComponent_t comp, Level_t level,
                               const String& msg)>>
      mLambdaHooks;

  /// Day the last message was logged on (for log rotation).
  int64_t mLastLog;

  /// Queue of messages to be written.
  MessageQueue<libcomp::LogMessage*> mMessages;

  /// Thread that writes messages.
  std::thread mThread;

  /// Console attributes before the log changed them (Windows only).
  uint16_t mConsoleAttributes;
};

/**
 * Message to be logged.
 */
class LogMessage {
 public:
  /**
   * Create a log message.
   * @param comp Log component of the message.
   * @param level Level of the message.
   */
  LogMessage(GenericLogComponent_t comp, BaseLog::Level_t level)
      : mComponent(comp),
        mLevel(level),
        mTimestamp(std::chrono::system_clock::now()) {}

  /**
   * Clean up the message.
   */
  virtual ~LogMessage();

  /**
   * Get the formatted message.
   * @returns The message.
   */
  virtual String GetMsg() const { return String(); }

  /**
   * If this message tells the log thread to stop.
   * @returns true if the log thread should stop.
   */
  virtual bool ShouldStop() const;

  /**
   * Get the log component of the message.
   * @returns Log component of the message.
   */
  GenericLogComponent_t GetComponent() const { return mComponent; }

  /**
   * Get the level of the message.
   * @returns Level of the message.
   */
  BaseLog::Level_t GetLevel() const { return mLevel; }

  /**
   * Get the time the message was created.
   * @returns Time the message was created.
   */
  std::chrono::system_clock::time_point GetTimestamp() const {
    return mTimestamp;
  }

 private:
  /// Log component of the message.
  GenericLogComponent_t mComponent;

  /// Level of the message.
  BaseLog::Level_t mLevel;

  /// Time the message was created.
  std::chrono::system_clock::time_point mTimestamp;
};

/**
 * Log message with a pre-formatted string.
 */
class LogMessageFixed : public LogMessage {
 public:
  /**
   * Create a log message.
   * @param comp Log component of the message.
   * @param level Level of the message.
   * @param msg The message.
   */
  LogMessageFixed(GenericLogComponent_t comp, BaseLog::Level_t level,
                  const String& msg);

  /**
   * Clean up the message.
   */
  virtual ~LogMessageFixed();

  String GetMsg() const override;

 private:
  /// The message.
  String mMessage;
};

/**
 * Log message that is formatted on the log thread by calling a function with
 * the stored arguments.
 */
template <typename... Args>
class LogMessageImpl : public LogMessage {
 public:
  /**
   * Create a log message.
   * @param comp Log component of the message.
   * @param level Level of the message.
   * @param f Function that returns the formatted message.
   * @param args Arguments to pass to the function.
   */
  template <typename Function>
  LogMessageImpl(GenericLogComponent_t comp, BaseLog::Level_t level,
                 Function&& f, Args&&... args)
      : LogMessage(comp, level),
        mFunction(std::forward<Function>(f)),
        mArgs(std::forward<Args>(args)...) {}

  /**
   * Clean up the message.
   */
  virtual ~LogMessageImpl() {}

  String GetMsg() const override {
    return Call(std::index_sequence_for<Args...>{});
  }

 private:
  template <std::size_t... I>
  String Call(std::index_sequence<I...>) const {
    return mFunction(std::get<I>(mArgs)...);
  }

  /// Function to format the message.
  std::function<String(const typename std::decay<Args>::type&...)> mFunction;

  /// Arguments to the function.
  std::tuple<typename std::decay<Args>::type...> mArgs;
};

}  // namespace libcomp

/**
 * Macro to create a log function for a base log component.
 * @param name Name of the function
 * @param comp Component the functions logs
 * @param level Log level the function logs
 */
#define BASE_LOG_FUNCTION(name, comp, level)                                 \
  static inline void name(const std::function<libcomp::String(void)>& fun) { \
    auto log = libcomp::BaseLog::GetBaseSingletonPtr();                      \
                                                                             \
    if (log &&                                                               \
        log->ShouldLog(to_underlying(libcomp::BaseLogComponent_t::comp),     \
                       level)) {                                             \
      auto msg = new libcomp::LogMessageFixed(                               \
          to_underlying(libcomp::BaseLogComponent_t::comp), level, fun());   \
      log->LogMessage(msg);                                                  \
    }                                                                        \
  }                                                                          \
                                                                             \
  template <typename Function, typename... Args>                             \
  static inline void name##Delayed(Function&& f, Args&&... args) {           \
    auto log = libcomp::BaseLog::GetBaseSingletonPtr();                      \
                                                                             \
    if (log &&                                                               \
        log->ShouldLog(to_underlying(libcomp::BaseLogComponent_t::comp),     \
                       level)) {                                             \
      auto msg = new libcomp::LogMessageImpl<Args...>(                       \
          to_underlying(libcomp::BaseLogComponent_t::comp), level,           \
          std::forward<Function>(f), std::forward<Args>(args)...);           \
      log->LogMessage(msg);                                                  \
    }                                                                        \
  }                                                                          \
                                                                             \
  static inline void name##Msg(const libcomp::String& _msg) {                \
    auto log = libcomp::BaseLog::GetBaseSingletonPtr();                      \
                                                                             \
    if (log &&                                                               \
        log->ShouldLog(to_underlying(libcomp::BaseLogComponent_t::comp),     \
                       level)) {                                             \
      auto msg = new libcomp::LogMessageFixed(                               \
          to_underlying(libcomp::BaseLogComponent_t::comp), level, _msg);    \
      log->LogMessage(msg);                                                  \
    }                                                                        \
  }

/**
 * Macro to create a set of log functions for a base log component.
 * @param comp Component to create the functions for
 */
#define BASE_LOG_FUNCTIONS(comp)                                          \
  BASE_LOG_FUNCTION(Log##comp##Debug, comp,                               \
                    libcomp::BaseLog::LOG_LEVEL_DEBUG)                    \
  BASE_LOG_FUNCTION(Log##comp##Info, comp,                                \
                    libcomp::BaseLog::LOG_LEVEL_INFO)                     \
  BASE_LOG_FUNCTION(Log##comp##Warning, comp,                             \
                    libcomp::BaseLog::LOG_LEVEL_WARNING)                  \
  BASE_LOG_FUNCTION(Log##comp##Error, comp,                               \
                    libcomp::BaseLog::LOG_LEVEL_ERROR)                    \
  BASE_LOG_FUNCTION(Log##comp##Critical, comp,                            \
                    libcomp::BaseLog::LOG_LEVEL_CRITICAL)

BASE_LOG_FUNCTIONS(Connection)
BASE_LOG_FUNCTIONS(Crypto)
BASE_LOG_FUNCTIONS(Database)
BASE_LOG_FUNCTIONS(DataStore)
BASE_LOG_FUNCTIONS(DataSyncManager)
BASE_LOG_FUNCTIONS(General)
BASE_LOG_FUNCTIONS(Packet)
BASE_LOG_FUNCTIONS(ScriptEngine)
BASE_LOG_FUNCTIONS(Server)

#endif  // LIBCOMP_SRC_BASELOG_H
