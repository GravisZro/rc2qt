#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <type_traits>
#include <map>
#include <vector>
#include <list>
#include <array>
#include <algorithm>

#include "shortjson.h"
#include "rcdef.h"
#include "rc_tokenizer.h"
#include "rc_parser.h"
#include "rc_ast.h"

using namespace shortjson;

std::string substitute(std::string str, std::string one, std::string two)
{
  for(size_t loc = std::string::npos; (loc = str.find("$1")) != std::string::npos;)
    str = str.replace(loc, 2, one);

  for(size_t loc = std::string::npos; (loc = str.find("$2")) != std::string::npos;)
    str = str.replace(loc, 2, two);

  return str;
}

static std::string read_file(const std::string& path)
{
  std::ifstream file(path);
  if(!file.is_open())
  {
    std::cerr << "Error: cannot open file: " << path << std::endl;
    return "";
  }
  std::ostringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " <file.rc>" << std::endl;
    return 1;
  }

  std::string content = read_file(argv[1]);
  if(content.empty())
    return 1;

  std::cout << "Read " << content.size() << " bytes from " << argv[1] << std::endl;

  auto tokens = rc::tokenize(content);
  std::cout << "Tokenized into " << tokens.size() << " tokens" << std::endl;

  rc::parser p(tokens);
  rc::rc_file file = p.parse();

  std::cout << "Parsed " << file.resources.size() << " resources:" << std::endl;

  for(const auto& res : file.resources)
  {
    std::cout << "  " << res.id << " " << res.type;
    if(!res.attributes.empty())
    {
      std::cout << " [";
      for(size_t i = 0; i < res.attributes.size(); ++i)
      {
        if(i > 0) std::cout << " ";
        std::cout << res.attributes[i];
      }
      std::cout << "]";
    }
    if(!res.filename.empty())
      std::cout << " \"" << res.filename << "\"";

    if(std::holds_alternative<rc::dialog_data>(res.data))
    {
      const auto& dd = std::get<rc::dialog_data>(res.data);
      std::cout << " (" << dd.width << "x" << dd.height << ")";
      std::cout << " statements=" << dd.statements.size();
      std::cout << " controls=" << dd.controls.size();
    }
    else if(std::holds_alternative<rc::toolbar_data>(res.data))
    {
      const auto& td = std::get<rc::toolbar_data>(res.data);
      std::cout << " (" << td.width << "x" << td.height << ")";
      std::cout << " entries=" << td.entries.size();
    }
    else if(std::holds_alternative<std::vector<rc::accelerator_entry>>(res.data))
    {
      std::cout << " entries=" << std::get<std::vector<rc::accelerator_entry>>(res.data).size();
    }
    else if(std::holds_alternative<std::vector<rc::string_table_entry>>(res.data))
    {
      std::cout << " entries=" << std::get<std::vector<rc::string_table_entry>>(res.data).size();
    }
    else if(std::holds_alternative<rc::menu_data>(res.data))
    {
      std::cout << " entries=" << std::get<rc::menu_data>(res.data).entries.size();
    }

    std::cout << std::endl;
  }

  return 0;
}
