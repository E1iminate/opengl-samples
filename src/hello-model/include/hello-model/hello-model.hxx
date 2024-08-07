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

class HelloModel : public engine::glfw::Application
{
public:
  HelloModel();
  ~HelloModel();

  void OnUpdate() final;
  void OnRender() final;

private:
  void LoadAssets();

  std::unique_ptr<tinyobj::Model> m_model;
  std::vector<GLuint> m_vertex_indexes;

  std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();
  float m_angle = 0.f;
  float m_speed = 1.f;
  float m_scalingFactor = 0.1f;
  GLuint m_program = 0u;
  GLuint m_ebo = -1;
  GLuint m_vbo = -1;
  GLuint m_vao = -1;
};
