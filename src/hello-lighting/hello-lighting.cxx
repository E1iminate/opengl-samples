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

#include "hello-lighting/hello-lighting.hxx"
#include "core/exceptions.hxx"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <cmath>
#include <format>
#include <iostream>
#include <numbers>

HelloLighting::HelloLighting()
 : Application(m_camera)
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.IniFilename = nullptr;

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();

  LoadAssets();

  glEnable(GL_DEPTH_TEST);
}

void HelloLighting::LoadAssets()
{
  std::filesystem::path texture_name = "wine_barrel_01_4k.gltf";
  std::filesystem::path modelFolder = GetCurrentExecutableDirectory() / "assets" / "models" / texture_name;
  std::filesystem::path modelPath = modelFolder / texture_name;

  Assimp::Importer importer;

  m_scene = importer.ReadFile(modelPath.string(),
    aiProcess_Triangulate
  );

  if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode) {
    throw engine::RuntimeError(std::format("ERROR::ASSIMP:: %s", importer.GetErrorString()));
  }

  std::cout << "Model loaded successfully!" << std::endl;
  std::cout << "Number of meshes: " << m_scene->mNumMeshes << std::endl;

  m_meshes.resize(m_scene->mNumMeshes);

  for (size_t mesh_index = 0; mesh_index < m_meshes.size(); mesh_index++) {
    aiMesh* mesh = m_scene->mMeshes[mesh_index];
    m_meshes[mesh_index].m_vertices.resize(mesh->mNumVertices);

    for (size_t vertex_index = 0; vertex_index < mesh->mNumVertices; vertex_index++) {
      m_meshes[mesh_index].m_vertices[vertex_index].position = glm::vec3(mesh->mVertices[vertex_index].x,
                                                                         mesh->mVertices[vertex_index].y,
                                                                         mesh->mVertices[vertex_index].z);

      if (mesh->mNormals) {
          m_meshes[mesh_index].m_vertices[vertex_index].normal = glm::vec3(mesh->mNormals[vertex_index].x,
                                                                           mesh->mNormals[vertex_index].y,
                                                                           mesh->mNormals[vertex_index].z);
      }

      if (mesh->mTextureCoords[0]) {
          m_meshes[mesh_index].m_vertices[vertex_index].texcoord = glm::vec2(mesh->mTextureCoords[0][vertex_index].x,
            1.0f - mesh->mTextureCoords[0][vertex_index].y);
      }
    }

    for (size_t face_index = 0; face_index < mesh->mNumFaces; face_index++) {
      aiFace& face = mesh->mFaces[face_index];
      for (size_t index = 0; index < face.mNumIndices; index++) {
        m_meshes[mesh_index].m_indices.push_back(face.mIndices[index]);
      }
    }
  }

  aiMaterial* mat = m_scene->mMaterials[0];

  auto bindTexture = [](GLint slot, std::filesystem::path path) {
    GLuint texture = -1;
    glGenTextures(1, &texture);
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, texture);
    ProcessImage(path, [](stb::Image& image)
      {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.Width(), image.Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.Bytes());
      });
    glGenerateMipmap(GL_TEXTURE_2D);
    };

  aiString path;
  if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
    std::cout << "Diffuse texture path: " << path.C_Str() << std::endl;
    bindTexture(GL_TEXTURE0, modelFolder / path.C_Str());
  }
  if (mat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
    std::cout << "Normal map path: " << path.C_Str() << std::endl;
    bindTexture(GL_TEXTURE1, modelFolder / path.C_Str());
  }

  m_vaos.resize(m_meshes.size());
  m_vbos.resize(m_meshes.size());
  m_ebos.resize(m_meshes.size());

  glGenVertexArrays(m_vaos.size(), m_vaos.data());
  glGenBuffers(m_vbos.size(), m_vbos.data());
  glGenBuffers(m_ebos.size(), m_ebos.data());

  for (size_t i = 0u; i < m_vaos.size(); i++) {
    glBindVertexArray(m_vaos[i]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebos[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_meshes[i].m_indices.size() * sizeof(m_meshes[i].m_indices[0]), m_meshes[i].m_indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]);
    glBufferData(GL_ARRAY_BUFFER, m_meshes[i].m_vertices.size() * sizeof(m_meshes[i].m_vertices[0]), m_meshes[i].m_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texcoord)));
    glEnableVertexAttribArray(2);
  }

  glBindVertexArray(0);

  const GLchar* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 vsPosition;
layout (location = 1) in vec3 vsNormal;
layout (location = 2) in vec2 vsTexCoord;
uniform mat4 rotation_z;
uniform mat4 rotation_y;
uniform mat4 projection;
uniform float scale;
uniform mat4 translation;
uniform mat4 camera;
out vec2 psTexCoord;

