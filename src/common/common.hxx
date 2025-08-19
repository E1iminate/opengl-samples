/*************************************************************************
* Copyright 2024 Vladislav Riabov
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include <tiny_obj_loader.h>
#include <stb_image.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
struct WinAPIError : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

inline std::filesystem::path GetCurrentExecutableAbsolutePath() {
  // https://learn.microsoft.com/en-ie/windows/win32/fileio/maximum-file-path-limitation?tabs=registry
  // The maximum path of 32, 767 characters is approximate, because the "\\?\" prefix may be expanded to a longer string by the system at run time,
  // and this expansion applies to the total length.

  constexpr size_t maxPath = 32767;
  std::basic_string <TCHAR> buffer(maxPath, 0);
  DWORD result = GetModuleFileName(NULL, buffer.data(), buffer.size());
  if (result == 0)
    throw WinAPIError("GetModuleFileName failed");

  return buffer;
}
#endif

inline std::filesystem::path GetCurrentExecutableDirectory() {
  std::filesystem::path path = GetCurrentExecutableAbsolutePath();
  return path.parent_path();
}

struct FailedToLoadObject : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

namespace tinyobj
{
class Model
{
public:
  Model(const std::filesystem::path& model_path, const std::filesystem::path& materials_path = {})
  {
    std::string errors;
    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &errors, model_path.string().c_str(), materials_path.string().c_str());
    if (!result || !errors.empty())
      throw FailedToLoadObject(errors);
  }

  const tinyobj::attrib_t& Attrib() const { return attrib; }
  const std::vector<tinyobj::shape_t>& Shapes() const { return shapes; }
  const std::vector<tinyobj::material_t>& Materials() const { return materials; }

private:
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
};

} // namespace tinyobjloader


namespace stb
{
class Image
{
public:
  Image(const std::filesystem::path& path)
  {
    if (!std::filesystem::exists(path)) {
      throw FailedToLoadObject(std::string("File does not exist: ") + path.string());
    }

    if (!std::filesystem::is_regular_file(path)) {
      throw FailedToLoadObject(std::string("Not a regular file: ") + path.string());
    }

    bytes = stbi_load(path.string().c_str(), &width, &height, &channelsNum, 0);
    if (!bytes)
      throw FailedToLoadObject(std::string("Failed to load ") + path.string());
  }

  ~Image()
  {
    stbi_image_free(bytes);
  }

  stbi_uc* Bytes() const { return bytes; }
  int Width() const { return width; }
  int Height() const { return height; }
  int ChannelsNum() const { return channelsNum; }

private:
  stbi_uc* bytes = nullptr;
  int width = 0;
  int height = 0;
  int channelsNum = 0;
};

} // namespace stb

template<class ProcessingFunction>
static void ProcessImage(const std::filesystem::path& path, ProcessingFunction processing_function)
{
  stb::Image image(path);
  processing_function(image);
}
