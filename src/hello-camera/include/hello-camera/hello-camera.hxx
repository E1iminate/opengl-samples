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

#include "core/application.hxx"

#include <memory>

class HelloCamera : public engine::glfw::Application
{
public:
  HelloCamera();
  ~HelloCamera();

  void OnUpdate() final;
  void OnRender() final;

private:
  void LoadAssets();

  struct Vec2 {
    tinyobj::real_t x = 0.f;
    tinyobj::real_t y = 0.f;
  };

  struct Vec3 {
    tinyobj::real_t x = 0.f;
    tinyobj::real_t y = 0.f;
    tinyobj::real_t z = 0.f;
  };

  std::unique_ptr<tinyobj::Model> m_model;
  std::vector<Vec3> m_vertices;
  std::vector<Vec2> m_texcoords;

  std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();
  float m_angle = 0.f;
  float m_speed = 1.f;
  float m_fov = 90.f;
  float m_cube_scale = 0.5f;
  float m_near_z = 0.1f;
  float m_far_z = 100.f;

  float m_translation_x = 0.f;
  float m_translation_y = 0.f;
  float m_translation_z = 2.f;
  GLuint m_program = 0u;
  GLuint m_ebo = -1;
  GLuint m_vbo = -1;
  GLuint m_vao = -1;
};
