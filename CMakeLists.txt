cmake_minimum_required(VERSION 3.16)

project(OpenGLV2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CPP_LINKER_FLAGS "-loleaut32 -limm32 -lversion -lole32")

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT FALSE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE)
set(ASSIMP_BUILD_GLTF_IMPORTER TRUE)
set(ASSIMP_BUILD_FBX_IMPORTER TRUE)

set(ASSIMP_BUILD_M3D_EXPORTER FALSE)
set(ASSIMP_BUILD_ASSBIN_EXPORTER FALSE)
set(ASSIMP_BUILD_ASSXML_EXPORTER FALSE)
set(ASSIMP_BUILD_GLTF_EXPORTER FALSE)
set(ASSIMP_BUILD_OBJ_EXPORTER FALSE)
set(ASSIMP_BUILD_OPENGEX_EXPORTER FALSE)
set(ASSIMP_BUILD_PLY_EXPORTER FALSE)
set(ASSIMP_BUILD_3DS_EXPORTER FALSE)
set(ASSIMP_BUILD_COLLADA_EXPORTER FALSE)
set(ASSIMP_BUILD_FBX_EXPORTER FALSE)
set(ASSIMP_BUILD_STL_EXPORTER FALSE)
set(ASSIMP_BUILD_X_EXPORTER FALSE)
set(ASSIMP_BUILD_X3D_EXPORTER FALSE)
set(ASSIMP_BUILD_3MF_EXPORTER FALSE)
set(ASSIMP_BUILD_ASSJSON_EXPORTER FALSE)
set(ASSIMP_BUILD_STEP_EXPORTER FALSE)

add_subdirectory("Vendor/tinyobjloader")
add_subdirectory("Vendor/glm")
add_subdirectory("Vendor/GLFW")
add_subdirectory("Vendor/Glad")
add_subdirectory("Vendor/efsw")
add_subdirectory("Vendor/assimp")
# add_subdirectory("Vendor/rttr")

# find_package(RTTR CONFIG REQUIRED Core)

add_executable(${PROJECT_NAME}
"Source/main.cpp"
"Source/Window.cpp"
"Source/Texture.cpp"
"Source/Model.cpp"
"Source/Mesh.cpp"
"Source/Shader.cpp"
"Source/Camera.cpp"
"Source/Skybox.cpp"
"Source/Framebuffer.cpp"
"Source/Vertex.cpp"
"Source/Render.cpp"
"Source/Scene.cpp"
"Source/FileWatcher.cpp"
"Source/Billboard.cpp"
"Source/ResourceManager.cpp"
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
target_link_libraries(${PROJECT_NAME} PUBLIC efsw)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)
# target_link_libraries(${PROJECT_NAME} PUBLIC Core_Lib)