#include "material.h"

Material::Material()
{
    m_Color = QVector3D(51,51,51);
    m_Refl = 0;
    m_Refr = 0;
    m_Diff = 0.2f;
    m_Spec = 0.8f;
    m_Rindex = 1.5f;
}

Material::~Material()
{

}

