#include "pefile.hpp"
#include "pe_constants.hpp"
#include "pe_containers.hpp"
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

static std::string read_unicode(const uint8_t* data, size_t& pos, size_t max)
{
  std::string result;
  while (pos + 1 < max)
  {
    uint16_t ch = data[pos] | (static_cast<uint16_t>(data[pos + 1]) << 8);
    pos += 2;
    if (ch == 0)
      break;
    result += static_cast<char>(ch < 128 ? ch : '?');
  }
  return result;
}

int main()
{
  pefile::PE pe("test/PEview.exe", false);

  const auto& resources = pe.resources();
  printf("Found %zu resource directories\n", resources.size());

  for (const auto& root_dir : resources)
  {
    printf("Directory with %zu entries\n", root_dir.entries.size());
    for (const auto& type_entry : root_dir.entries)
    {
      printf("  type_entry.id = %u\n", type_entry.id);
      if (type_entry.id != 4)
        continue;

      std::span<const uint8_t> raw;
      if (type_entry.data_entry)
      {
        raw = pe.get_data(type_entry.data_entry->data_rva, type_entry.data_entry->size);
      }
      else if (type_entry.directory)
      {
        for (const auto& lang : type_entry.directory->entries)
        {
          if (lang.data_entry)
          {
            raw = pe.get_data(lang.data_entry->data_rva, lang.data_entry->size);
            break;
          }
        }
      }

      if (raw.empty())
        continue;

      printf("Menu resource: %zu bytes\n\n", raw.size());

      const uint8_t* d = raw.data();
      size_t len = raw.size();
      size_t pos = 0;

      // Read header
      uint16_t version = d[0] | (d[1] << 8);
      uint16_t offset_val = d[2] | (d[3] << 8);
      printf("Header: version=%u, offset=%u\n", version, offset_val);
      pos = 4;

      if (version == 1 && len >= 8)
      {
        // Try interpretation A: header is 4 bytes, no dwHelpId in header
        printf("\n=== Interpretation A: header = 4 bytes, first item at offset 4 ===\n");
        size_t p = 4;

        // Read first item assuming NO dwHelpId
        printf("At offset %zu:\n", p);
        uint32_t dwType = d[p] | (d[p+1] << 8) | (d[p+2] << 16) | (d[p+3] << 24);
        printf("  dwType   = 0x%08X\n", dwType);
        uint32_t dwState = d[p+4] | (d[p+5] << 8) | (d[p+6] << 16) | (d[p+7] << 24);
        printf("  dwState  = 0x%08X\n", dwState);
        uint32_t uId = d[p+8] | (d[p+9] << 8) | (d[p+10] << 16) | (d[p+11] << 24);
        printf("  uId      = %u (0x%X)\n", uId, uId);
        uint16_t wFlags = d[p+12] | (d[p+13] << 8);
        printf("  wFlags   = 0x%04X\n", wFlags);
        size_t textPos = p + 14;
        std::string text = read_unicode(d, textPos, len);
        printf("  text     = \"%s\"\n", text.c_str());
        printf("  text ends at offset %zu\n", textPos);

        // Try interpretation B: header is 8 bytes, first item at offset 8 with dwHelpId for popup
        printf("\n=== Interpretation B: header = 8 bytes, first item at offset 8 with dwHelpId ===\n");
        p = 8;
        printf("At offset %zu:\n", p);
        uint32_t dh = d[p] | (d[p+1] << 8) | (d[p+2] << 16) | (d[p+3] << 24);
        printf("  dwHelpId = 0x%08X\n", dh);
        dwType = d[p+4] | (d[p+5] << 8) | (d[p+6] << 16) | (d[p+7] << 24);
        printf("  dwType   = 0x%08X\n", dwType);
        dwState = d[p+8] | (d[p+9] << 8) | (d[p+10] << 16) | (d[p+11] << 24);
        printf("  dwState  = 0x%08X\n", dwState);
        uId = d[p+12] | (d[p+13] << 8) | (d[p+14] << 16) | (d[p+15] << 24);
        printf("  uId      = %u (0x%X)\n", uId, uId);
        wFlags = d[p+16] | (d[p+17] << 8);
        printf("  wFlags   = 0x%04X\n", wFlags);
        textPos = p + 18;
        text = read_unicode(d, textPos, len);
        printf("  text     = \"%s\"\n", text.c_str());
        printf("  text ends at offset %zu\n", textPos);

        // Try interpretation C: header is 4 bytes, popup has dwHelpId BEFORE standard fields
        printf("\n=== Interpretation C: header = 4 bytes, popup = dwHelpId+dwType+dwState+uId+wFlags+text ===\n");
        p = 4;
        printf("At offset %zu:\n", p);
        dh = d[p] | (d[p+1] << 8) | (d[p+2] << 16) | (d[p+3] << 24);
        printf("  dwHelpId = 0x%08X\n", dh);
        dwType = d[p+4] | (d[p+5] << 8) | (d[p+6] << 16) | (d[p+7] << 24);
        printf("  dwType   = 0x%08X\n", dwType);
        dwState = d[p+8] | (d[p+9] << 8) | (d[p+10] << 16) | (d[p+11] << 24);
        printf("  dwState  = 0x%08X\n", dwState);
        uId = d[p+12] | (d[p+13] << 8) | (d[p+14] << 16) | (d[p+15] << 24);
        printf("  uId      = %u (0x%X)\n", uId, uId);
        wFlags = d[p+16] | (d[p+17] << 8);
        printf("  wFlags   = 0x%04X\n", wFlags);
        textPos = p + 18;
        text = read_unicode(d, textPos, len);
        printf("  text     = \"%s\"\n", text.c_str());
        printf("  text ends at offset %zu\n", textPos);

        // Now try to read the item that should be "&Open...\tCtrl+O" with id=36
        printf("\n--- Looking for first child (should be Open, id=36) ---\n");

        // From interp A: first child after padding
        printf("\nIf interp A (no dwHelpId, 14-byte fixed):\n");
        p = 14 + text.size() * 2 + 2; // skip first item
        printf("  Raw position after first item: %zu\n", p);
        // Check for 4-byte alignment
        size_t aligned = (p + 3) & ~3;
        printf("  Aligned to 4 bytes: %zu\n", aligned);
        if (aligned < len - 14)
        {
          dwType = d[aligned] | (d[aligned+1] << 8) | (d[aligned+2] << 16) | (d[aligned+3] << 24);
          dwState = d[aligned+4] | (d[aligned+5] << 8) | (d[aligned+6] << 16) | (d[aligned+7] << 24);
          uId = d[aligned+8] | (d[aligned+9] << 8) | (d[aligned+10] << 16) | (d[aligned+11] << 24);
          wFlags = d[aligned+12] | (d[aligned+13] << 8);
          printf("  dwType=%08X dwState=%08X uId=%u wFlags=%04X\n", dwType, dwState, uId, wFlags);
        }

        printf("\nIf interp C (dwHelpId, 18-byte fixed):\n");
        p = 18 + text.size() * 2 + 2; // skip first item
        printf("  Raw position after first item: %zu\n", p);
        aligned = (p + 3) & ~3;
        printf("  Aligned to 4 bytes: %zu\n", aligned);
        if (aligned < len - 14)
        {
          dwType = d[aligned] | (d[aligned+1] << 8) | (d[aligned+2] << 16) | (d[aligned+3] << 24);
          dwState = d[aligned+4] | (d[aligned+5] << 8) | (d[aligned+6] << 16) | (d[aligned+7] << 24);
          uId = d[aligned+8] | (d[aligned+9] << 8) | (d[aligned+10] << 16) | (d[aligned+11] << 24);
          wFlags = d[aligned+12] | (d[aligned+13] << 8);
          printf("  dwType=%08X dwState=%08X uId=%u wFlags=%04X\n", dwType, dwState, uId, wFlags);
          if (uId == 36)
          {
            printf("  *** FOUND OPEN ITEM at offset %zu ***\n", aligned);
            textPos = aligned + 14;
            text = read_unicode(d, textPos, len);
            printf("  text = \"%s\"\n", text.c_str());
          }
        }

        // Also try: interpret C with 8-byte alignment
        printf("\nIf interp C (dwHelpId, 18-byte fixed, 8-byte align):\n");
        p = 18 + text.size() * 2 + 2;
        aligned = (p + 7) & ~7;
        printf("  Aligned to 8 bytes: %zu\n", aligned);
        if (aligned < len - 14)
        {
          dwType = d[aligned] | (d[aligned+1] << 8) | (d[aligned+2] << 16) | (d[aligned+3] << 24);
          dwState = d[aligned+4] | (d[aligned+5] << 8) | (d[aligned+6] << 16) | (d[aligned+7] << 24);
          uId = d[aligned+8] | (d[aligned+9] << 8) | (d[aligned+10] << 16) | (d[aligned+11] << 24);
          wFlags = d[aligned+12] | (d[aligned+13] << 8);
          printf("  dwType=%08X dwState=%08X uId=%u wFlags=%04X\n", dwType, dwState, uId, wFlags);
          if (uId == 36)
          {
            printf("  *** FOUND OPEN ITEM at offset %zu ***\n", aligned);
            textPos = aligned + 14;
            text = read_unicode(d, textPos, len);
            printf("  text = \"%s\"\n", text.c_str());
          }
        }
      }
    }
  }

  return 0;
}
