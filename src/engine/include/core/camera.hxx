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

#include <glm/matrix.hpp>

namespace engine::glfw
{

class Camera
{
public:
  glm::mat4 GetViewTransform();
  void Move(glm::vec3 direction);
private:
  glm::vec3 m_position = {0.f, 0.f , 0.f};
  glm::vec3 m_view = {0.f, 0.f, 1.f};
  float m_velocity = .5f;
};

}  // namespace engine::glfw
