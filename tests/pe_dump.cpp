#include "pefile.hpp"
#include "pe_constants.hpp"
#include "pe_containers.hpp"

#include <cstdint>
#include <cstdio>
#include <string>

using pefile::uint32_t;
using pefile::uint8_t;

static const char* resource_type_name(uint32_t type_id)
{
  switch (type_id)
  {
    case 1:  return "CURSOR";
    case 2:  return "BITMAP";
    case 3:  return "ICON";
    case 4:  return "MENU";
    case 5:  return "DIALOG";
    case 6:  return "STRING";
    case 7:  return "FONTDIR";
    case 8:  return "FONT";
    case 9:  return "ACCELERATOR";
    case 10: return "RCDATA";
    case 11: return "MESSAGETABLE";
    case 12: return "GROUP_CURSOR";
    case 14: return "GROUP_ICON";
    case 16: return "VERSION";
    case 17: return "DLGINCLUDE";
    case 24: return "MANIFEST";
    default: return "UNKNOWN";
  }
}

static bool should_hexdump(uint32_t type_id)
{
  switch (type_id)
  {
    case 4:
    case 5:
    case 6:
    case 9:
    case 10:
    case 16:
    case 17:
      return true;
    default:
      return false;
  }
}

static void hex_dump(const uint8_t* data, size_t len)
{
  const size_t max_bytes = 2048;
  size_t count = len < max_bytes ? len : max_bytes;

  for (size_t i = 0; i < count; i += 16)
  {
    printf("    %04zx: ", i);
    for (size_t j = 0; j < 16 && (i + j) < count; j++)
    {
      printf("%02x ", data[i + j]);
    }
    printf("\n");
  }
  if (count < len)
  {
    printf("    ... (%zu more bytes)\n", len - count);
  }
}

static void dump_resource_entry(
    const pefile::PE& pe,
    uint32_t type_id,
    const pefile::ResourceDirEntryData& id_entry,
    const pefile::ResourceDataEntryData& data_entry)
{
  std::string id_str;
  if (!id_entry.name.empty())
  {
    id_str = "\"" + id_entry.name + "\"";
  }
  else
  {
    id_str = std::to_string(id_entry.id);
  }

  printf("  Type: %-16s (%u)  ID: %-20s  Lang: %u  Sublang: %u  Size: %u bytes\n",
         resource_type_name(type_id), type_id,
         id_str.c_str(),
         data_entry.lang,
         data_entry.sublang,
         data_entry.size);

  if (should_hexdump(type_id) && data_entry.size > 0)
  {
    auto data = pe.get_data(data_entry.data_rva, data_entry.size);
    if (!data.empty())
    {
      hex_dump(data.data(), data.size());
    }
    else
    {
      printf("    (could not read resource data at RVA 0x%x)\n", data_entry.data_rva);
    }
  }
}

static void walk_id_entries(
    const pefile::PE& pe,
    uint32_t type_id,
    const pefile::ResourceDirData& id_dir)
{
  for (const auto& id_entry : id_dir.entries)
  {
    if (id_entry.data_entry)
    {
      dump_resource_entry(pe, type_id, id_entry, *id_entry.data_entry);
    }
    else if (id_entry.directory)
    {
      for (const auto& lang_entry : id_entry.directory->entries)
      {
        if (lang_entry.data_entry)
        {
          dump_resource_entry(pe, type_id, id_entry, *lang_entry.data_entry);
        }
      }
    }
  }
}

int main(int argc, char* argv[])
{
  const char* pe_path = "test/PEview.exe";
  if (argc > 1)
  {
    pe_path = argv[1];
  }

  printf("Opening PE file: %s\n\n", pe_path);

  pefile::PE pe(pe_path, false);

  const auto& warnings = pe.get_warnings();
  for (const auto& w : warnings)
  {
    printf("  WARNING: %s\n", w.c_str());
  }

  const auto& resources = pe.resources();
  printf("Found %zu top-level resource types\n\n", resources.size());

  for (const auto& type_dir : resources)
  {
    for (const auto& type_entry : type_dir.entries)
    {
      uint32_t type_id = type_entry.id;

      if (type_entry.directory)
      {
        walk_id_entries(pe, type_id, *type_entry.directory);
      }
      else if (type_entry.data_entry)
      {
        pefile::ResourceDirEntryData synthetic;
        synthetic.id = 0;
        dump_resource_entry(pe, type_id, synthetic, *type_entry.data_entry);
      }
    }
  }

  return 0;
}
