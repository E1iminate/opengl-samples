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
#include "core/camera.hxx"
#include "core/user-input-handler.hxx"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>

class HelloLighting : public engine::glfw::Application
{
public:
  HelloLighting();
  ~HelloLighting();

  void OnUpdate() final;
  void OnRender() final;


private:
  void LoadAssets();

  engine::glfw::Camera m_camera;

  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
  };

  struct Mesh {
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
  };

  std::vector<GLuint> m_vaos;
  std::vector<GLuint> m_vbos;
  std::vector<GLuint> m_ebos;

  std::vector<Mesh> m_meshes;

  const aiScene* m_scene = nullptr;
  std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();
  float m_angle = 0.f;
  float m_speed = 1.f;
  float m_fov = 90.f;
  float m_cube_scale = 1.f;
  float m_near_z = 0.1f;
  float m_far_z = 5000.f;

  float m_translation_x = 0.f;
  float m_translation_y = 0.f;
  float m_translation_z = 0.f;
  float m_camera_velocity = .5f;
  GLuint m_program = 0u;
  GLboolean m_is_skybox = 0;
};
