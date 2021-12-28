cmake_minimum_required(VERSION 3.16)

project(OpenGLV2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CPP_LINKER_FLAGS "-loleaut32 -limm32 -lversion -lole32")

add_subdirectory("Vendor/tinyobjloader")
add_subdirectory("Vendor/glm")
add_subdirectory("Vendor/GLFW")
add_subdirectory("Vendor/Glad")

add_executable(${PROJECT_NAME}
"Source/main.cpp"
"Source/Window.cpp"
"Source/Texture.cpp"
"Source/Model.cpp"
"Source/Mesh.cpp"
"Source/Shader.cpp"
"Source/Camera.cpp"
"Source/Skybox.cpp"
"Source/Vertex.cpp"
"Source/Billboard.cpp"
"Vendor/ImGui/imgui.cpp"
"Vendor/ImGui/imgui_demo.cpp"
"Vendor/ImGui/imgui_draw.cpp"
"Vendor/ImGui/imgui_tables.cpp"
"Vendor/ImGui/imgui_widgets.cpp"
"Vendor/ImGui/backends/imgui_impl_glfw.cpp"
"Vendor/ImGui/backends/imgui_impl_opengl3.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME} PRIVATE "${CPP_LINKER_FLAGS}")

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      _CRT_SECURE_NO_WARNINGS
)

target_include_directories(${PROJECT_NAME} PUBLIC "Source")
target_include_directories(${PROJECT_NAME} PUBLIC "Vendor")
target_include_directories(${PROJECT_NAME} PUBLIC "Vendor/ImGui")

target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC stdc++fs)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC tinyobjloader)