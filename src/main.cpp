#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <map>
#include <vector>
#include <list>
#include <array>
#include <algorithm>
#include <filesystem>
#include <unistd.h>

#include "rc_tokenizer.h"
#include "rc_parser.h"
#include "rc_ast.h"
#include "rc_resolver.h"
#include "rc_constants.h"
#include "rc_generator.h"
#include "pe_resource_decoder.h"
#include "imageio.h"

static bool is_pe_file(const std::string& path)
{
  std::ifstream f(path, std::ios::binary);
  if (!f.is_open())
    return false;
  char magic[2] = {};
  f.read(magic, 2);
  return magic[0] == 'M' && magic[1] == 'Z';
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

static void print_usage(const char* program_name)
{
  std::cerr << "Usage: " << program_name << " [options] <file.rc|file.exe>" << std::endl;
  std::cerr << "  -o <dir>       Output directory (default: input file directory)" << std::endl;
  std::cerr << "  -r <name>      Resource subdirectory name (default: res)" << std::endl;
  std::cerr << "  -q <file.qrc>  Generate .qrc resource file" << std::endl;
  std::cerr << "  -h             Show this help" << std::endl;
}

int main(int argc, char** argv)
{
  std::string input_path;
  std::string output_path;
  std::string qrc_path;
  std::string res_dir_name = "res";

  int opt;
  while((opt = getopt(argc, argv, "o:r:q:h")) != -1)
  {
    switch(opt)
    {
      case 'o':
        output_path = std::filesystem::path(optarg).generic_string();
        break;
      case 'r':
        res_dir_name = std::filesystem::path(optarg).generic_string();
        break;
      case 'q':
        qrc_path = std::filesystem::path(optarg).generic_string();
        break;
      case 'h':
        print_usage(argv[0]);
        return 0;
      default:
        print_usage(argv[0]);
        return 1;
    }
  }

  if(optind >= argc)
  {
    print_usage(argv[0]);
    return 1;
  }
  input_path = std::filesystem::path(argv[optind]).generic_string();

  if(is_pe_file(input_path))
  {
    std::cout << "PE file detected: " << input_path << std::endl;

    std::vector<pe_decoder::decoded_resource> pe_resources;
    std::filesystem::path out_dir;
    if(!output_path.empty())
    {
      std::filesystem::path p = std::filesystem::path(output_path);
      if(std::filesystem::path(p).extension().empty())
        out_dir = p;
      else
        out_dir = p.parent_path();
    }
    if(out_dir.empty())
      out_dir = std::filesystem::path(input_path).parent_path();
    if(out_dir.empty())
      out_dir = ".";
    std::filesystem::path rc_stem = std::filesystem::path(input_path).stem();
    std::string rc_basename = rc_stem.string();
    std::filesystem::create_directories(out_dir);
    std::filesystem::path img_dir = out_dir / res_dir_name;
    std::filesystem::create_directories(img_dir);

    try
    {
      pe_resources = pe_decoder::decode_pe_resources(input_path, img_dir);
    }
    catch(const std::exception& e)
    {
      std::cerr << "Error reading PE resources: " << e.what() << std::endl;
      return 1;
    }

    std::cout << "Decoded " << pe_resources.size() << " resources from PE file:" << std::endl;

    rc::resolver res;
    rc::constant_registry::instance();

    rc::generator gen;

    rc::rc_file file;

    for(const auto& decoded : pe_resources)
    {
      std::cout << "  " << decoded.id << " " << decoded.type;
      if (!decoded.filename.empty())
        std::cout << " -> " << decoded.filename;
      std::cout << std::endl;

      // Save BMP images
      if (!decoded.image_data.empty())
      {
        imageio::save_image(decoded.filename, decoded.image_data, img_dir);

        // Add image resource entry to rc_file for .qrc generation
        rc::resource img_res;
        img_res.id = decoded.id;
        img_res.type = decoded.type;
        img_res.filename = (out_dir / res_dir_name / decoded.filename).generic_string();
        img_res.data = rc::empty_data{};
        file.resources.push_back(std::move(img_res));
        continue;
      }

      // Parse RC text
      if (!decoded.rc_text.empty())
      {
        std::cout << "--- Decoded RC text ---" << std::endl;
        std::cout << decoded.rc_text << std::endl;
        std::cout << "---" << std::endl;

        try
        {
          std::vector<rc::token> tokens = rc::tokenize(decoded.rc_text);
          rc::parser p(tokens);
          rc::rc_file chunk = p.parse();

          for(auto& r : chunk.resources)
            file.resources.push_back(std::move(r));
        }
        catch(const std::exception& e)
        {
          std::cerr << "Error parsing decoded resource " << decoded.id << ": " << e.what() << std::endl;
        }
      }
    }

    std::cout << "\nTotal parsed resources: " << file.resources.size() << std::endl;

    res.resolve_file(file);

    if(gen.generate_all(file, out_dir.generic_string(), res_dir_name))
      std::cout << "Generated all dialogs in: "
                << (out_dir / res_dir_name).generic_string() << std::endl;
    else
      std::cerr << "Error: failed to generate dialogs" << std::endl;

    std::filesystem::path qrc_path_fs;
    if(!qrc_path.empty())
      qrc_path_fs = qrc_path;
    else
      qrc_path_fs = out_dir / (rc_basename + ".qrc");

    std::vector<std::string> ui_files;
    std::filesystem::path sub_dir = out_dir / res_dir_name;
    if(std::filesystem::exists(sub_dir))
    {
      for(const auto& entry : std::filesystem::directory_iterator(sub_dir))
      {
        if(entry.path().extension() == ".ui")
          ui_files.push_back(entry.path().generic_string());
      }
      std::sort(ui_files.begin(), ui_files.end());
    }

    if(!ui_files.empty())
    {
      if(gen.generate_qrc(file, qrc_path_fs.generic_string(), ui_files))
        std::cout << "Generated: " << qrc_path_fs.generic_string() << std::endl;
      else
        std::cerr << "Error: failed to generate " << qrc_path_fs.generic_string() << std::endl;
    }

    return 0;
  }

  std::string content = read_file(input_path);
  if(content.empty())
    return 1;

  std::cout << "Read " << content.size() << " bytes from " << input_path << std::endl;

  std::vector<rc::token> tokens;
  try
  {
    tokens = rc::tokenize(content);
  }
  catch(const std::runtime_error& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
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
          {
            rc::category_t stmt_cat =
              rc::category_t::dialog_style | rc::category_t::window_style;
            if(s.keyword == "EXSTYLE")
              stmt_cat = rc::category_t::extended_style;
            std::cout << " -> 0x" << std::hex << s.value.resolved_value
                      << " (" << res.format_value(stmt_cat, s.value.resolved_value) << ")" << std::dec;
          }
          std::cout << std::endl;
        }
        for(const auto& c : dd.controls)
        {
          std::cout << "  CONTROL " << c.id << " " << c.class_name;

          rc::category_t cat = rc::constant_registry::resolve_category(c.class_name);

          if(c.style.resolved_value >= 0)
            std::cout << " style=0x" << std::hex << c.style.resolved_value
                      << " (" << res.format_value(cat, c.style.resolved_value) << ")" << std::dec;
          if(c.ext_style.resolved_value >= 0)
            std::cout << " ext=0x" << std::hex << c.ext_style.resolved_value
                      << " (" << res.format_value(cat, c.ext_style.resolved_value) << ")" << std::dec;
          std::cout << std::endl;
        }
      }
    }
  }

  if(!output_path.empty() || !qrc_path.empty())
  {
    rc::generator gen;

    std::filesystem::path out_dir;
    if(!output_path.empty())
    {
      std::filesystem::path p = std::filesystem::path(output_path);
      if(std::filesystem::path(p).extension().empty())
        out_dir = p;
      else
        out_dir = p.parent_path();
    }
    if(out_dir.empty())
      out_dir = std::filesystem::path(input_path).parent_path();
    if(out_dir.empty())
      out_dir = ".";
    std::filesystem::path rc_stem = std::filesystem::path(input_path).stem();
    std::string rc_basename = rc_stem.string();

    std::filesystem::create_directories(out_dir);

    if(gen.generate_all(file, out_dir.generic_string(), res_dir_name))
      std::cout << "Generated all dialogs in: "
                << (out_dir / res_dir_name).generic_string() << std::endl;

    std::filesystem::path qrc_path_fs;
    if(!qrc_path.empty())
      qrc_path_fs = qrc_path;
    else
      qrc_path_fs = out_dir / res_dir_name / (rc_basename + ".qrc");

    std::vector<std::string> ui_files;
    std::filesystem::path sub_dir = out_dir / res_dir_name;
    if(std::filesystem::exists(sub_dir))
    {
      for(const auto& entry : std::filesystem::directory_iterator(sub_dir))
      {
        if(entry.path().extension() == ".ui")
          ui_files.push_back(entry.path().generic_string());
      }
      std::sort(ui_files.begin(), ui_files.end());
    }

    if(!ui_files.empty())
    {
      if(gen.generate_qrc(file, qrc_path_fs.generic_string(), ui_files))
        std::cout << "Generated: " << qrc_path_fs.generic_string() << std::endl;
      else
        std::cerr << "Error: failed to generate " << qrc_path_fs.generic_string() << std::endl;
    }
  }

  return 0;
}
