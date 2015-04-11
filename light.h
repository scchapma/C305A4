#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>

class Light
{
public:
    Light(const QVector3D& _position, const QVector3D& _color, float _intensity);
    ~Light();

    QVector3D position;
    QVector3D color;
    float intensity;
};

#endif // LIGHT_H

