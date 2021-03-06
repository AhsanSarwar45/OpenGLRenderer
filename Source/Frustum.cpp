#include "Frustum.hpp"

#include "Camera.hpp"

std::vector<glm::vec4> GetFrustumCornersWorldSpace(const ViewProjection& viewProjection)
{
    const glm::mat4 inverseProjectionView = glm::inverse(viewProjection.projectionMatrix * viewProjection.viewMatrix);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 point = inverseProjectionView * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(point / point.w);
            }
        }
    }

    return frustumCorners;
}