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

#ifdef HAVE_QT
// needed for font database
#include <QApplication>
#include "rc_render.h"
#endif

#include "rc_tokenizer.h"
#include "rc_parser.h"
#include "rc_types.h"
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

#ifdef HAVE_QT
static int run_verify_layouts(rc::generator& gen, const std::string& render_dir)
{
  std::vector<rc::render::verify_input> inputs = gen.take_verify_inputs();
  if(inputs.empty())
  {
    std::cerr << "Verify: no dialogs collected" << std::endl;
    return 1;
  }

  std::cout << "Verify: " << inputs.size() << " dialogs offscreen" << std::endl;
  int total_targets = 0;
  int total_rendered = 0;
  int total_missing = 0;
  int total_overlaps = 0;
  double iou_sum = 0.0;
  double dx_sum = 0.0;
  double dy_sum = 0.0;
  double dw_sum = 0.0;
  double dh_sum = 0.0;
  int mean_count = 0;
  double max_dx = 0.0;
  double max_dy = 0.0;

  for(const auto& input : inputs)
  {
    rc::render::result r = rc::render::verify_layout(input, render_dir);
    std::cout << "  " << input.name << ": targets=" << r.target_widgets
              << " rendered=" << r.rendered_widgets
              << " missing=" << r.missing_widgets
              << " iou=" << r.mean_iou
              << " dx=" << r.mean_dx << " dy=" << r.mean_dy
              << " dw=" << r.mean_dw << " dh=" << r.mean_dh
              << " max_dx=" << r.max_dx << " max_dy=" << r.max_dy
              << " overlaps=" << r.overlap_violations << std::endl;
    total_targets += r.target_widgets;
    total_rendered += r.rendered_widgets;
    total_missing += r.missing_widgets;
    total_overlaps += r.overlap_violations;
    if(r.rendered_widgets > 0)
    {
      iou_sum += r.mean_iou;
      dx_sum += r.mean_dx;
      dy_sum += r.mean_dy;
      dw_sum += r.mean_dw;
      dh_sum += r.mean_dh;
      ++mean_count;
    }
    max_dx = std::max(max_dx, r.max_dx);
    max_dy = std::max(max_dy, r.max_dy);
  }

  if(mean_count > 0)
  {
    std::cout << "Verify: total targets=" << total_targets
              << " rendered=" << total_rendered
              << " missing=" << total_missing
              << " overlaps=" << total_overlaps
              << " mean_iou=" << iou_sum / mean_count
              << " mean_dx=" << dx_sum / mean_count
              << " mean_dy=" << dy_sum / mean_count
              << " mean_dw=" << dw_sum / mean_count
              << " mean_dh=" << dh_sum / mean_count
              << " max_dx=" << max_dx << " max_dy=" << max_dy << std::endl;
  }
  return 0;
}
#endif

static void print_usage(const char* program_name)
{
  std::cerr << "Usage: " << program_name << " [options] <file.rc|file.exe>" << std::endl;
  std::cerr << "  -o <dir>       Output directory (default: current directory)" << std::endl;
  std::cerr << "  -r <name>      Resource subdirectory name (default: res)" << std::endl;
  std::cerr << "  -q <file.qrc>  .qrc output filename (default: <input basename>.qrc)" << std::endl;
  std::cerr << "  -m <file.txt>  Widget metrics file from getuimetrics (default: uimetrics.txt)" << std::endl;
  std::cerr << "  -n             Disable all geometric adjustments" << std::endl;
  std::cerr << "  -f             Prevent font substitutions (keep original font names)" << std::endl;
  std::cerr << "  -l             Emit Qt layout managers instead of absolute geometry (experimental)" << std::endl;
#ifdef HAVE_QT
  std::cerr << "  -v             Verify layout-mode output offscreen (requires -l)" << std::endl;
  std::cerr << "  -d <dir>       Render dump directory for -v (default: none)" << std::endl;
#endif
  std::cerr << "  -h             Show this help" << std::endl;
}

