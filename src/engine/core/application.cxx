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

#include "core/application.hxx"

namespace engine {
namespace glfw {

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  Application* application = static_cast<Application*>(glfwGetWindowUserPointer(window));
  application->OnKey(key, scancode, action, mods);
}

Application::Application()
{
  if (!m_window.Get())
    throw WindowInitFail();

  glfwSetWindowUserPointer(m_window.Get(), this);
  glfwMakeContextCurrent(m_window.Get());
  glfwSetFramebufferSizeCallback(m_window.Get(), FramebufferSizeCallback);
  glfwSetKeyCallback(m_window.Get(), KeyCallback);

  if (gladLoadGL() == 0)
    throw LibraryInitFail("gladLoadGL failed!");
}

void Application::Run()
{
  while (!glfwWindowShouldClose(GetWindow())) {
    OnUpdate();
    OnRender();

    glfwSwapBuffers(GetWindow());
    glfwPollEvents();
  }
}

} // namespace glfw
} // namespace engine
