#include "imageio.h"

#include <cstring>
#include <fstream>

struct [[gnu::packed]] bmp_file_header
{
  uint16_t bfType = 0x4D42;
  uint32_t bfSize = 0;
  uint16_t bfReserved1 = 0;
  uint16_t bfReserved2 = 0;
  uint32_t bfOffBits = 0;
};

static constexpr uint8_t png_magic[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

namespace
{

struct [[gnu::packed]] bitmap_info_header
{
  uint32_t header_size;
  int32_t width;
  int32_t height;
  uint16_t planes;
  uint16_t bit_count;
  uint32_t compression;
  uint32_t image_size;
  int32_t x_pels_per_meter;
  int32_t y_pels_per_meter;
  uint32_t clr_used;
  uint32_t clr_important;
};

struct dib_info
{
  uint32_t header_size = 0;
  int32_t width = 0;
  int32_t height = 0;
  uint16_t bit_count = 0;
  uint32_t compression = 0;
  uint32_t palette_entries = 0;
  size_t pixel_offset = 0;
  size_t row_stride = 0;
};

dib_info parse_dib(std::span<const uint8_t> data)
{
  dib_info di;
  if (data.size() < 40)
    return di;

  bitmap_info_header bih;
  std::memcpy(&bih, data.data(), 40);

  di.header_size = bih.header_size;
  di.width = bih.width;
  di.height = bih.height;
  di.bit_count = bih.bit_count;
  di.compression = bih.compression;

  if (di.header_size < 40 || di.header_size > data.size())
  {
    di.header_size = 0;
    return di;
  }

  if (di.compression != 0 || di.width <= 0 || di.height == 0)
  {
    di.width = 0;
    return di;
  }

  if (di.bit_count != 1 && di.bit_count != 4 && di.bit_count != 8 &&
      di.bit_count != 16 && di.bit_count != 24 && di.bit_count != 32)
  {
    di.width = 0;
    return di;
  }

  if (di.bit_count <= 8)
    di.palette_entries = (bih.clr_used > 0) ? bih.clr_used : (1u << di.bit_count);

  di.pixel_offset = di.header_size + di.palette_entries * 4;
  di.row_stride = ((static_cast<size_t>(di.width) * di.bit_count + 31) / 32) * 4;

  if (di.pixel_offset > data.size())
  {
    di.width = 0;
    return di;
  }

  return di;
}

std::vector<uint8_t> build_bmp(
  const dib_info& di,
  std::span<const uint8_t> data,
  int actual_h)
{
  size_t pixel_array_size = static_cast<size_t>(actual_h) * di.row_stride;
  size_t bmp_size = 14 + di.pixel_offset + pixel_array_size;

  std::vector<uint8_t> result(bmp_size);

  bmp_file_header bfh;
  bfh.bfOffBits = 14 + static_cast<uint32_t>(di.pixel_offset);
  bfh.bfSize = static_cast<uint32_t>(bmp_size);
  std::memcpy(result.data(), &bfh, 14);

  std::memcpy(result.data() + 14, data.data(), di.header_size);

  if (di.palette_entries > 0)
    std::memcpy(result.data() + 14 + di.header_size,
                data.data() + di.header_size,
                di.palette_entries * 4);

  std::memcpy(result.data() + 14 + di.pixel_offset,
              data.data() + di.pixel_offset,
              pixel_array_size);

  return result;
}

}

namespace imageio
{

bool is_png_data(std::span<const uint8_t> data)
{
  return data.size() >= 8 && std::memcmp(data.data(), png_magic, 8) == 0;
}

std::vector<uint8_t> dib_to_bmp(std::span<const uint8_t> data)
{
  if (is_png_data(data))
    return std::vector<uint8_t>(data.begin(), data.end());

  dib_info di = parse_dib(data);
  if (di.width <= 0)
    return {};

  return build_bmp(di, data, di.height);
}

std::vector<uint8_t> ico_to_bmp(std::span<const uint8_t> data)
{
  if (is_png_data(data))
    return std::vector<uint8_t>(data.begin(), data.end());

  dib_info di = parse_dib(data);
  if (di.width <= 0)
    return {};

  int actual_h = di.height;
  if (actual_h > 0)
    actual_h /= 2;
  if (actual_h <= 0)
    return {};

  size_t and_stride = ((static_cast<size_t>(di.width) + 31) / 32) * 4;
  size_t and_offset = di.pixel_offset + static_cast<size_t>(actual_h) * di.row_stride;

  std::vector<uint8_t> result = build_bmp(di, data, actual_h);

  std::memcpy(result.data() + 14 + 8, &actual_h, 4);

  size_t xor_row_base = 14 + di.pixel_offset;

  if (and_offset + static_cast<size_t>(actual_h) * and_stride <= data.size())
  {
    if (di.bit_count == 32)
    {
      for (int y = 0; y < actual_h; y++)
      {
        size_t and_row = and_offset + static_cast<size_t>(y) * and_stride;
        size_t xor_row = xor_row_base + static_cast<size_t>(y) * di.row_stride;
        for (int x = 0; x < di.width; x++)
        {
          size_t ab = and_row + x / 8;
          if (ab >= data.size())
            break;
          if ((data[ab] >> (7 - (x & 7))) & 1)
            result[xor_row + x * 4 + 3] = 0;
        }
      }
    }
    else if (di.bit_count <= 8)
    {
      for (int y = 0; y < actual_h; y++)
      {
        size_t and_row = and_offset + static_cast<size_t>(y) * and_stride;
        size_t xor_row = xor_row_base + static_cast<size_t>(y) * di.row_stride;
        for (int x = 0; x < di.width; x++)
        {
          size_t ab = and_row + x / 8;
          if (ab >= data.size())
            break;
          if ((data[ab] >> (7 - (x & 7))) & 1)
          {
            if (di.bit_count == 8)
              result[xor_row + x] = 0;
            else if (di.bit_count == 4)
            {
              size_t off = xor_row + x / 2;
              if (x & 1)
                result[off] &= 0xF0u;
              else
                result[off] &= 0x0Fu;
            }
            else if (di.bit_count == 1)
            {
              size_t off = xor_row + x / 8;
              result[off] &= static_cast<uint8_t>(~(1u << (7 - (x & 7))));
            }
          }
        }
      }
    }
  }

  return result;
}

bool save_image(
  const std::string& filename,
  const std::vector<uint8_t>& image_data,
  const std::filesystem::path& output_dir)
{
  if (image_data.empty())
    return false;
  std::filesystem::path out_path = output_dir / filename;
  std::ofstream f(out_path, std::ios::binary);
  if (!f.is_open())
    return false;
  f.write(reinterpret_cast<const char*>(image_data.data()),
    static_cast<std::streamsize>(image_data.size()));
  return f.good();
}

}