int main(int argc, char** argv)
{
#ifdef HAVE_QT
  /* Verify mode renders dialogs offscreen; force the Qt offscreen platform
     unless the caller already selected one. This must happen before the
     QApplication is constructed. */
  for(int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if(arg.size() >= 2 && arg[0] == '-' && arg[1] != '-' &&
       arg.find('v') != std::string::npos)
    {
      qputenv("QT_QPA_PLATFORM", "offscreen");
      /* The generator converts dialog units to pixels at 96 DPI; pin the
         offscreen render to the same DPI so rendered geometry matches. */
      qputenv("QT_FONT_DPI", "96");
      break;
    }
  }
  QApplication app(argc, argv);
#endif

  std::string input_path;
  std::string output_path;
  std::string qrc_path;
  std::string metrics_path;
  std::string res_dir_name = "res";
  bool disable_geometry_adjustments = false;
  bool prevent_font_substitution = false;
  bool use_layouts = false;
#ifdef HAVE_QT
  bool verify_layouts = false;
  std::string render_dir;
#endif

  int opt;
#ifdef HAVE_QT
  while((opt = getopt(argc, argv, "o:r:q:m:hnflvd:")) != -1)
#else
  while((opt = getopt(argc, argv, "o:r:q:m:hnfl")) != -1)
#endif
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
      case 'm':
        metrics_path = std::filesystem::path(optarg).generic_string();
        break;
      case 'n':
        disable_geometry_adjustments = true;
        break;
      case 'f':
        prevent_font_substitution = true;
        break;
      case 'l':
        use_layouts = true;
        break;
      case 'v':
#ifdef HAVE_QT
        verify_layouts = true;
#endif
        break;
      case 'd':
#ifdef HAVE_QT
        render_dir = std::filesystem::path(optarg).generic_string();
#endif
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

#ifdef HAVE_QT
  if(verify_layouts)
    use_layouts = true;
#endif

  if(metrics_path.empty() && std::filesystem::exists("uimetrics.txt"))
    metrics_path = "uimetrics.txt";

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
    // if(out_dir.empty())
    //   out_dir = std::filesystem::path(input_path).parent_path();
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
      gen.set_disable_geometry_adjustments(disable_geometry_adjustments);
      gen.set_prevent_font_substitution(prevent_font_substitution);
      gen.set_use_layouts(use_layouts);
#ifdef HAVE_QT
      gen.set_collect_verify(verify_layouts);
#endif

    if(!metrics_path.empty())
    {
      if(!gen.load_uimetrics(metrics_path))
        std::cerr << "Warning: cannot load metrics file: " << metrics_path << std::endl;
    }

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

        // Add image resource entry to rc_file for .qrc generation.
        // The filename is stored relative to the .qrc output directory
        // (out_dir); generate_qrc emits resource filenames verbatim.
        rc::resource img_res;
        img_res.id = decoded.id;
        img_res.type = decoded.type;
        img_res.filename = (std::filesystem::path(res_dir_name) / decoded.filename).generic_string();
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

#ifdef HAVE_QT
    if(verify_layouts)
      return run_verify_layouts(gen, render_dir);
#endif

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
  }
  else
  {
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
        std::cout << " entries=" << std::get<std::vector<rc::accelerator_entry>>(res.data).size();
      else if(std::holds_alternative<std::vector<rc::string_table_entry>>(res.data))
        std::cout << " entries=" << std::get<std::vector<rc::string_table_entry>>(res.data).size();
      else if(std::holds_alternative<rc::menu_data>(res.data))
        std::cout << " entries=" << std::get<rc::menu_data>(res.data).entries.size();

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


    {
    rc::generator gen;
    gen.set_disable_geometry_adjustments(disable_geometry_adjustments);
    gen.set_prevent_font_substitution(prevent_font_substitution);
    gen.set_use_layouts(use_layouts);
#ifdef HAVE_QT
    gen.set_collect_verify(verify_layouts);
#endif
      if(!metrics_path.empty())
      {
        if(!gen.load_uimetrics(metrics_path))
          std::cerr << "Warning: cannot load metrics file: " << metrics_path << std::endl;
      }
      std::filesystem::path out_dir = ".";
      if(!output_path.empty())
      {
        std::filesystem::path p = std::filesystem::path(output_path);
        if(std::filesystem::path(p).extension().empty())
          out_dir = p;
        else
          out_dir = p.parent_path();
      }

      std::filesystem::path rc_stem = std::filesystem::path(input_path).stem();
      std::string rc_basename = rc_stem.string();

      std::filesystem::create_directories(out_dir);

      if(gen.generate_all(file, out_dir.generic_string(), res_dir_name))
        std::cout << "Generated all dialogs in: "
                  << (out_dir / res_dir_name).generic_string() << std::endl;

#ifdef HAVE_QT
      if(verify_layouts)
        return run_verify_layouts(gen, render_dir);
#endif

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
    }
  }

  return 0;
}
