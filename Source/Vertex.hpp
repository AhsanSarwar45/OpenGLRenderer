#include "glm/glm.hpp"
#include <glm/gtx/hash.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;

    bool operator==(const Vertex& other) const;
};

template <class T>
inline void hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

template <>
struct std::hash<Vertex>
{
    std::size_t operator()(Vertex const& s) const
    {
        std::size_t res = 0;
        hash_combine(res, s.Position);
        hash_combine(res, s.Normal);
        hash_combine(res, s.TexCoord);
        return res;
    }
};