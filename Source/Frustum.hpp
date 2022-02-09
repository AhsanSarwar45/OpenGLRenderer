#pragma once

#include <vector>

#include <glm/glm.hpp>

struct ViewProjection;

std::vector<glm::vec4> GetFrustumCornersWorldSpace(const ViewProjection& viewProjection);
