/**
 * @file libcomp/src/BaseLog.cpp
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

#include "BaseLog.h"

// libcomp Includes
#include "EnumMap.h"

// Standard C++11 Includes
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <shlwapi.h>
#include <wincon.h>
#include <windows.h>
#else
#include <unistd.h>
#endif  // _WIN32

#include <zlib.h>

using namespace libcomp;

/// Singleton pointer for the log.
static BaseLog* gLogInst = nullptr;

/// Mapping of base log components to their string names.
static EnumMap<BaseLogComponent_t, String> gBaseLogComponentMapping = {
    {BaseLogComponent_t::Connection, "Connection"},
    {BaseLogComponent_t::Crypto, "Crypto"},
    {BaseLogComponent_t::Database, "Database"},
    {BaseLogComponent_t::DataStore, "DataStore"},
    {BaseLogComponent_t::DataSyncManager, "DataSyncManager"},
    {BaseLogComponent_t::General, "General"},
    {BaseLogComponent_t::Packet, "Packet"},
    {BaseLogComponent_t::ScriptEngine, "ScriptEngine"},
    {BaseLogComponent_t::Server, "Server"},
};

namespace libcomp {

/**
 * Message placed on the queue to stop the log thread.
 */
class LogMessageStop : public LogMessage {
 public:
  LogMessageStop()
      : LogMessage(to_underlying(BaseLogComponent_t::General),
                   BaseLog::LOG_LEVEL_CRITICAL) {}

  virtual ~LogMessageStop() {}

  bool ShouldStop() const override { return true; }
};

}  // namespace libcomp

BaseLogComponent_t libcomp::StringToBaseLogComponent(const String& comp) {
  for (auto pair : gBaseLogComponentMapping) {
    if (pair.second == comp) {
      return pair.first;
    }
  }

  return BaseLogComponent_t::Invalid;
}

String libcomp::BaseLogComponentToString(GenericLogComponent_t comp) {
  // The general component is not written as a prefix.
  if (to_underlying(BaseLogComponent_t::General) == comp) {
    return String();
  }

  auto match = gBaseLogComponentMapping.find((BaseLogComponent_t)comp);

  if (gBaseLogComponentMapping.end() != match) {
    return match->second;
  }

  return "Unknown";
}

/*
 * Black       0;30     Dark Gray     1;30
 * Blue        0;34     Light Blue    1;34
 * Green       0;32     Light Green   1;32
 * Cyan        0;36     Light Cyan    1;36
 * Red         0;31     Light Red     1;31
 * Purple      0;35     Light Purple  1;35
 * Brown       0;33     Yellow        1;33
 * Light Gray  0;37     White         1;37
 */

/**
 * Log hook to send all log messages to standard output. This hook will color
 * all log messages depending on their log level.
 * @param comp Log component of the message.
 * @param level Numeric level representing the log level.
 * @param msg The message to write to standard output.
 * @param pUserData User defined data that was passed with the hook to
 * @ref BaseLog::AddLogHook.
 */
static void LogToStandardOutput(GenericLogComponent_t comp,
                                BaseLog::Level_t level, const String& msg,
                                void* pUserData) {
#ifdef _WIN32
  static const WORD gLogColors[BaseLog::LOG_LEVEL_COUNT] = {
      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |
          FOREGROUND_INTENSITY,
      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
      FOREGROUND_RED | FOREGROUND_INTENSITY,
      FOREGROUND_RED | FOREGROUND_INTENSITY,
  };
#else
  static const String gLogColors[BaseLog::LOG_LEVEL_COUNT] = {
      "\e[1;32;40m",  // Debug
      "\e[37;40m",    // Info
      "\e[1;33;40m",  // Warning
      "\e[1;31;40m",  // Error
      "\e[1;37;41m",  // Critical
  };
#endif  // _WIN32

  (void)comp;
  (void)pUserData;

  if (0 > level || BaseLog::LOG_LEVEL_COUNT <= level) {
    level = BaseLog::LOG_LEVEL_CRITICAL;
  }

  std::list<String> msgs = msg.Split("\n");
  String last = msgs.back();
  msgs.pop_back();

  for (String m : msgs) {
#if _WIN32
    (void)SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                  gLogColors[level]);

    std::cout << m.ToUtf8();

    (void)SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    std::cout << std::endl;
#else
    if (isatty(fileno(stdout))) {
      std::cout << gLogColors[level] << m.ToUtf8() << "\e[0K\e[0m"
                << std::endl;
    } else {
      std::cout << m.ToUtf8() << std::endl;
    }
#endif  // _WIN32
  }

  if (!last.IsEmpty()) {
#if _WIN32
    (void)SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                  gLogColors[level]);

    std::cout << last.ToUtf8();

    (void)SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    if (isatty(fileno(stdout))) {
      std::cout << gLogColors[level] << last.ToUtf8() << "\e[0K\e[0m";
    } else {
      std::cout << last.ToUtf8();
    }
