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
#ifdef _WIN32
#include <Windows.h>
#endif

#include <filesystem>
#include <stdexcept>
#include <string>


#ifdef _WIN32
struct WinAPIError : public std::runtime_error
{
  using std::runtime_error::runtime_error;
};

std::filesystem::path GetCurrentExecutableAbsolutePath() {
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

std::filesystem::path GetCurrentExecutableDirectory() {
  std::filesystem::path path = GetCurrentExecutableAbsolutePath();
  return path.parent_path();
}