void main()
{
    gl_Position = projection * camera * translation * rotation_y * vec4(scale * vsPosition, 1.0);
    psTexCoord = vsTexCoord;
}
)";

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  const GLchar* fragmentShaderSource = R"(
#version 330 core
in vec2 psTexCoord;
out vec4 FragColor;
uniform sampler2D diffuseTex;
uniform sampler2D normalsTex;
void main()
{
  FragColor = texture(diffuseTex, psTexCoord);
}
)";

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  m_program = glCreateProgram();

  glAttachShader(m_program, vertexShader);
  glAttachShader(m_program, fragmentShader);

  glLinkProgram(m_program);
}

HelloLighting::~HelloLighting()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

constexpr float Radians(float degrees) {
  return std::numbers::pi_v<float> / 180.f * degrees;
}

void HelloLighting::OnUpdate()
{
  auto endTime = std::chrono::steady_clock::now();
  float dt = std::chrono::duration<float>(endTime - m_startTime).count();
  m_startTime = endTime;

  m_camera.OnFrame(*this, dt);

  m_angle = std::fmodf(m_angle + m_speed * dt, 2.f * std::numbers::pi_v<float>);

  glUseProgram(m_program);

  GLuint rotationLoc = glGetUniformLocation(m_program, "rotation");
  GLuint scaleLoc = glGetUniformLocation(m_program, "scale");

  int window_width = 0, window_height = 0;
  glfwGetWindowSize(GetWindow(), &window_width, &window_height);
  float aspect = (float)window_width / (float)window_height;

  float range_z = m_far_z - m_near_z;
  float projection_fov = 1.f / std::tanf(Radians(m_fov / 2.f));

  glm::mat4 translation = {
  1.f, 0.f, 0.f, m_translation_x,
  0.f, 1.f, 0.f, m_translation_y,
  0.f, 0.f, 1.f, m_translation_z,
  0.f, 0.f, 0.f, 1.f,
  };

  glm::mat4 projection = {
    projection_fov, 0.f, 0.f, 0.f,
    0.f, projection_fov * aspect, 0.f, 0.f,
    0.f, 0.f, (m_far_z + m_near_z) / range_z, - (2.f * m_far_z * m_near_z) / range_z,
    0.f, 0.f, 1.f, 0.f,
  };

  glm::mat4 rotation_z = {
    std::cos(m_angle), -std::sin(m_angle), 0.f, 0.f,
    std::sin(m_angle),  std::cos(m_angle), 0.f, 0.f,
                  0.f,                0.f, 1.f, 0.f,
                  0.f,                0.f, 0.f, 1.f,
  };

  glm::mat4 rotation_y = {
    std::cos(m_angle),  0.f, std::sin(m_angle), 0.f,
                  0.f,  1.f,             0.f,   0.f,
    -std::sin(m_angle), 0.f, std::cos(m_angle), 0.f,
                   0.f, 0.f,             0.f,   1.f,
  };

  glm::mat4 camera = m_camera.GetViewTransform();

  glUniformMatrix4fv(glGetUniformLocation(m_program, "rotation_z"), 1, GL_TRUE, glm::value_ptr(rotation_z));
  glUniformMatrix4fv(glGetUniformLocation(m_program, "rotation_y"), 1, GL_TRUE, glm::value_ptr(rotation_y));
  glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_TRUE, glm::value_ptr(projection));
  glUniform1f(glGetUniformLocation(m_program, "scale"), m_cube_scale);
  glUniformMatrix4fv(glGetUniformLocation(m_program, "translation"), 1, GL_TRUE, glm::value_ptr(translation));
  glUniformMatrix4fv(glGetUniformLocation(m_program, "camera"), 1, GL_TRUE, glm::value_ptr(camera));
  glUniform1i(glGetUniformLocation(m_program, "diffuseTex"), 0);
  glUniform1i(glGetUniformLocation(m_program, "normalsTex"), 1);
}

void HelloLighting::OnRender()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();

  ImGui::Begin("hello-lighting");

    ImGui::InputFloat("Rotation velocity", &m_speed, 0.5f, 1.0f, "%.1f");
    ImGui::InputFloat("Fov", &m_fov, 0.1f, 0.f, "%.1f");
    ImGui::InputFloat("Cube scale", &m_cube_scale, 0.1f, 0.f, "%.1f");
    ImGui::InputFloat("Near Z", &m_near_z, 0.1f, 0.f, "%.1f");
    ImGui::InputFloat("Far Z", &m_far_z, 0.1f, 0.f, "%.1f");
    ImGui::InputFloat("Translation X", &m_translation_x, 0.1f, 0.f, "%.1f");
    ImGui::InputFloat("Translation Y", &m_translation_y, 0.1f, 0.f, "%.1f");
    ImGui::InputFloat("Translation Z", &m_translation_z, 0.1f, 0.f, "%.1f");
    ImGui::InputFloat("Camera velocity", &m_camera_velocity, 0.1f, 0.f, "%.1f");

  ImGui::End();

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (size_t i = 0u; i < m_vaos.size(); i++) {
    glBindVertexArray(m_vaos[i]);

    glDrawElements(GL_TRIANGLES, m_meshes[i].m_indices.size(), GL_UNSIGNED_INT, nullptr);
  }
  glBindVertexArray(0);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main()
{
  HelloLighting application;
  application.Run();
  return 0;
}
