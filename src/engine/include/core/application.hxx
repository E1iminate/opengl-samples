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

// Include in this order to prevent GL header & Windows redefenition errors
#include "common.hxx"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
//

#include <functional>
#include <memory>

namespace engine {
namespace glfw {


class Application
{
  struct LibraryHandle
  {
    LibraryHandle()
    {
      if (glfwInit() != GLFW_TRUE)
        throw LibraryInitFail("glfwInit failed!");
    }
    ~LibraryHandle()
    {
      glfwTerminate();
    }
  };

  class Window
  {
  public:
    Window()
    {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

      handle = glfwCreateWindow(800, 600, "OpenGL Tutorial", NULL, NULL);
    }

    ~Window()
    {
      glfwDestroyWindow(handle);
    }

    GLFWwindow* Get() { return handle; }

  private:
    GLFWwindow* handle = nullptr;
  };

public:
  Application();

  virtual void OnUpdate() = 0;
  virtual void OnRender() = 0;

  void Run();

protected:
  GLFWwindow* GetWindow() { return m_window.Get(); }

private:
  LibraryHandle m_handle;
  Window m_window;
};

} // namespace glfw
} // namespace engine