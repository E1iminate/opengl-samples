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

#include "core/camera.hxx"

namespace engine::glfw
{

glm::mat4 Camera::GetViewTransform()
{
  return glm::mat4
  {
    1.f, 0.f, 0.f, -m_position.x,
    0.f, 1.f, 0.f, -m_position.y,
    0.f, 0.f, 1.f, -m_position.z,
    0.f, 0.f, 0.f, 1.f,
  };
}

void Camera::Move(glm::vec3 direction)
{
  m_position += glm::normalize(direction) * m_velocity;
}

}  // namespace engine::glfw
