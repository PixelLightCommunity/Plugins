///
/// @file WebConfig.h
///
/// @brief The header for the WebConfig class.
///
/// @author
///
/// This file is a part of Awesomium, a Web UI bridge for native apps.
///
/// Website: <http://www.awesomium.com>
///
/// Copyright (C) 2012 Khrona. All rights reserved. Awesomium is a
/// trademark of Khrona.
///
#ifndef AWESOMIUM_WEB_CONFIG_H_
#define AWESOMIUM_WEB_CONFIG_H_

#include <Awesomium/Platform.h>
#include <Awesomium/WebString.h>

namespace Awesomium {

///
/// The level of verbosity for the log. See WebConfig::log_level
///
enum LogLevel {
  kLogLevel_None = 0,  ///< No log is created
  kLogLevel_Normal,    ///< Logs only errors
  kLogLevel_Verbose,   ///< Logs everything
};

///
/// @brief Use this class to specify configuration settings for the WebCore.
///
/// @see WebCore::Initialize
///
class OSM_EXPORT WebConfig {
 public:
  ///
  /// Creates the default WebConfig.
  ///
  WebConfig();

  ///
  /// The level of verbosity for the logger.
  ///
  LogLevel log_level;

  ///
  /// The directory path that contains the Awesomium package. Will be used to
  /// resolve all other paths. The working directory is used by default.
  ///
  WebString package_path;

  ///
  /// The directory path to search for additional plugins. This is useful for
  /// bundling plugins with your application.
  ///
  WebString plugin_path;

  ///
  /// The file path to the log file (should include filename, for example:
  /// `C:\\my_path\\out.log`). This is `./awesomium.log` by default.
  ///
  WebString log_path;

  ///
  /// The file path to the child process. This is `./awesomium_process.exe`
  /// by default.
  ///
  WebString child_process_path;

  ///
  /// The user-agent string to use instead of the default. You should only
  /// specify this if you want to spoof the user-agent.
  ///
  WebString user_agent;

  ///
  /// The port that the Awesomium Inspector's HTTP server should listen on.
  ///
  /// To enable the inspector, set this value to any non-zero port (ex, 1337)
  /// and then go to http://127.0.0.1:1337 (or whatever port you specified)
  /// in some other browser.
  ///
  /// Please note you must have `inspector.pak` in your working directory
  /// for the Inspector to load correctly.
  ///
  /// Specify 0 to disable the inspector (the default).
  ///
  int remote_debugging_port;
};

}  // namespace Awesomium

#endif  // AWESOMIUM_WEB_CONFIG_H_
