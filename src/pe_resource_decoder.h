#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <filesystem>

namespace pe_decoder
{
  struct decoded_resource
  {
    std::string id;
    std::string type;
    std::string rc_text;
    std::vector<uint8_t> image_data;
    std::string filename;
  };

  std::vector<decoded_resource> decode_pe_resources(
    const std::string& pe_path,
    const std::filesystem::path& output_dir = "");

  std::string decode_dialog(
    std::span<const uint8_t> data,
    const std::string& resource_id);

  std::string decode_menu(
    std::span<const uint8_t> data,
    const std::string& resource_id);

  std::string decode_accelerators(
    std::span<const uint8_t> data,
    const std::string& resource_id);

  std::string decode_stringtable(
    std::span<const uint8_t> data,
    uint32_t group_id);

  std::string decode_versioninfo(
    std::span<const uint8_t> data,
    const std::string& resource_id);
}
