#pragma once

#include <cstdint>

using WindowDimension = uint16_t;
using Framebuffer     = unsigned int;
using Renderbuffer    = unsigned int;

using TextureDimension = uint16_t;
using TextureId        = unsigned int;

using ShaderProgram = unsigned int;

using ShaderStageId = unsigned int;
using ShaderType    = unsigned int; // TODO rename this

using UniformLocation      = unsigned int;
using UniformBufferObject  = unsigned int;
using UniformBufferBinding = unsigned int;

using VertexBuffer  = unsigned int;
using VertexArray   = unsigned int;
using ElementBuffer = unsigned int;

using MaterialId = uint32_t;