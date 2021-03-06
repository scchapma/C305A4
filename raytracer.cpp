#include "raytracer.h"

#include <vector>
#include "shape.h"
#include "sphere.h"
#include "triangle.h"
#include "plane.h"
#include "light.h"
//#include "camera.h"

const unsigned short int sampleSize = 1;

const QVector3D c (0, 0, 0);
const QVector3D gaze (0, 0, 0);
const QVector3D vup (0, 1, 0);

RayTracer::RayTracer()
{

}

/*
Camera RayTracer::initCamera()
{
    //QVector3D origin (672, 468, 500);
    QVector3D c (500, 700, 300);
    //QVector3D c (500, 268, 200);
    QVector3D gaze (0, -50, -100);
    //QVector3D gaze (0, 0, -1);
    QVector3D vup (0, 1, 0);
    const float left = 0.0;
    const float right = 1344.0;
    const float bottom = 936.0;
    const float top = 0.0;
    const float distance = 500.0;
    Camera camera = Camera(c, gaze, vup, left, right, bottom, top, distance);
    return camera;
}
*/

/*
bool RayTracer::rayTrace(HitRecord &rec, int i, int j, std::vector<Shape*> shapes)
{
    //Camera camera = initCamera();    

    bool is_a_hit;
    bool sample_hit;
    float tmax;
    //QVector3D dir (0, 0, -1);

    tmax = 100000.0f;
    is_a_hit = false;

    //initialize sampling values
    float t = 0;
    QVector3D normal (0, 0, 0);
    QVector3D intersectionPoint (0, 0, 0);
    QVector3D color (0, 0, 0);

    //QVector3D origin (672, 468, 500);

    QVector3D origin (0, 0, 1);

    //add sampling
    QVector2D samples[sampleSize*sampleSize];

    jitter(samples, sampleSize);

    for(int c = 0; c < sampleSize*sampleSize; c++){
        sample_hit = false;
        tmax = 100000.0f;
        //QVector3D origin(i + samples[c].x() - 0.5, j + samples[c].y() - 0.5, 0);
        QVector3D dir(QVector3D(i, j, 0) - origin);
        dir.normalized();
        //cout << "samples[c].x: " << i + samples[c].x() - 0.5 << endl;
        //cout << "samples[c].y: " << j + samples[c].y() - 0.5 << endl;
        Ray r(origin, dir);
        //Ray r = camera.getRay(i + samples[c].x() - 0.5, j + samples[c].y() - 0.5, 0);
        //Ray r = camera.getRay(i, j, 0);
        //cout << "r.dir.x: " << r.direction().x() << endl;
        //cout << "r.dir.y: " << r.direction().x() << endl;
        //cout << "r.dir.z: " << r.direction().x() << endl;

        for (int k = 0; k < (int)shapes.size(); k++)
        {
            if (shapes[k]->hit(r, .00001f, tmax, rec))
            {
                tmax = rec.t;
                is_a_hit = true;
                sample_hit = true;
            }
        }
        t += rec.t;
        normal += rec.normal;
        intersectionPoint += rec.intersectionPoint;
        if(sample_hit) color += rec.color;
        else color += QVector3D(160,160,160);
    }

    rec.t = t/(sampleSize*sampleSize);
    rec.normal = normal/(sampleSize*sampleSize);
    rec.intersectionPoint = intersectionPoint/(sampleSize*sampleSize);
    rec.color = color/(sampleSize*sampleSize);
    rec.clamp();
    rec.normal = rec.normal.normalized();

    return is_a_hit;
}
*/

bool RayTracer::rayTrace(HitRecord &rec, int i, int j, std::vector<Shape*> shapes)
{
    bool is_a_hit;
    float tmax;

    tmax = 100000.0f;
    is_a_hit = false;

    QVector3D origin (0, 0, 500);
    QVector3D dir(QVector3D(i, j, 0) - origin);
    dir.normalized();

    /*
    //orthographic
    QVector3D origin (i, j, 0);
    QVector3D dir(0,0,-1);
    */

    //cout << "samples[c].x: " << i + samples[c].x() - 0.5 << endl;
    //cout << "samples[c].y: " << j + samples[c].y() - 0.5 << endl;
    Ray r(origin, dir);

    for (int k = 0; k < (int)shapes.size(); k++)
    {
        if (shapes[k]->hit(r, .00001f, tmax, rec))
        {
            //tmax = rec.t;
            is_a_hit = true;
            rec.clamp();
        }
    }

    return is_a_hit;
}


