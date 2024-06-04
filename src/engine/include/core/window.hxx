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

#include "core/exceptions.hxx"
#include <GLFW/glfw3.h>

namespace engine {
namespace glfw {
class LibraryHandle
{
public:
  LibraryHandle()
  {
    if (glfwInit() != GLFW_TRUE)
      throw LibraryInitFail{};
  }
  ~LibraryHandle()
  {
    glfwTerminate();
  }
};

class Window
{
public:
  class Holder
  {
  public:
    Holder() = default;
    Holder(GLFWwindow* window);
    ~Holder();
    GLFWwindow* Get();
  private:
    GLFWwindow* m_window = nullptr;
  };

  Window();
  ~Window();
  GLFWwindow* Get();


private:
  LibraryHandle m_library_handle;
  Holder m_holder;
};

} // namespace glfw
} // namespace engine