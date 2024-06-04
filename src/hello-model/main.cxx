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

#include "common.hxx"
#include "core/application.hxx"
#include "core/exceptions.hxx"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <cmath>
#include <chrono>
#include <iostream>
#include <numbers>
#include <string>

void LoadModel()

void Render()
{
  if (glfwInit() != GLFW_TRUE)
    throw LibrarInit;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Tutorial", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  auto callback = [] (GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); };

  glfwSetFramebufferSizeCallback(window, callback);

  tinyobj::Model model(GetCurrentExecutableDirectory() / "assets/models/cube.obj", GetCurrentExecutableDirectory() / "assets/models");

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.IniFilename = nullptr;

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();

  if (gladLoadGL() == 0)
    return -1;

  GLuint texture = -1;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  ProcessImage(GetCurrentExecutableDirectory() / "assets/textures/wooden_container.jpg", [](stb::Image& image)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.Width(), image.Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.Bytes());
  });
  glGenerateMipmap(GL_TEXTURE_2D);

  std::vector<GLuint> vertex_indexes;

  for (tinyobj::index_t index : model.Shapes()[0].mesh.indices)
  {
    vertex_indexes.push_back(index.vertex_index);
  }

  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indexes.size() * sizeof(vertex_indexes[0]), vertex_indexes.data(), GL_STATIC_DRAW);


  GLuint vbo[2];
  glGenBuffers(2, vbo);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, model.Attrib().vertices.size() * sizeof(model.Attrib().vertices[0]), model.Attrib().vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, model.Attrib().texcoords.size() * sizeof(model.Attrib().texcoords[0]), model.Attrib().texcoords.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
  glEnableVertexAttribArray(1);

  const GLchar* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 texCoord;
uniform mat4 rotation;
uniform mat4 scale;
void main()
{
  gl_Position = rotation * scale * vec4(aPos, 1.0);
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

  GLuint program = glCreateProgram();

  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);

  glLinkProgram(program);


  auto startTime = std::chrono::steady_clock::now();
  float angle = 0.f;
  float speed = 1.f;
  float scalingFactor = 0.1f;

  glEnable(GL_DEPTH_TEST);
  while (!glfwWindowShouldClose(window)) {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGui::InputFloat("Rotation velocity", &speed, 0.5f, 1.0f, "%.1f");
    ImGui::InputFloat("Scale", &scalingFactor, 0.1f, 0.f, "%.1f");
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    GLuint rotationLoc = glGetUniformLocation(program, "rotation");
    GLuint scaleLoc = glGetUniformLocation(program, "scale");

    auto endTime = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(endTime - startTime).count();
    startTime = endTime;

    angle = std::fmodf(angle + speed * dt, 2.f * std::numbers::pi_v<float>);


    glm::mat4 scale =
    {
      scalingFactor,   0.f,   0.f, 0.f,
        0.f, scalingFactor,   0.f, 0.f,
        0.f,   0.f, scalingFactor, 0.f,
        0.f,   0.f,   0.f, 1.f,
    };

    glm::mat4 rotation_z = {
      std::cos(angle), -std::sin(angle), 0.f, 0.f,
      std::sin(angle),  std::cos(angle), 0.f, 0.f,
                  0.f,              0.f, 1.f, 0.f,
                  0.f,              0.f, 0.f, 1.f,
    };

    glm::mat4 rotation_y = {
      std::cos(angle),  0.f, std::sin(angle), 0.f,
                  0.f,  1.f,             0.f, 0.f,
      -std::sin(angle), 0.f, std::cos(angle), 0.f,
                  0.f,  0.f,             0.f, 1.f,
    };

    glUniformMatrix4fv(rotationLoc, 1, GL_TRUE, glm::value_ptr(rotation_y));
    glUniformMatrix4fv(scaleLoc, 1, GL_TRUE, glm::value_ptr(scale));

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, vertex_indexes.size(), GL_UNSIGNED_INT, nullptr);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}

int main()
{
  engine::glfw::Application application();
}