void RayTracer::jitter(QVector2D* samples, int sampleSize)
{
    for(int a = 0; a < sampleSize; a++){
        for(int b = 0; b < sampleSize; b++)
        {
            float x = ((double)a + drand48()) / sampleSize;
            float y = ((double)b + drand48()) / sampleSize;
            (samples[a*sampleSize + b]).setX(x);
            (samples[a*sampleSize + b]).setY(y);
        }
    }

}

void RayTracer::initRender()
{
    //TODO: Hardcoded - fix later
    m_Width = 1200;
    m_Height = 900;

    //set up image plane - 8 wide by 6 high
    //m_leftX = -4, m_rightX = 4, m_topY = 3, m_bottomY = -3;
    m_leftX = -600, m_rightX = 600, m_topY = 450, m_bottomY = -450;
    //calculate deltas
    m_dX = (m_rightX - m_leftX)/m_Width;
    //std::cout << "dX: " << m_dX << endl;
    m_dY = (m_topY - m_bottomY)/m_Height;
    //std::cout << "dY: " << m_dY << endl;
    //start at top left corner
    m_targetY = m_topY;
    m_targetX = m_leftX;
}

void RayTracer::render(QImage &myimage, int renderWidth, int renderHeight)
{
    HitRecord rec;
    QVector3D incidentLightRay;
    QVector3D surfaceNormal;

    int counter = 0;

    //geometry
    std::vector<Shape*> shapes;
    std::vector<Light*> lights;

    //init shapes
    shapes.push_back(new Sphere (QVector3D(0, 0, -100), 100, QVector3D(255, 0, 0)));
    shapes.push_back(new Sphere (QVector3D(-90, 0, -200), 100, QVector3D(255, 215, 0)));
    //shapes.push_back(new Sphere (QVector3D(675, 400, -225), 150, QVector3D(255, 0, 0)));
    //shapes.push_back(new Sphere (QVector3D(350, 400, -200), 150, QVector3D(255, 215, 0)));
    //shapes.push_back(new Sphere (QVector3D(100, 100, -1000), 50, QVector3D(0, 0, 255)));
    //shapes.push_back(new Sphere (QVector3D(600, 400, -300), 150, QVector3D(139, 0, 139)));

    shapes.push_back(new Plane (QVector3D(0, -1, 0), -100, QVector3D(0.4*256,0.3*256,0.3*256)));
    //shapes.push_back(new Plane (QVector3D(0, 0, -1), -110, QVector3D(0,0,255)));

    //init lights
    lights.push_back(new Light(QVector3D(150,150,300), QVector3D(255,255,255), 1.0));

    for (int j = 0; j < renderHeight; j++)
    {
        m_targetX = m_leftX;
        for (int i = 0; i < renderWidth; i++)
        {
            //if (rayTrace(rec, i, j, shapes))
            //std::cout << "targetX: " << m_targetX << endl;
            //std::cout << "targetY: " << m_targetY << endl;
            if (rayTrace(rec, m_targetX, m_targetY, shapes))
            {
                myimage.setPixel(i, j, qRgb(rec.color.x(), rec.color.y(), rec.color.z()));
                //myimage.setPixel(i, renderHeight -1 - j, qRgb(rec.color.x(), rec.color.y(), rec.color.z()));
            }
            else
            {
                myimage.setPixel(i, j, qRgb(160,160,160));
                //myimage.setPixel(i, renderHeight- 1 - j, qRgb(160,160,160));
            }
            m_targetX += m_dX;
            counter++;
        }
        m_targetY -= m_dY;
    }
    std::cout << "Counter: " << counter << endl;
}

