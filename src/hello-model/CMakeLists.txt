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

set(TARGET hello-model)

set(SOURCES include/hello-model/hello-model.hxx hello-model.cxx main.cxx)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} engine glad stbimage glm imgui tinyobjloader common)

target_include_directories(${TARGET}
PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include
)

source_group(Shaders FILES
  ${ROOT_DIR}/shaders/vertex.glsl
  #${ROOT_DIR}/shaders/fragment.glsl
)

copy_assets(${TARGET})
