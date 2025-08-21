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

#include "hello-model/hello-model.hxx"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <numbers>

HelloModel::HelloModel()
{
  m_model = std::make_unique<tinyobj::Model>(GetCurrentExecutableDirectory() / "assets/models/cube.obj", GetCurrentExecutableDirectory() / "assets/models/");

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

void HelloModel::LoadAssets()
{
  GLuint texture = -1;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  ProcessImage(GetCurrentExecutableDirectory() / "assets/textures/LearnOpenGL/container.jpg", [](stb::Image& image)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.Width(), image.Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.Bytes());
    });
  glGenerateMipmap(GL_TEXTURE_2D);

  for (tinyobj::index_t index : m_model->Shapes()[0].mesh.indices)
  {
    m_vertices.push_back({m_model->Attrib().vertices[3 * index.vertex_index], m_model->Attrib().vertices[3 * index.vertex_index + 1], m_model->Attrib().vertices[3 * index.vertex_index + 2]});
    m_texcoords.push_back({m_model->Attrib().texcoords[2 * index.texcoord_index], m_model->Attrib().texcoords[2 * index.texcoord_index + 1]});
  }

  GLuint vbo[2];
  glGenBuffers(2, vbo);

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(m_vertices[0]), m_vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, m_texcoords.size() * sizeof(m_texcoords[0]), m_texcoords.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
  glEnableVertexAttribArray(1);

  const GLchar* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 texCoord;
uniform mat4 rotation_y;
uniform mat4 rotation_z;
uniform float scale;
uniform mat4 translation;
uniform mat4 projection;

void main()
{
  gl_Position = projection * translation * rotation_z * rotation_y * vec4(scale * aPos, 1.0);
  texCoord = aTexCoord;
}
)";

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  //FragColor = texture(ourTexture, texCoord);
  const GLchar* fragmentShaderSource = R"(
#version 330 core
in vec2 texCoord;
out vec4 FragColor;
uniform sampler2D ourTexture;
void main()
{
  FragColor = texture(ourTexture, texCoord);
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

HelloModel::~HelloModel()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

static constexpr float Radians(float degrees) {
  return std::numbers::pi_v<float> / 180.f * degrees;
}

void HelloModel::OnUpdate()
{
  auto endTime = std::chrono::steady_clock::now();
  float dt = std::chrono::duration<float>(endTime - m_startTime).count();
  m_startTime = endTime;

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

  glUniformMatrix4fv(glGetUniformLocation(m_program, "rotation_z"), 1, GL_TRUE, glm::value_ptr(rotation_z));
  glUniformMatrix4fv(glGetUniformLocation(m_program, "rotation_y"), 1, GL_TRUE, glm::value_ptr(rotation_y));
  glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_TRUE, glm::value_ptr(projection));
  glUniform1f(glGetUniformLocation(m_program, "scale"), m_cube_scale);
  glUniformMatrix4fv(glGetUniformLocation(m_program, "translation"), 1, GL_TRUE, glm::value_ptr(translation));
}

void HelloModel::OnRender()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();
  ImGui::InputFloat("Rotation velocity", &m_speed, 0.5f, 1.0f, "%.1f");
  ImGui::InputFloat("Fov", &m_fov, 0.1f, 0.f, "%.1f");
  ImGui::InputFloat("Cube scale", &m_cube_scale, 0.1f, 0.f, "%.1f");
  ImGui::InputFloat("Near Z", &m_near_z, 0.1f, 0.f, "%.1f");
  ImGui::InputFloat("Far Z", &m_far_z, 0.1f, 0.f, "%.1f");
  ImGui::InputFloat("Translation X", &m_translation_x, 0.1f, 0.f, "%.1f");
  ImGui::InputFloat("Translation Y", &m_translation_y, 0.1f, 0.f, "%.1f");
  ImGui::InputFloat("Translation Z", &m_translation_z, 0.1f, 0.f, "%.1f");

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
