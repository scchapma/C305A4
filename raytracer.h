#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <QWidget>
#include <glwidget.h>
#include <vector>
#include "shape.h"
//#include "camera.h"

class RayTracer
{
public:
    RayTracer();
    //Camera initCamera();
    bool rayTrace(HitRecord &rec, int i, int j, std::vector<Shape*> shapes);
    void jitter(QVector2D* samples, int sampleSize);
    void initRender();
    void render(QImage &myimage, int width, int height);
    ~RayTracer();

protected:
    float m_leftX, m_rightX, m_topY, m_bottomY, m_dX, m_dY, m_targetX, m_targetY;
    int m_Width, m_Height;
};

#endif

