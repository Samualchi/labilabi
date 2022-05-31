#pragma once

#include <list>
#include <string>
#include <unordered_map>

enum class WriteMode
{
  WriteMode_Full, // write full log
  WriteMode_No_Commands, // write full log without commands
  WriteMode_Commands // write only commands
};

class Editor
{
private:

  std::list<std::wstring> console_log_; // ful log
  std::list<std::wstring> no_command_log_; // log without commands
  std::list<std::wstring> command_list_; // command list

  void save_command_line(const std::wstring& filename, const WriteMode mode) const;

  bool command_handler(const std::wstring& command);

  void loop();

  Editor() { loop(); }

public:

	static void Init() { Editor(); }

  static std::unordered_map<std::string, std::string> commands_list();

  static void print_help();

};
