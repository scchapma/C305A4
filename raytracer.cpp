#include "raytracer.h"

#include <vector>
#include "shape.h"
#include "sphere.h"
#include "triangle.h"
#include "plane.h"
#include "light.h"
#include "camera.h"

const unsigned short int sampleSize = 1;

const QVector3D c (0, 0, 0);
const QVector3D gaze (0, 0, 0);
const QVector3D vup (0, 1, 0);

RayTracer::RayTracer()
{

}


Camera RayTracer::initCamera()
{
    //QVector3D origin (672, 468, 500);
    QVector3D c (500, 225, 200);
    //QVector3D c (672, 468, 200);
    //QVector3D c(400,-100,200);
    QVector3D gaze (0, 0, -1);
    //lefthand side
    //QVector3D c (-400, 0, -225);
    //QVector3D gaze (1, 1, 1);
    //behind
    //QVector3D c (-300, 150, -700);
    //QVector3D gaze (0, 0, 1);
    QVector3D vup (0, 1, 0);
    const float left = -300;
    const float right = 300;
    const float bottom = -225;
    const float top = 225;
    const float distance = 200.0;
    Camera camera = Camera(c, gaze, vup, left, right, bottom, top, distance);
    return camera;
}


QVector3D RayTracer::rayTrace(HitRecord &rec, int i, int j, std::vector<Shape*> shapes, std::vector<Light*> lights)
{
    Camera camera = initCamera();

    bool is_a_hit;
    float tmax;
    Shape* closestShape;
    //Shape* obstacle;
    HitRecord srec;

    tmax = 100000.0f;
    is_a_hit = false;

    //QVector3D color (0, 0, 0);
    QVector3D backgroundColor (80, 80, 80);

    QVector3D origin (0, 0, 500);
    QVector3D dir(QVector3D(i, j, 0) - origin);
    dir.normalized();

    //initialize sampling values
    float t = 0;
    QVector3D normal (0, 0, 0);
    QVector3D intersectionPoint (0, 0, 0);
    QVector3D color (0, 0, 0);
    bool sample_hit;

    //add sampling
    QVector2D samples[sampleSize*sampleSize];
    jitter(samples, sampleSize);

    //cout << "samples[c].x: " << i + samples[c].x() - 0.5 << endl;
    //cout << "samples[c].y: " << j + samples[c].y() - 0.5 << endl;
    //Ray r(origin, dir);

    for(int c = 0; c < sampleSize*sampleSize; c++){
        sample_hit = false;
        tmax = 100000.0f;        
        //Ray r = camera.getRay(i + samples[c].x() - 0.5, j + samples[c].y() - 0.5);

        QVector3D origin (0, 0, 500);
        QVector3D dir = QVector3D(i + samples[c].x() - 0.5, j + samples[c].y() - 0.5, 0) - origin;
        Ray r(origin, dir.normalized());

        //find closest intersection
        for (int k = 0; k < (int)shapes.size(); k++)
        {
            if (shapes[k]->hit(r, .00001f, tmax, rec))
            {
                //tmax = rec.t;
                is_a_hit = true;
                closestShape = shapes[k];
                rec.clamp();
                sample_hit = true;
            }
        }
        t += rec.t;
        normal += rec.normal;
        intersectionPoint += rec.intersectionPoint;
        if(sample_hit) color += rec.color;
        else color += backgroundColor;

        rec.t = t/(sampleSize*sampleSize);
        rec.normal = normal/(sampleSize*sampleSize);
        rec.intersectionPoint = intersectionPoint/(sampleSize*sampleSize);
        rec.color = color/(sampleSize*sampleSize);
        rec.clamp();
        rec.normal = rec.normal.normalized();
    }

    if (!is_a_hit) return backgroundColor;

    float specularCoefficient = 0.5;
    int specPower = 50;

    //add diffuse component
    for(int l = 0; l < (int)lights.size(); l++){

        QVector3D incidentLightRay = (lights[l]->position - rec.intersectionPoint).normalized();
        QVector3D surfaceNormal = rec.normal;

        //calculate shadows
        float shade = 1.0f;


        //NB - not normalized
        QVector3D shadowDir = (lights[l]->position - rec.intersectionPoint);
        float tdist = shadowDir.length();
        Ray shadowRay(rec.intersectionPoint + shadowDir*(.00001f), shadowDir.normalized());

        //check for obstructions
        for(int o = 0; o < (int)shapes.size(); o++)
        {
            if (shapes[o]->hit(shadowRay, .00001f, tdist, srec))
            {
                shade = 0.0f;
                break;
            }
        }



        //diffuse shading
        if(closestShape->GetMaterial()->GetDiffuse() > 0.0f)
        {

            float dotLN = fabs(QVector3D::dotProduct(incidentLightRay, surfaceNormal));
            if(dotLN >= 0){
                rec.color = dotLN*(closestShape->GetMaterial()->GetDiffuse())*shade*rec.color*lights[l]->intensity;
                rec.clamp();
            }
        }

        //add specular component
        float myDot = - QVector3D::dotProduct(incidentLightRay, surfaceNormal);
        float myLen = 2.0f * myDot;

        QVector3D tempNormal = myLen * surfaceNormal;
        QVector3D reflectVector = (tempNormal + incidentLightRay).normalized();

        float mySpec = 0.0;
        float tempDot = - QVector3D::dotProduct(reflectVector, incidentLightRay);
        //float tempDot = QVector3D::dotProduct(reflectVector, r.direction());
        if (tempDot > 0.0) mySpec = tempDot;

        mySpec = powf(mySpec, specPower);

        QVector3D specularColor (255, 255, 255);
        specularColor = (mySpec*specularCoefficient) * specularColor*lights[l]->intensity;

        //add diffuse and specular components
        rec.color += specularColor;

        /*
         * calculate reflection
            if((current_depth < depth) && (closestShape.getMat()->getReflectionAmount() > 0.0f)){
            QVector3D Reflected_Vector = r.direction() - 2 * QVector3D::dotProduct(r.direction(), surfaceNormal) * surfaceNormal;
            QVector3D Reflected_Color = raytrace(Ray(intersectionPoint + Reflected_Vector * 0.000001f, Reflected_Vector), current_depth + 1);
            rec.color += Reflected_Color * closestShape.getMat()->getReflectionAmount();
        }
         *
        */

        /*
         * calculate refraction
            if((current_depth < depth) && (closestShape.getMat()->getTransparency() == true)){
            QVector3D Refracted_Vector;
            double n = closestShape.getMat()->getIoR();
            double cosI = QVector3D::dotProduct(surfaceNormal, r.direction());
            double sinT2 = n*n * (1.0 - cosI*cosI);
            if(sinT2 <= 1.0)
            {
                Refracted_Vector = n * r.direction() - (n + sqrtf(1.0 - sinT2)) * surfaceNormal;
                QVector3D Refracted_Color = raytrace(Ray(intersectionPoint + Refracted_Vector * 0.000001f, Refracted_Vector), current_depth + 1);
                rec.color = Refracted_Color;
            }
        }
         */


        //clamp
        rec.clamp();
     }



    color = rec.color;

    return color;
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

void RayTracer::initRender(int renderWidth, int renderHeight)
{

    //set up image plane - 8 wide by 6 high
    //m_leftX = -4, m_rightX = 4, m_topY = 3, m_bottomY = -3;
    //m_leftX = -600, m_rightX = 600, m_topY = 450, m_bottomY = -450;
    m_leftX = -300, m_rightX = 300, m_topY = 225, m_bottomY = -225;
    //calculate deltas
    m_dX = (m_rightX - m_leftX)/renderWidth;    
    m_dY = (m_topY - m_bottomY)/renderHeight;   
    //start at top left corner
    m_targetY = m_topY;
    m_targetX = m_leftX;
}

void RayTracer::render(QImage &myimage, int renderWidth, int renderHeight)
{
    HitRecord rec;

    //geometry
    std::vector<Shape*> shapes;
    std::vector<Light*> lights;

    //init shapes
    Sphere *sphere1 = new Sphere (QVector3D(0, 0, -200), 100, QVector3D(255, 0, 0));
    sphere1->GetMaterial()->SetDiffuse(0.9f);
    sphere1->GetMaterial()->SetReflection(0.9f);
    sphere1->GetMaterial()->SetRefraction(0.8f);
    sphere1->GetMaterial()->SetRefrIndex(1.3f);
    sphere1->GetMaterial()->SetSpecular(0.9f);

    Sphere *sphere2 = new Sphere (QVector3D(-70, 50, -50), 20, QVector3D(255, 215, 0));
    sphere2->GetMaterial()->SetReflection(0.9f);
    sphere2->GetMaterial()->SetRefraction(0.0f);
    sphere2->GetMaterial()->SetRefrIndex(1.3f);
    sphere2->GetMaterial()->SetDiffuse(0.9f);
    sphere2->GetMaterial()->SetSpecular(0.9f);

    Plane *plane1 = new Plane (QVector3D(0, 1, 0), 200, QVector3D(0.9*256,0.9*256,0.9*256));
    plane1->GetMaterial()->SetReflection(0.9f);
    plane1->GetMaterial()->SetRefraction(0.0f);
    plane1->GetMaterial()->SetRefrIndex(1.3f);
    plane1->GetMaterial()->SetDiffuse(0.9f);
    plane1->GetMaterial()->SetSpecular(0.9f);

    shapes.push_back(sphere1);
    shapes.push_back(sphere2);
    shapes.push_back(plane1);

    //init lights
    //lights.push_back(new Light(QVector3D(-150, 300, 100), QVector3D(1.0, 1.0, 1.0), 1.0));
    lights.push_back(new Light(QVector3D(-140, 100, 100), QVector3D(1.0, 1.0, 1.0), 2.0));
    //lights.push_back(new Light(QVector3D(100, 100, 300), QVector3D(1.0, 1.0, 1.0), 1.5));

    for (int j = 0; j < renderHeight; j++)
    {
        m_targetX = m_leftX;
        for (int i = 0; i < renderWidth; i++)
        {            
            QVector3D color = (rayTrace(rec, m_targetX, m_targetY, shapes, lights));
            myimage.setPixel(i, j, qRgb(color.x(), color.y(), color.z()));
            m_targetX += m_dX;
        }
        m_targetY -= m_dY;
    }
}




RayTracer::~RayTracer()
{

}



