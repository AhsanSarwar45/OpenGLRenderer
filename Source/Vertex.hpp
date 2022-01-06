#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include <glm/gtx/hash.hpp>

#include "Utilities/Hash.hpp"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    bool operator==(const Vertex& other) const;
};

template <>
struct std::hash<Vertex>
{
    std::size_t operator()(Vertex const& s) const
    {
        std::size_t res = 0;
        HashCombine(res, s.Position);
        HashCombine(res, s.Normal);
        HashCombine(res, s.TexCoord);
        return res;
    }
};