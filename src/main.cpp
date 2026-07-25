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
#include <filesystem>

#include "rc_tokenizer.h"
#include "rc_parser.h"
#include "rc_ast.h"
#include "rc_resolver.h"
#include "rc_constants.h"
#include "rc_generator.h"



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
  std::string input_path;
  std::string output_path;
  std::string qrc_path;

  for(int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if(arg == "-o" && i + 1 < argc)
    {
      output_path = std::filesystem::path(argv[++i]).generic_string();
    }
    else if(arg == "-q" && i + 1 < argc)
    {
      qrc_path = std::filesystem::path(argv[++i]).generic_string();
    }
    else if(arg == "-h" || arg == "--help")
    {
      std::cerr << "Usage: " << argv[0] << " [options] <file.rc>" << std::endl;
      std::cerr << "  -o <file.ui>   Generate .ui file" << std::endl;
      std::cerr << "  -q <file.qrc>  Generate .qrc resource file" << std::endl;
      std::cerr << "  -h, --help     Show this help" << std::endl;
      return 0;
    }
    else if(input_path.empty())
    {
      input_path = std::filesystem::path(arg).generic_string();
    }
  }

  if(input_path.empty())
  {
    std::cerr << "Usage: " << argv[0] << " [options] <file.rc>" << std::endl;
    std::cerr << "  -o <file.ui>   Generate .ui file" << std::endl;
    std::cerr << "  -q <file.qrc>  Generate .qrc resource file" << std::endl;
    std::cerr << "  -h, --help     Show this help" << std::endl;
    return 1;
  }

  std::string content = read_file(input_path);
  if(content.empty())
    return 1;

  std::cout << "Read " << content.size() << " bytes from " << input_path << std::endl;

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

  rc::resolver res;
  res.resolve_file(file);
  std::cout << "\nConstant resolver: " << rc::constant_registry::instance().size() << " constants loaded" << std::endl;

  for(const auto& r : file.resources)
  {
    if(std::holds_alternative<rc::dialog_data>(r.data))
    {
      const auto& dd = std::get<rc::dialog_data>(r.data);
      if(!dd.statements.empty())
      {
        std::cout << "\nDialog " << r.id << ":" << std::endl;
        for(const auto& s : dd.statements)
        {
          std::cout << "  " << s.keyword << " " << s.value.first;
          for(const auto& [op, name] : s.value.ops)
            std::cout << " " << op << " " << name;
          if(s.value.resolved_value >= 0)
            std::cout << " -> 0x" << std::hex << s.value.resolved_value
                      << " (" << res.format_value(s.value.resolved_value) << ")" << std::dec;
          std::cout << std::endl;
        }
        for(const auto& c : dd.controls)
        {
          std::cout << "  CONTROL " << c.id << " " << c.class_name;
          if(c.style.resolved_value >= 0)
            std::cout << " style=0x" << std::hex << c.style.resolved_value
                      << " (" << res.format_value(c.style.resolved_value) << ")" << std::dec;
          if(c.ext_style.resolved_value >= 0)
            std::cout << " ext=0x" << std::hex << c.ext_style.resolved_value
                      << " (" << res.format_value(c.ext_style.resolved_value) << ")" << std::dec;
          std::cout << std::endl;
        }
      }
    }
  }

  if(!output_path.empty() || !qrc_path.empty())
  {
    rc::generator gen;

    if(!output_path.empty())
    {
      if(gen.generate(file, output_path))
        std::cout << "Generated: " << output_path << std::endl;
      else
        std::cerr << "Error: failed to generate " << output_path << std::endl;
    }

    if(!qrc_path.empty())
    {
      if(gen.generate_qrc(file, qrc_path, output_path))
        std::cout << "Generated: " << qrc_path << std::endl;
      else
        std::cerr << "Error: failed to generate " << qrc_path << std::endl;
    }
  }

  return 0;
}
