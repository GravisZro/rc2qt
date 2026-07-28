#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <filesystem>

namespace pe_decoder
{
  bool is_png_data(std::span<const uint8_t> data);
  std::vector<uint8_t> dib_to_bmp(std::span<const uint8_t> data);
  std::vector<uint8_t> ico_to_bmp(std::span<const uint8_t> data);
  bool save_image(
    const std::string& filename,
    const std::vector<uint8_t>& image_data,
    const std::filesystem::path& output_dir);
}
