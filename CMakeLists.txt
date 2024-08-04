##########################################################################
# Copyright 2024 Vladislav Riabov
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

cmake_minimum_required(VERSION 3.25)
project(opengl-samples)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
list(APPEND CMAKE_MODULE_PATH ${ROOT_DIR}/cmake/modules)

set(ASSETS_DIR ${ROOT_DIR}/assets)

function(copy_assets TARGET_NAME)
  set(TARGET_ASSETS $<TARGET_FILE_DIR:${TARGET_NAME}>/assets)
  
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${TARGET_ASSETS}
  )
  
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${ASSETS_DIR}
      ${TARGET_ASSETS}
  )
endfunction()

find_package(glad REQUIRED)
find_package(imgui REQUIRED)
find_package(stbimage REQUIRED)
find_package(tinyobjloader REQUIRED)
add_subdirectory(${ROOT_DIR}/3rd-party/glfw)
add_subdirectory(${ROOT_DIR}/3rd-party/glm)

add_subdirectory(src)
