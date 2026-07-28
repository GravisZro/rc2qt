// Corrected MENUEX debug tool per Microsoft documentation.
#include "pefile.hpp"
#include "pe_constants.hpp"
#include "pe_containers.hpp"
#include <cstdint>
#include <cstdio>
#include <string>
#include <span>

static std::string read_unicode(const uint8_t* d, size_t& pos, size_t max)
{
  std::string result;
  while (pos + 1 < max)
  {
    uint16_t ch = d[pos] | (static_cast<uint16_t>(d[pos + 1]) << 8);
    pos += 2;
    if (ch == 0)
      break;
    result += static_cast<char>(ch < 128 ? ch : '?');
  }
  return result;
}

static size_t align_to(size_t pos, size_t alignment)
{
  return (pos + alignment - 1) & ~(alignment - 1);
}

// Returns the next unprocessed position (past this item AND its children)
static size_t walk_menu_items(
    const uint8_t* d, size_t len, size_t start, int depth,
    bool& out_found_last)
{
  size_t pos = start;
  std::string indent(depth * 2, ' ');
  out_found_last = false;

  while (pos < len)
  {
    if (pos + 14 > len)
      break;

    // Print raw bytes for debugging first few items
    if (depth <= 1 && pos < 0xC0)
    {
      printf("%s  [RAW %04zx: %02x %02x %02x %02x %02x %02x %02x %02x "
             "%02x %02x %02x %02x %02x %02x]\n",
             indent.c_str(), pos,
             d[pos], d[pos+1], d[pos+2], d[pos+3],
             d[pos+4], d[pos+5], d[pos+6], d[pos+7],
             d[pos+8], d[pos+9], d[pos+10], d[pos+11],
             d[pos+12], d[pos+13]);
    }

    // Read 14-byte fixed portion
    uint32_t dw_type  = d[pos] | (d[pos+1]<<8) | (d[pos+2]<<16) | (d[pos+3]<<24);
    uint32_t dw_state = d[pos+4] | (d[pos+5]<<8) | (d[pos+6]<<16) | (d[pos+7]<<24);
    uint32_t u_id     = d[pos+8] | (d[pos+9]<<8) | (d[pos+10]<<16) | (d[pos+11]<<24);
    uint16_t w_flags  = d[pos+12] | (d[pos+13]<<8);
    size_t text_start = pos + 14;

    size_t tp = text_start;
    std::string text = read_unicode(d, tp, len);
    size_t text_end = tp; // past null terminator

    // Check terminator (all-zero fields + empty text)
    if (dw_type == 0 && dw_state == 0 && u_id == 0 && w_flags == 0 && text.empty())
    {
      printf("%s[TERMINATOR at 0x%zx]\n", indent.c_str(), pos);
      size_t item_end = align_to(text_end, 4);
      out_found_last = true;
      return item_end;
    }

    bool is_popup = (w_flags & 0x01) != 0;
    bool is_last  = (w_flags & 0x80) != 0;

    // For popup: read dwHelpId at first DWORD boundary after text
    uint32_t dw_help_id = 0;
    size_t item_end;
    if (is_popup)
    {
      size_t dh_pos = align_to(text_end, 4);
      if (dh_pos + 4 <= len)
      {
        dw_help_id = d[dh_pos] | (d[dh_pos+1]<<8) | (d[dh_pos+2]<<16) | (d[dh_pos+3]<<24);
        item_end = dh_pos + 4;
      }
      else
      {
        item_end = text_end;
      }
    }
    else
    {
      item_end = text_end;
    }
    item_end = align_to(item_end, 4);

    bool is_separator = (!is_popup) && (dw_type & 0x0800) != 0;

    if (is_separator)
    {
      printf("%sMENUITEM SEPARATOR  (0x%zx-0x%zx)\n",
             indent.c_str(), pos, item_end);
    }
    else if (is_popup)
    {
      printf("%sPOPUP \"%s\"  id=%u flags=0x%04x state=0x%x dh=%u  (0x%zx-0x%zx)\n",
             indent.c_str(), text.c_str(), u_id, w_flags, dw_state, dw_help_id,
             pos, item_end);
      // Children start at item_end (past dwHelpId)
      bool child_found_last = false;
      size_t after_children = walk_menu_items(d, len, item_end, depth + 1, child_found_last);
      // Continue from after children
      if (is_last)
      {
        out_found_last = true;
        return after_children;
      }
      pos = after_children;
      continue;
    }
    else
    {
      printf("%sMENUITEM \"%s\", %u  flags=0x%04x state=0x%x  (0x%zx-0x%zx)\n",
             indent.c_str(), text.c_str(), u_id, w_flags, dw_state,
             pos, item_end);
    }

    if (is_last)
    {
      printf("%s[LAST ITEM flag set]\n", indent.c_str());
      out_found_last = true;
      return item_end;
    }

    pos = item_end;
  }

  return pos;
}

int main()
{
  pefile::PE pe("test/PEview.exe", false);

  const auto& resources = pe.resources();

  for (const auto& root_dir : resources)
  {
    for (const auto& type_entry : root_dir.entries)
    {
      if (type_entry.id != 4)
        continue;

      std::span<const uint8_t> raw;
      if (type_entry.directory)
      {
        for (const auto& lang : type_entry.directory->entries)
        {
          if (lang.data_entry)
          {
            raw = pe.get_data(lang.data_entry->data_rva, lang.data_entry->size);
            break;
          }
          else if (lang.directory)
          {
            for (const auto& sub : lang.directory->entries)
            {
              if (sub.data_entry)
              {
                raw = pe.get_data(sub.data_entry->data_rva, sub.data_entry->size);
                break;
              }
            }
            if (!raw.empty())
              break;
          }
        }
      }

      if (raw.empty())
        continue;

      printf("Menu resource: %zu bytes\n\n", raw.size());

      const uint8_t* d = raw.data();
      size_t len = raw.size();

      if (len < 4)
      {
        printf("Too small for header\n");
        continue;
      }

      uint16_t version  = d[0] | (d[1] << 8);
      uint16_t offset_val = d[2] | (d[3] << 8);

      printf("Header: version=%u, wOffset=%u\n", version, offset_val);

      if (version == 1)
      {
        if (len < 8)
        {
          printf("Too small for MENUEX header\n");
          continue;
        }
        uint32_t menu_help_id = d[4] | (d[5]<<8) | (d[6]<<16) | (d[7]<<24);
        printf("Menu dwHelpId = %u\n", menu_help_id);

        size_t first_item = 4 + offset_val;
        printf("First item at offset 0x%zx (4 + %u)\n", first_item, offset_val);

        bool found_last = false;
        walk_menu_items(d, len, first_item, 0, found_last);
      }
    }
  }

  return 0;
}
