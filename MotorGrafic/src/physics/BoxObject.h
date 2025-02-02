#pragma once

#include <components/SceneObject.h>


struct BoundingVolume {
    rml::Vector3 Min;
    rml::Vector3 Max;
};

class BoxObject : public SceneObject {
    friend class PhysicsController;

public:
    BoxObject(unsigned int id, std::string name);
    ~BoxObject() override;

    void CustomDraw() override;

    void GenerateBoundingVolume();

    inline BoundingVolume* GetBoundingVolume() { return _boundingVolume; }

    rml::Vector3 Velocity = rml::Vector3(0, 0, 0);

protected:
    BoundingVolume* _boundingVolume = nullptr;
};