#endif  // _WIN32
  }

  std::cout.flush();
}

LogMessage::~LogMessage() {}

bool LogMessage::ShouldStop() const { return false; }

LogMessageFixed::LogMessageFixed(GenericLogComponent_t comp,
                                 BaseLog::Level_t level, const String& msg)
    : LogMessage(comp, level), mMessage(msg) {}

LogMessageFixed::~LogMessageFixed() {}

String LogMessageFixed::GetMsg() const { return mMessage; }

BaseLog::BaseLog()
    : mLogFileTimestampEnabled(false),
      mLogRotationEnabled(false),
      mLogCompression(true),
      mLogRotationCount(3),
      mLogRotationDays(1),
      mLogFile(nullptr),
      mLastLog(-1337),
      mConsoleAttributes(0) {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                                 &consoleInfo)) {
    mConsoleAttributes = consoleInfo.wAttributes;
  } else {
    mConsoleAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
  }

  (void)SetConsoleTextAttribute(
      GetStdHandle(STD_OUTPUT_HANDLE),
      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif  // _WIN32

  if (!gLogInst) {
    gLogInst = this;
  }

  // General messages (such as "Server ready!") are logged at info level by
  // default; every other component defaults to warning.
  mComponentLogLevels[to_underlying(BaseLogComponent_t::General)] =
      LOG_LEVEL_INFO;

  mThread = std::thread([this]() { MessageLoop(); });
}

BaseLog::~BaseLog() {
  // Stop the log thread after it writes the remaining messages.
  mMessages.Enqueue(new LogMessageStop);

  if (mThread.joinable()) {
    mThread.join();
  }

#ifdef _WIN32
  (void)SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                mConsoleAttributes);
#else
  std::cout << "\e[0K\e[0m";
#endif  // _WIN32

  delete mLogFile;
  mLogFile = nullptr;

  if (gLogInst == this) {
    gLogInst = nullptr;
  }
}

BaseLog* BaseLog::GetBaseSingletonPtr() { return gLogInst; }

void BaseLog::LogMessage(libcomp::LogMessage* pMessage) {
  if (pMessage) {
    mMessages.Enqueue(pMessage);
  }
}

