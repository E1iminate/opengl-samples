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

set(TARGET imgui)

set(SOURCES
  ${ROOT_DIR}/3rd-party/imgui/imgui.cpp
  ${ROOT_DIR}/3rd-party/imgui/imgui_demo.cpp
  ${ROOT_DIR}/3rd-party/imgui/imgui_draw.cpp
  ${ROOT_DIR}/3rd-party/imgui/imgui_tables.cpp
  ${ROOT_DIR}/3rd-party/imgui/imgui_widgets.cpp
  ${ROOT_DIR}/3rd-party/imgui/backends/imgui_impl_opengl3.cpp
  ${ROOT_DIR}/3rd-party/imgui/backends/imgui_impl_glfw.cpp
  ${ROOT_DIR}/3rd-party/imgui/misc/cpp/imgui_stdlib.cpp
)
    
set(HEADERS
  ${ROOT_DIR}/3rd-party/imgui/imconfig.h
  ${ROOT_DIR}/3rd-party/imgui/imgui.h
  ${ROOT_DIR}/3rd-party/imgui/imgui_internal.h
  ${ROOT_DIR}/3rd-party/imgui/imstb_rectpack.h
  ${ROOT_DIR}/3rd-party/imgui/imstb_textedit.h
  ${ROOT_DIR}/3rd-party/imgui/imstb_truetype.h
  ${ROOT_DIR}/3rd-party/imgui/backends/imgui_impl_opengl3_loader.h
  ${ROOT_DIR}/3rd-party/imgui/backends/imgui_impl_opengl3.h
  ${ROOT_DIR}/3rd-party/imgui/backends/imgui_impl_glfw.h
  ${ROOT_DIR}/3rd-party/imgui/misc/cpp/imgui_stdlib.h
)

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${TARGET} PUBLIC
  ${ROOT_DIR}/3rd-party/imgui
  ${ROOT_DIR}/3rd-party/imgui/backends
)

target_link_libraries(${TARGET} PRIVATE glfw)
