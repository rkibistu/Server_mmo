#pragma once

#include <components/SceneObject.h>


struct BoundingSphere {
    rml::Vector3 center;
    float radius;
};

class SphereObject : public SceneObject{
    friend class PhysicsController;

public:
    SphereObject(unsigned int id, std::string name);
    virtual ~SphereObject() override;

    void GenerateBoundingSphere();

    inline BoundingSphere* GetBoundingSphere() { return _boundingSphere; }
    
    rml::Vector3 Velocity = rml::Vector3(0,0,0);

protected:

protected:
    BoundingSphere* _boundingSphere = nullptr;
};
