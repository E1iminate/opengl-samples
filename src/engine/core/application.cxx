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

Application::Application(Callback callback)
  : m_callback(callback)
{}

void Application::Run()
{
  while (!glfwWindowShouldClose(GetWindow())) {
    m_callback(*this);

    glfwSwapBuffers(GetWindow());
    glfwPollEvents();
  }
}

} // namespace glfw
} // namespace engine
