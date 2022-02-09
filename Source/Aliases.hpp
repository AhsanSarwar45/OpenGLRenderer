#pragma once

#include <cstdint>

using WindowDimension    = uint16_t;
using FramebufferObject  = unsigned int;
using RenderbufferObject = unsigned int;

using TextureDimension = uint16_t;
using TextureId        = unsigned int;

using ShaderProgram = unsigned int;

using ShaderStageId = unsigned int;
using ShaderType    = unsigned int; // TODO rename this

using UniformLocation      = unsigned int;
using UniformBufferObject  = unsigned int;
using UniformBufferBinding = unsigned int;

using VertexBufferObject  = unsigned int;
using VertexArrayObject   = unsigned int;
using ElementBufferObject = unsigned int;

using MaterialId = uint32_t;