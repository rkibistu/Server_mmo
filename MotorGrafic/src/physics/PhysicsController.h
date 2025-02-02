#pragma once

#include <unordered_map>

#include <physics/SphereObject.h>
#include <physics/BoxObject.h>
#include <physics/ObbObject.h>

struct BoxesArrays {

	rml::Vector3* Velocity;
	rml::Vector3* Position;
	rml::Vector3* MinAABB;
	rml::Vector3* MaxAABB;
	unsigned int* IDs;
};

struct SphereArrays {
	rml::Vector3* Velocity;
	rml::Vector3* Position;
	float* Radius;
	unsigned int* IDs;
};

struct CollisionPoint {
	unsigned int Object1;
	unsigned int Object2;
	rml::Vector3 Penetration;
};


/**
 * Singleton class used to manage all objects that exist in the world.
 *
 * Later, parse xml file and create all the objects
 */
class PhysicsController {

public:
	static PhysicsController& GetInstance();
	void DestroyInstance();

	void PopulateTheScene(int spheresCount, int boxesCount, int conesCount);
	void ClearTheScene();
	void EnforceMovement(rml::Vector3 maxVelocity);

	SphereObject* CreateAndAddSphereObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, SceneObject* parent = nullptr);
	BoxObject* CreateAndAddBoxObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, SceneObject* parent = nullptr);
	ObbObject* CreateAndAddObbObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, SceneObject* parent = nullptr);

	std::unordered_map<unsigned int, SphereObject*>& GetSphereObjects() { return _sphereObjects; }
	std::unordered_map<unsigned int, BoxObject*>& GetBoxObjects() { return _boxObjects; }

	inline rml::Vector3 GetBoxMinBound() { return _boxMinBound; }
	inline rml::Vector3 GetBoxMaxBound() { return _boxMaxBound; }

	rml::Vector3 GetRandomPositionInsideBox();

	void Update(float deltaTime);
	void UpdateGpu(float deltaTime);

	int GetNumberOfObjects() {
		return _boxObjects.size() + _sphereObjects.size();
	}
private:
	static PhysicsController* _spInstance;
	PhysicsController();
	PhysicsController(const PhysicsController&) = delete;
	PhysicsController& operator= (const PhysicsController&) = delete;

	std::unordered_map<unsigned int, SphereObject*> _sphereObjects;
	std::unordered_map<unsigned int, BoxObject*> _boxObjects;
	std::unordered_map<unsigned int, ObbObject*> _obbObjects;

	void UpdatePosition(SphereObject* obj, float dt);
	void UpdatePosition(BoxObject* obj, float dt);
	void UpdatePosition(ObbObject* obj, float dt);
	void ApplyGravity(SphereObject* obj, float dt);
	void ApplyGravity(BoxObject* obj, float dt);
	void ApplyGravity(ObbObject* obj, float dt);
	void ApplyFriction(BoxObject* obj, float dragCoefficient);
	void ApplyFriction(SphereObject* obj, float dragCoefficient);
	void ApplyFriction(ObbObject* obj, float dragCoefficient);
	void HandleWallCollision(SphereObject* obj);
	void HandleWallCollision(BoxObject* obj);
	void HandleWallCollision(ObbObject* obj);
	void HandleObjectsCollision(SphereObject* obj1, SphereObject* obj2);
	void HandleObjectsCollision(BoxObject* obj1, BoxObject* obj2);
	void HandleObjectsCollision(ObbObject* obj1, ObbObject* obj2);
	void HandleObjectsCollision(BoxObject* box, SphereObject* sphere);

	float ProjectOBB(OBB& obb, rml::Vector3& axis);
	bool TestAxis(ObbObject* obj1, ObbObject* obj2, rml::Vector3& axis, float& minPenetration, rml::Vector3& minPenetrationAxis);

	bool CheckOverlap(const rml::Vector3& min1, const rml::Vector3& max1, const rml::Vector3& min2, const rml::Vector3& max2);

	void AllocCollisionArraysBox();
	void AllocCollisionArraysSphere();
	void GenerateSphereArrays();
	void GenerateBoxesArrays();
	void PostGpuBoxes();
	void PostGpuSpheres();


	void DetectBoxCollisions(std::vector<CollisionPoint>& collisionPoints);
	/** Return the Penetraion or (0,0,0) if none */
	rml::Vector3 CalculateBoxesCollision(BoxObject* obj1, BoxObject* obj2);
	void ResolveBoxCollisions(std::vector<CollisionPoint>& collisionPoints);
	void ResolveBoxCollisionsGeneratedByGpu(CollisionPoint* collisionPoints, int* collisionPointsCount);

	void DetectSphereCollisions(std::vector<CollisionPoint>& collisionPoints);
	void ResolveSphereCollisions(std::vector<CollisionPoint>& collisionPoints);
	void ResolveSphereCollisionsGeneratedByGpu(CollisionPoint* collisionPoints, int* collisionPointsCount);


private:
	rml::Vector3 _boxMinBound = rml::Vector3(-5, 1, -5);
	rml::Vector3 _boxMaxBound = rml::Vector3(5, 60, 5);
	float _gravity = 40;

	float _wallRestitution = 0.5f;
	float _collisionRestitution = 0.45f;
	float _dumping = 0.999f;

	int _oldBoxesCount = 0;
	BoxesArrays _boxes;

	int _oldSpheresCount = 0;
	SphereArrays _spheres;

	int _collisionPointsMaxCountBox = 0;
	int* _generatedPointsCountBox = nullptr;
	CollisionPoint* _collisionPointsBox = nullptr;

	int _collisionPointsMaxCountSphere = 0;
	int* _generatedPointsCountSphere = nullptr;
	CollisionPoint* _collisionPointsSphere = nullptr;

	std::vector<CollisionPoint> _sphereCpuCollisions;
};

