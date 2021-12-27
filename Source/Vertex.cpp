#include "Vertex.hpp"

bool Vertex::operator==(const Vertex& other) const
{
    return Position == other.Position && TexCoord == other.TexCoord && Normal == other.Normal;
}