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

#include "core/user-input-handler.hxx"
#include "core/frame-updatable.hxx"

#include <glm/glm.hpp>

#include <mutex>

namespace engine::glfw
{

class Camera : public IUserInputHandler,
               public IFrameUpdatable
{
public:
  glm::mat4 GetViewTransform() const;
  void OnFrame(Application& application, float deltaTime) override;
  void OnKey(int key, int scancode, int action, int mods) override;
  void OnMouse(double x, double y) override;
private:
  struct KeysPressed
  {
    bool w = false;
    bool s = false;
    bool a = false;
    bool d = false;
  } m_pressed;
  glm::vec3 m_position = {0.f, 0.f, 0.f};
  glm::vec3 m_view = {0.f, 0.f, 1.f};
  float m_speed = 5.f;
  std::once_flag m_initialized_mouse_position;
  glm::vec2 m_mouse_pos = {0.f, 0.f};

  glm::vec3 u = {1.f, 0.f, 0.f};
  glm::vec3 v = { 0.f, 1.f, 0.f };
  glm::vec3 n = { 0.f, 0.f, 1.f };

  glm::vec3 target = {0.f, 0.f, 1.f};

  float m_pitch = 0.f;
  float m_yaw = 0.f;
};

}  // namespace engine::glfw
