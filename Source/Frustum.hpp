#pragma once

#include <vector>

#include <glm/glm.hpp>

std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& projection, const glm::mat4& view);
