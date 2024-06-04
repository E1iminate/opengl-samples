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

#include "core/window.hxx"
#include "core/exceptions.hxx"

namespace engine {
namespace glfw {

Window::Window()
{
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_holder = glfwCreateWindow(800, 600, "OpenGL Tutorial", NULL, NULL);

  glfwMakeContextCurrent(Get());
}

Window::~Window()
{}

GLFWwindow* Window::Get()
{
  return m_holder.Get();
}

Window::Holder::Holder(GLFWwindow* window) : m_window(window)
{
  if (!m_window)
    throw WindowInitFail{};
}

Window::Holder::~Holder()
{
  glfwDestroyWindow(m_window);
}

GLFWwindow* Window::Holder::Get()
{
  return m_window;
}

} // namespace glfw
} // namespace engine
