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

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include <iostream>
#include <numbers>
#include <cmath>

namespace engine::glfw
{

glm::mat4 Camera::GetViewTransform() const
{
  return glm::mat4
  {
    u.x, u.y, u.z, -glm::dot(u, m_position),
    v.x, v.y, v.z, -glm::dot(v, m_position),
    n.x, n.y, n.z, -glm::dot(n, m_position),
    0.f, 0.f, 0.f, 1.f,
  };
}


float CyclicClamp(float value, float minValue, float maxValue)
{
  if (value < minValue) {
    return maxValue;  // Round to min when value goes below min
  }
  else if (value > maxValue) {
    return minValue;  // Clamp to max when above max
  }
  return value;  // Otherwise, return the value itself
}

void Camera::OnFrame(Application& application, float deltaTime)
{
  m_pressed.w = glfwGetKey(application.GetWindow(), GLFW_KEY_W) == GLFW_PRESS;
  m_pressed.s = glfwGetKey(application.GetWindow(), GLFW_KEY_S) == GLFW_PRESS;
  m_pressed.a = glfwGetKey(application.GetWindow(), GLFW_KEY_A) == GLFW_PRESS;
  m_pressed.d = glfwGetKey(application.GetWindow(), GLFW_KEY_D) == GLFW_PRESS;
  
  glm::vec3 direction = glm::vec3(0.f);
  if (m_pressed.w) {
    direction += n;
  }

  if (m_pressed.s) {
    direction -= n;
  }

  if (m_pressed.a) {
    direction -= u;
  }

  if (m_pressed.d) {
    direction += u;
  }


  if (glm::length(direction) > 0.0f)
      direction = glm::normalize(direction);

  glm::vec3 velocity = m_speed * direction;
  m_position = m_position + velocity * deltaTime;

  m_pressed = {};
}

void Camera::OnKey(int key, int scancode, int action, int mods)
{}


constexpr float Radians(float degrees)
{
  return std::numbers::pi_v<float> / 180.f * degrees;
}

void Camera::OnMouse(double x, double y)
{
  std::call_once(m_initialized_mouse_position, [&mouse_pos = m_mouse_pos ,x, y]
  {
      mouse_pos = {x , y};
  });

  glm::vec2 mouse_pos_diff = {x - m_mouse_pos.x, m_mouse_pos.y - y};
  m_mouse_pos = {x, y};

  m_pitch = glm::clamp(m_pitch + mouse_pos_diff.y, -45.f, 45.f);
  m_yaw = CyclicClamp(m_yaw + mouse_pos_diff.x, 0.f, 360.f);

  float quat_pitch = m_pitch / 2.f;
  float quat_yaw = m_yaw / 2.f;

  glm::quat rotate_yaw = glm::quat(std::cos(Radians(quat_yaw)), 0.f, std::sin(Radians(quat_yaw)), 0.f);
  glm::quat rotate_yaw_conjugate(rotate_yaw.w, -rotate_yaw.x, -rotate_yaw.y, -rotate_yaw.z);
  glm::quat new_x_quat = rotate_yaw * glm::quat(0.f, 1.f, 0.f, 0.f) * rotate_yaw_conjugate;
  glm::vec3 new_x = glm::normalize(glm::vec3(new_x_quat.x, new_x_quat.y, new_x_quat.z));

  glm::quat rotate_pitch(std::cos(Radians(quat_pitch)), new_x.x * std::sin(Radians(quat_pitch)), new_x.y * std::sin(Radians(quat_pitch)), new_x.z * std::sin(Radians(quat_pitch)));
  glm::quat rotate_pitch_conjugate(rotate_pitch.w, -rotate_pitch.x, -rotate_pitch.y, -rotate_pitch.z);
  glm::vec3 up_y(0.f, 1.f, 0.f);
  glm::vec3 current_z = glm::cross(new_x, up_y);
  glm::quat new_z_quat = rotate_pitch * glm::quat(0.f, current_z.x, current_z.y, current_z.z) * rotate_pitch_conjugate;
  glm::vec3 new_z = glm::normalize(glm::vec3(new_z_quat.x, new_z_quat.y, new_z_quat.z));

  glm::vec3 new_y = glm::normalize(glm::cross(new_x, new_z));

  u = new_x;
  v = new_y;
  n = new_z;
}


}  // namespace engine::glfw