/*
void RayTracer::render(QImage &myimage, int renderWidth, int renderHeight)
{
    HitRecord rec;
    QVector3D incidentLightRay;
    QVector3D surfaceNormal;

    //geometry
    std::vector<Shape*> shapes;
    std::vector<Light*> lights;

    //init shapes
    shapes.push_back(new Sphere (QVector3D(0, 0, -5), 1.0, QVector3D(255, 255, 0)));
    //shapes.push_back(new Sphere (QVector3D(350, 400, -200), 150, QVector3D(255, 215, 0)));
    //shapes.push_back(new Sphere (QVector3D(675, 400, -225), 150, QVector3D(255, 0, 0)));
    //shapes.push_back(new Sphere (QVector3D(350, 400, -200), 150, QVector3D(255, 215, 0)));
    //shapes.push_back(new Sphere (QVector3D(100, 100, -1000), 50, QVector3D(0, 0, 255)));
    //shapes.push_back(new Sphere (QVector3D(600, 400, -300), 150, QVector3D(139, 0, 139)));

    //shapes.push_back(new Plane (QVector3D(0, 1, 0), 0, QVector3D(255,0,0)));

    //init lights
    lights.push_back(new Light(QVector3D(150,150,300), QVector3D(255,255,255), 1.0));

    QVector3D lightPosition (150, 150, 300);
    //QVector3D lightPosition (0, -150, 300);
    //QVector3D lightPosition (50, 0, 300);
    //QVector3D lightPosition (2000, 450, -100);

    float diffuseFactor;
    float ambientCoefficient = 1.0;
    float diffuseCoefficient = 0.9;
    float specularCoefficient = 0.9;
    int specPower = 50;

    float tmax = 100000.0f;

    for (int i = 0; i < renderWidth; i++)
    {
        m_targetX = m_leftX;
        for (int j = 0; j < renderHeight; j++)
        {
            //if (rayTrace(rec, i, j, shapes))
            if (rayTrace(rec, m_targetX, m_targetY, shapes))
            {
                //add ambient component
                //rec.color *= ambientCoefficient;
                //rec.clamp();


                HitRecord srec;
                //int counter = 0;
                bool is_ambient_hit = false;
                QVector3D p = rec.intersectionPoint;
                QVector3D dir = (lightPosition - p).normalized();
                Ray r(p, dir);

                for (int k = 0; k < (int)shapes.size(); k++)
                {
                    if (shapes[k]->hit(r, .00001f, tmax, srec))
                    {
                        tmax = srec.t;
                        is_ambient_hit = true;
                        //counter++;
                        //cout << "counter: " << counter;
                    }
                }

                if (!is_ambient_hit)
                {
                    //add diffuse component
                    incidentLightRay = (rec.intersectionPoint - lightPosition).normalized();
                    surfaceNormal = rec.normal;
                    diffuseFactor = surfaceNormal.dotProduct(incidentLightRay, surfaceNormal);
                    diffuseFactor *= (-1);
                    rec.color = (diffuseFactor*diffuseCoefficient)*rec.color;

                    //clamp
                    rec.clamp();

                    //add specular component
                    float myDot = - incidentLightRay.dotProduct(incidentLightRay, surfaceNormal);
                    float myLen = 2.0f * myDot;

                    QVector3D tempNormal = myLen * surfaceNormal;
                    QVector3D reflectVector = (tempNormal + incidentLightRay).normalized();

                    float mySpec = 0.0;
                    float tempDot = - reflectVector.dotProduct(reflectVector, incidentLightRay);
                    if (tempDot > 0.0) mySpec = tempDot;

                    mySpec = powf(mySpec, specPower);

                    QVector3D specularColor (255, 255, 255);
                    specularColor = (mySpec*specularCoefficient) * specularColor;

                    //add diffuse and specular components
                    rec.color += specularColor;

                    //clamp
                    rec.clamp();                   
                }

                //myimage.setPixel(i, j, qRgb(rec.color.x(), rec.color.y(), rec.color.z()));
                myimage.setPixel(i, renderHeight -1 - j, qRgb(rec.color.x(), rec.color.y(), rec.color.z()));
            }
            else
            {
                //myimage.setPixel(i, j, qRgb(160,160,160));
                myimage.setPixel(i, renderHeight- 1 - j, qRgb(160,160,160));
            }
            m_targetX += m_dX;
        }
        m_targetY -= m_dY;
    }
}
*/


RayTracer::~RayTracer()
{

}



