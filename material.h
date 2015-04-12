#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector3D>


class Material
{
public:
    Material();
    void SetColor(QVector3D& a_Color){m_Color = a_Color;}
    void SetDiffuse(float a_Diff){m_Diff = a_Diff;}
    void SetSpecular(float a_Spec){m_Spec = a_Spec;}
    void SetReflection(float a_Refl){m_Refl = a_Refl;}
    void SetRefraction(float a_Refr){m_Refr = a_Refr;}
    void SetRefrIndex(float a_Rindex){m_Rindex = a_Rindex;}
    QVector3D GetColor(){return m_Color;}
    float GetDiffuse(){return m_Diff;}
    float GetSpecular(){return m_Spec;}
    float GetReflection(){return m_Refl;}
    float GetRefraction(){return m_Refr;}
    float GetRefrIndex(){return m_Rindex;}
    ~Material();
private:
    QVector3D m_Color;
    float m_Refl, m_Refr;
    float m_Diff, m_Spec;
    float m_Rindex;
};

#endif // MATERIAL_H
