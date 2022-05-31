#include "Editor.h"

#include <algorithm>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

void Editor::print_help()
{
  std::cout << "Command \t: Description\n";
  const auto cml = Editor::commands_list();
  for(const auto& command_pair : cml)
  {
    std::cout << "! " << command_pair.first << "\t: " << command_pair.second << "\n";
  }
}

std::unordered_map<std::string, std::string> Editor::commands_list()
{
  std::unordered_map<std::string, std::string> result;

  result.insert(std::pair<std::string, std::string>
    ("<help>", "Prints possible commands, e.g. <help>"));
  result.insert(std::pair<std::string, std::string>
    ("<font={FONT_NAME}>", "Sets console font to FONT_NAME, e.g. <font=Consolas>"));
  result.insert(std::pair<std::string, std::string>
    ("<color={COLOR_VALUE={0-255}}>", "Sets console text color to COLOR_VALUE, e.g. <color=4>"));
  result.insert(std::pair<std::string, std::string>
    ("<quit>", "Terminates the programm, e.g. <quit>"));
  result.insert(std::pair<std::string, std::string>
    ("<fwrite={FILENAME}>", "Writes the whole console text to the FILENAME, e.g. <fwrite=c:/Repos/fwrite.txt>"));
  result.insert(std::pair<std::string, std::string>
    ("<nwrite={FILENAME}>", std::string("Writes the whole console text instead of command blocks" +
      std::string("(<>, \\<> isn't a command block) to the FILENAME, e.g. <nwrite=c:/Repos/nwrite.txt>"))));
  result.insert(std::pair<std::string, std::string>
    ("<cwrite={FILENAME}>", "Writes every command block to the FILENAME, e.g. <fwrite=c:/Repos/fwrite.txt>"));

  return result;
}

void Editor::save_command_line(const std::wstring& filename, WriteMode mode) const
{
  std::wofstream file(filename);
  if(!file.is_open())
  {
    std::cerr << "CANNOT WRITE TO THE FILE\n";
    return;
  }
  if(mode == WriteMode::WriteMode_Full || mode == WriteMode::WriteMode_No_Commands)
  {
    if(mode == WriteMode::WriteMode_Full)
    {
      for(const auto& str : console_log_)
      {
        file << str;
        file << '\n';
      }
    }
    else
    {
      for (const auto& str : no_command_log_)
      {
        file << str;
        file << '\n';
      }
    }
  }
  else
  {
    for(const auto& it : command_list_)
    {
      file << it;
      file << '\n';
    }
  }
  file.close();
}

bool Editor::command_handler(const std::wstring& command)
{
  if(command.find(L"help") != std::wstring::npos)
  {
    print_help();
  }
  else if (command.find(L"font") != std::wstring::npos)
  {
    const std::wstring font = command.substr(command.find_first_of(L"font=") + 1, L'>');

    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 20;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, font.c_str());
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
  }
  else if (command.find(L"color") != std::wstring::npos)
  {
    int color = std::stoi(command.substr(command.find_first_of(L'=') + 1));
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
  }
  else if (command.find(L"exit") != std::wstring::npos)
  {
    return false;
  }
  else if (command.find(L"fwrite") != std::wstring::npos)
  {
    const std::wstring filename = command.substr(command.find_first_of(L'=') + 1);
    save_command_line(filename, WriteMode::WriteMode_Full);
  }
  else if (command.find(L"nwrite") != std::wstring::npos)
  {
    const std::wstring filename = command.substr(command.find_first_of(L'=') + 1);
    save_command_line(filename, WriteMode::WriteMode_No_Commands);
  }
  else if (command.find(L"cwrite") != std::wstring::npos)
  {
    const std::wstring filename = command.substr(command.find_first_of(L'=') + 1);
    save_command_line(filename, WriteMode::WriteMode_Commands);
  }
  else if (command.find(L"read") != std::wstring::npos)
  {
    const std::wstring filename = command.substr(command.find_first_of(L'=') + 1);
    std::wifstream wif(filename);
    wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
    std::wstringstream wss;
    wss << wif.rdbuf();
    std::wstring wstr = wss.str();
    std::wcout << wstr;
  }
  return true;
}

void Editor::loop()
{
  std::wstring input_string;
  while (std::getline(std::wcin, input_string))
  {
    std::list<std::wstring> commands_to_remove;
    bool ignore_command_character = false;
    for (std::size_t i = 0; i < input_string.length(); i++)
    {
      const wchar_t curr_char = input_string[i];
      if (curr_char == '<' && !ignore_command_character && i < input_string.length() - 1)
      {
        const std::size_t command_end = input_string.substr(i).find_first_of(L'>');
        if (command_end != std::wstring::npos)
        {
          std::wstring clean_command = input_string.substr(i + 1, command_end - 1);
          if (!command_handler(clean_command))
          {
            std::cout << "EXIT FROM THE LOOP\n";
            return;
          }
          command_list_.push_back(clean_command);
          commands_to_remove.push_back(L'<' + clean_command + L'>');
        }
        ignore_command_character = false;
      }
      else if (curr_char != L'<' && ignore_command_character)
      {
        ignore_command_character = false;
      }
      else if (curr_char == '\\')
      {
        ignore_command_character = true;
      }
    }
    console_log_.push_back(input_string);
    for(const auto& it : commands_to_remove)
    {
      const std::size_t lb = input_string.find_first_of(it);
      if(lb >= 1 && lb != std::wstring::npos)
      {
        if(it[lb] != L'\\')
        {
          input_string.erase(lb, input_string.find_last_of(it) - lb);
        }
      }
      else if(lb != std::wstring::npos)
      {
        input_string.erase(lb, input_string.find_last_of(it) - lb);
      }
      no_command_log_.push_back(input_string);
    }
  }
}