void BaseLog::LogMessage(
    const std::chrono::system_clock::time_point& timestamp,
    GenericLogComponent_t comp, Level_t level, const String& msg) {
  static const String gLogMessages[BaseLog::LOG_LEVEL_COUNT] = {
      "DEBUG: %1%2", "%1%2", "WARNING: %1%2", "ERROR: %1%2", "CRITICAL: %1%2",
  };

  if (0 > level || LOG_LEVEL_COUNT <= level || !ShouldLog(comp, level)) {
    return;
  }

  String compString;

  if (LOG_SERVER_SPECIFIC_START_ID <= comp) {
    compString = LogComponentToString(comp);
  } else {
    compString = BaseLogComponentToString(comp);
  }

  if (!compString.IsEmpty()) {
    compString += ": ";
  }

  String final = String(gLogMessages[level]).Arg(compString).Arg(msg);

  if (nullptr != mLogFile) {
    auto duration =
        std::chrono::duration_cast<
            std::chrono::duration<int64_t, std::ratio<86400>>>(
            timestamp.time_since_epoch())
            .count();

    if (mLogRotationEnabled && mLogRotationDays <= (duration - mLastLog)) {
      RotateLogs();
    }

    mLastLog = duration;
  }

  if (nullptr != mLogFile) {
    if (mLogFileTimestampEnabled) {
      auto currentTime = std::chrono::system_clock::to_time_t(timestamp);

      std::stringstream ss;
      ss << std::put_time(std::localtime(&currentTime), "%Y/%m/%d %T");

      String formattedTime = String("[%1] ").Arg(ss.str());

      mLogFile->write(
          formattedTime.C(),
          (std::streamsize)(formattedTime.Size() * sizeof(char)));
    }

    mLogFile->write(final.C(), (std::streamsize)(final.Size() * sizeof(char)));
    mLogFile->flush();
  }

  for (auto i : mHooks) {
    (*i.first)(comp, level, final, i.second);
  }

  for (auto func : mLambdaHooks) {
    func(comp, level, final);
  }
}

void BaseLog::MessageLoop() {
  bool running = true;

  while (running) {
    std::list<libcomp::LogMessage*> msgs;
    mMessages.DequeueAll(msgs);

    for (auto pMessage : msgs) {
      if (pMessage->ShouldStop()) {
        running = false;
      } else if (running) {
        LogMessage(pMessage->GetTimestamp(), pMessage->GetComponent(),
                   pMessage->GetLevel(), pMessage->GetMsg());
      }

      delete pMessage;
    }
  }
}

String BaseLog::GetLogPath() const { return mLogPath; }

void BaseLog::SetLogPath(const String& path, bool truncate) {
  bool loaded = true;

  mLogPath = path;

  if (nullptr != mLogFile) {
    delete mLogFile;
    mLogFile = nullptr;
  }

  if (!mLogPath.IsEmpty()) {
    int mode = std::ofstream::out;

    if (truncate) {
      mode |= std::ofstream::trunc;
    } else {
      mode |= std::ofstream::app;
    }

    mLogFile = new std::ofstream();
    mLogFile->open(mLogPath.C(), (std::ios_base::openmode)mode);
    mLogFile->flush();

    if (!mLogFile->good()) {
      delete mLogFile;
      mLogFile = nullptr;
      mLogPath.Clear();
      loaded = false;
    }
  }

  if (!loaded) {
    LogGeneralCriticalMsg("Failed to open the log file for writing.\n");
    LogGeneralCriticalMsg("The application will now close.\n");
    LogGeneralInfoMsg("Bye!\n");

    exit(EXIT_FAILURE);
  }
}

void BaseLog::AddLogHook(Hook_t func, void* data) { mHooks[func] = data; }

void BaseLog::AddLogHook(
    const std::function<void(GenericLogComponent_t comp, Level_t level,
                             const String& msg)>& func) {
  mLambdaHooks.push_back(func);
}

void BaseLog::AddStandardOutputHook() { AddLogHook(&LogToStandardOutput); }

void BaseLog::ClearHooks() {
  mHooks.clear();
  mLambdaHooks.clear();
}

BaseLog::Level_t BaseLog::GetLogLevel(GenericLogComponent_t comp) const {
  auto it = mComponentLogLevels.find(comp);

  if (mComponentLogLevels.end() != it) {
    return it->second;
  }

  return LOG_LEVEL_WARNING;
}

void BaseLog::SetLogLevel(GenericLogComponent_t comp, Level_t level) {
  mComponentLogLevels[comp] = level;
}

bool BaseLog::GetLogFileTimestampsEnabled() const {
  return mLogFileTimestampEnabled;
}

void BaseLog::SetLogFileTimestampsEnabled(bool enabled) {
  mLogFileTimestampEnabled = enabled;
}

bool BaseLog::GetLogRotationEnabled() const { return mLogRotationEnabled; }

