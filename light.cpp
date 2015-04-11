#include "light.h"

Light::Light(const QVector3D& _position, const QVector3D& _color, float _intensity)
    :position(_position), color(_color), intensity(_intensity) {}

Light::~Light()
{

}