void BaseLog::SetLogRotationEnabled(bool enabled) {
  mLogRotationEnabled = enabled;
}

bool BaseLog::GetLogCompression() const { return mLogCompression; }

void BaseLog::SetLogCompression(bool enabled) { mLogCompression = enabled; }

int BaseLog::GetLogRotationCount() const { return mLogRotationCount; }

void BaseLog::SetLogRotationCount(int count) { mLogRotationCount = count; }

int BaseLog::GetLogRotationDays() const { return mLogRotationDays; }

void BaseLog::SetLogRotationDays(int days) { mLogRotationDays = days; }

bool BaseLog::ShouldLog(GenericLogComponent_t comp, Level_t level) const {
  return level >= GetLogLevel(comp);
}

GenericLogComponent_t BaseLog::StringToLogComponent(const String& comp) const {
  return to_underlying(StringToBaseLogComponent(comp));
}

String BaseLog::LogComponentToString(GenericLogComponent_t comp) const {
  return BaseLogComponentToString(comp);
}

void BaseLog::RotateLogs() {
  if (!mLogPath.IsEmpty() && !FileExists(mLogPath)) {
    return;
  }

  FileDelete(libcomp::String("%1.%2").Arg(mLogPath).Arg(mLogRotationCount));
  FileDelete(
      libcomp::String("%1.%2.gz").Arg(mLogPath).Arg(mLogRotationCount));

  for (int i = mLogRotationCount - 1; i > 0; --i) {
    FileMove(libcomp::String("%1.%2").Arg(mLogPath).Arg(i),
             libcomp::String("%1.%2").Arg(mLogPath).Arg(i + 1));
    FileMove(libcomp::String("%1.%2.gz").Arg(mLogPath).Arg(i),
             libcomp::String("%1.%2.gz").Arg(mLogPath).Arg(i + 1));
  }

  if (mLogFile) {
    mLogFile->close();
    delete mLogFile;
  }

  FileMove(mLogPath, mLogPath + ".1");

  mLogFile = new std::ofstream();
  mLogFile->open(mLogPath.C(), std::ofstream::out | std::ofstream::trunc);
  mLogFile->flush();

  if (!mLogFile->good()) {
    delete mLogFile;
    mLogFile = nullptr;
    mLogPath.Clear();
  }

  if (mLogCompression) {
    std::thread th(
        [](libcomp::String compressPath) {
          bool error = false;
          char buffer[4096];
          libcomp::String compressedPath = compressPath + ".gz";

          FILE* in = fopen(compressPath.C(), "rb");
          gzFile gf = gzopen(compressedPath.C(), "wb");

          if (in && gf) {
            while (!feof(in)) {
              auto sz = fread(buffer, 1, sizeof(buffer), in);

              if (0 >= sz) {
                break;
              }

              if ((int)sz != gzwrite(gf, buffer, (unsigned)sz)) {
                error = true;
                break;
              }
            }
          }

          if (gf) {
            gzclose(gf);
          }

          if (in) {
            fclose(in);
          }

          if (error) {
            FileDelete(compressedPath);
          } else {
            FileDelete(compressPath);
          }
        },
        mLogPath + ".1");

    th.detach();
  }
}

bool BaseLog::FileMove(const libcomp::String& oldPath,
                       const libcomp::String& newPath) {
#ifdef _WIN32
  return MoveFileA(oldPath.Replace("/", "\\").C(),
                   newPath.Replace("/", "\\").C());
#else
  return 0 == rename(oldPath.C(), newPath.C());
#endif
}

bool BaseLog::FileExists(const libcomp::String& file) {
#ifdef _WIN32
  return PathFileExistsA(file.Replace("/", "\\").C());
#else
  return 0 == access(file.C(), F_OK);
#endif
}

bool BaseLog::FileDelete(const libcomp::String& file) {
#ifdef _WIN32
  return DeleteFileA(file.Replace("/", "\\").C());
#else
  return 0 == unlink(file.C());
#endif
}
