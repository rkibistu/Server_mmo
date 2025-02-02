#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "PhysicsController.h"
#include <core/ResourceManager.h>
#include <core/SceneManager.h>
#include <utils/Utils.h>

#include "PhysicsGpu.h"

#include <core/MemoryDebug.h>

PhysicsController* PhysicsController::_spInstance = nullptr;

PhysicsController& PhysicsController::GetInstance() {
	if (_spInstance == nullptr)
		_spInstance = new PhysicsController();
	return *_spInstance;
}

void PhysicsController::DestroyInstance() {

	if (_generatedPointsCountBox)
		delete _generatedPointsCountBox;
	if (_collisionPointsBox)
		delete _collisionPointsBox;

	if (_generatedPointsCountSphere)
		delete _generatedPointsCountSphere;
	if (_collisionPointsSphere)
		delete _collisionPointsSphere;

	if (_boxes.Position) {
		delete _boxes.Position;
		_boxes.Position = nullptr;
	}
	if (_boxes.Velocity) {
		delete _boxes.Velocity;
		_boxes.Velocity = nullptr;
	}
	if (_boxes.MinAABB) {
		delete _boxes.MinAABB;
		_boxes.MinAABB = nullptr;
	}
	if (_boxes.MaxAABB) {
		delete _boxes.MaxAABB;
		_boxes.MaxAABB = nullptr;
	}
	if (_boxes.IDs) {
		delete _boxes.IDs;
		_boxes.IDs = nullptr;
	}

	if (_spheres.Position) {
		delete _boxes.Position;
		_boxes.Position = nullptr;
	}
	if (_spheres.Velocity) {
		delete _boxes.Velocity;
		_boxes.Velocity = nullptr;
	}
	if (_spheres.Radius) {
		delete _boxes.MinAABB;
		_boxes.MinAABB = nullptr;
	}
	if (_spheres.IDs) {
		delete _boxes.IDs;
		_boxes.IDs = nullptr;
	}


	if (_spInstance) {
		delete _spInstance;
		_spInstance = nullptr;
	}
}

void PhysicsController::PopulateTheScene(int spheresCount, int boxesCount, int conesCount) {

	for (int i = 0; i < spheresCount; i++) {
		PhysicsController::GetInstance().CreateAndAddSphereObject(
			"physics",
			"3_sphere",
			"3",
			GetRandomPositionInsideBox(),
			rml::Vector3(0, 0, 0),
			rml::Vector3(1, 1, 1),
			nullptr
		);
	}
	for (int i = 0; i < boxesCount; i++) {
		PhysicsController::GetInstance().CreateAndAddBoxObject(
			"physics",
			"1",
			"3",
			GetRandomPositionInsideBox(),
			rml::Vector3(0, 0, 0),
			rml::Vector3(1, 1, 1),
			nullptr
		);
	}
	for (int i = 0; i < conesCount; i++) {
		PhysicsController::GetInstance().CreateAndAddBoxObject(
			"physics",
			"6_cone",
			"3",
			GetRandomPositionInsideBox(),
			rml::Vector3(0, 0, 0),
			rml::Vector3(1, 1, 1),
			nullptr
		);
	}
}

void PhysicsController::ClearTheScene() {

	for (auto it = _sphereObjects.begin(); it != _sphereObjects.end(); ++it) {

		SceneManager::GetInstance().RemoveObject(it->second->GetID());
	}
	_sphereObjects.clear();

	for (auto it = _boxObjects.begin(); it != _boxObjects.end(); ++it) {

		SceneManager::GetInstance().RemoveObject(it->second->GetID());
	}
	_boxObjects.clear();
}

void PhysicsController::EnforceMovement(rml::Vector3 maxVelocity) {
	for (auto it = _sphereObjects.begin(); it != _sphereObjects.end(); ++it) {

		it->second->Velocity.x = Utils::RandomFloat(-maxVelocity.x, maxVelocity.x);
		it->second->Velocity.y = Utils::RandomFloat(0, maxVelocity.y);
		it->second->Velocity.z = Utils::RandomFloat(-maxVelocity.x, maxVelocity.z);
	}

	for (auto it = _boxObjects.begin(); it != _boxObjects.end(); ++it) {

		it->second->Velocity.x = Utils::RandomFloat(-maxVelocity.x, maxVelocity.x);
		it->second->Velocity.y = Utils::RandomFloat(0, maxVelocity.y);
		it->second->Velocity.z = Utils::RandomFloat(-maxVelocity.x, maxVelocity.x);
	}
}

PhysicsController::PhysicsController() {
	rml::Vector3 boxCenter = (_boxMinBound + _boxMaxBound) * 0.5;
	rml::Vector3 boxSize = (_boxMaxBound - _boxMinBound) * 0.5;

	_boxes.Position = new rml::Vector3[10000];
	_boxes.Velocity = new rml::Vector3[10000];
	_boxes.MinAABB = new rml::Vector3[10000];
	_boxes.MaxAABB = new rml::Vector3[10000];
	_boxes.IDs = new unsigned int[10000];

	_spheres.Position = new rml::Vector3[10000];
	_spheres.Velocity = new rml::Vector3[10000];
	_spheres.Radius = new float[10000];
	_spheres.IDs = new unsigned int[10000];
}

SphereObject* PhysicsController::CreateAndAddSphereObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, SceneObject* parent) {

	unsigned int id = SceneManager::GetInstance().GetSceneObjectID();
	SphereObject* sceneObject = new SphereObject(id, name);

	Shader* shader = ResourceManager::GetInstance().GetShader(shaderId);
	if (!shader) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using shaderId with value: " << shaderId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	sceneObject->SetShader(shader);

	Mesh* mesh = ResourceManager::GetInstance().GetMesh(meshId);
	sceneObject->AddMesh(mesh);

	Material* material = ResourceManager::GetInstance().GetMaterial("default");
	sceneObject->AddMaterial(material);

	sceneObject->SetPosition(position);
	sceneObject->SetRotation(rotation);
	sceneObject->SetScale(scale);

	sceneObject->SetParent(parent);

	sceneObject->GenerateBoundingSphere();

	// If it is a root object: add to main vector
	if (parent == nullptr) {
		//Add to scenemanager vector (the one that manages the lifetime)
		SceneManager::GetInstance().AddSceneObject(sceneObject);
		//Local vector that is used just to iterate over all spheres objects
		_sphereObjects[id] = sceneObject;
	}

	return sceneObject;
}

BoxObject* PhysicsController::CreateAndAddBoxObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, SceneObject* parent) {
	unsigned int id = SceneManager::GetInstance().GetSceneObjectID();
	BoxObject* sceneObject = new BoxObject(id, name);

	Shader* shader = ResourceManager::GetInstance().GetShader(shaderId);
	if (!shader) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using shaderId with value: " << shaderId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	sceneObject->SetShader(shader);

	Mesh* mesh = ResourceManager::GetInstance().GetMesh(meshId);
	sceneObject->AddMesh(mesh);

	Material* material = ResourceManager::GetInstance().GetMaterial("default");
	sceneObject->AddMaterial(material);

	sceneObject->SetPosition(position);
	sceneObject->SetRotation(rotation);
	sceneObject->SetScale(scale);

	sceneObject->SetParent(parent);

	sceneObject->GenerateBoundingVolume();

	// If it is a root object: add to main vector
	if (parent == nullptr) {
		//Add to scenemanager vector (the one that manages the lifetime)
		SceneManager::GetInstance().AddSceneObject(sceneObject);
		//Local vector that is used just to iterate over all spheres objects
		_boxObjects[id] = sceneObject;
	}

	return sceneObject;
}

ObbObject* PhysicsController::CreateAndAddObbObject(std::string name, std::string meshId, std::string shaderId, rml::Vector3 position, rml::Vector3 rotation, rml::Vector3 scale, SceneObject* parent) {


	unsigned int id = SceneManager::GetInstance().GetSceneObjectID();
	ObbObject* sceneObject = new ObbObject(id, name);

	Shader* shader = ResourceManager::GetInstance().GetShader(shaderId);
	if (!shader) {
		std::cout << "[SCENEMANAGER][ERROR]: Can;t create scene object using shaderId with value: " << shaderId << ". THis doesn't exist in ResourceManager.\n";
		return nullptr;
	}
	sceneObject->SetShader(shader);

	Mesh* mesh = ResourceManager::GetInstance().GetMesh(meshId);
	sceneObject->AddMesh(mesh);

	Material* material = ResourceManager::GetInstance().GetMaterial("default");
	sceneObject->AddMaterial(material);

	sceneObject->SetPosition(position);
	sceneObject->SetRotation(rotation);
	sceneObject->SetScale(scale);

	sceneObject->SetParent(parent);

	sceneObject->GenerateObbVolume();

	//// If it is a root object: add to main vector
	if (parent == nullptr) {
		//Add to scenemanager vector (the one that manages the lifetime)
		SceneManager::GetInstance().AddSceneObject(sceneObject);
		//Local vector that is used just to iterate over all spheres objects
		_obbObjects[id] = sceneObject;
	}

	return sceneObject;
}

rml::Vector3 PhysicsController::GetRandomPositionInsideBox() {
	return rml::Vector3(
		Utils::RandomFloat(_boxMinBound.x + 1, _boxMaxBound.z - 1),
		Utils::RandomFloat((_boxMinBound.y + _boxMaxBound.y) / 2, _boxMaxBound.y - 2),
		Utils::RandomFloat(_boxMinBound.x + 1, _boxMaxBound.z - 1)
	);
}

void PhysicsController::Update(float deltaTime) {

	//std::unordered_map<unsigned int, SphereObject*>& objects = _sphereObjects;
	//std::unordered_map<unsigned int, BoxObject*>& objects = _boxObjects;
	//std::unordered_map<unsigned int, ObbObject*>& objects = _obbObjects;

	UpdateGpu(deltaTime);
	return;

	//BOXES
	for (auto it = _boxObjects.begin(); it != _boxObjects.end(); ++it) {
		auto& object = it->second;

		ApplyGravity(object, deltaTime);
		//ApplyFriction(object, 0.005f);
		UpdatePosition(object, deltaTime);
		HandleWallCollision(object);
	}
	//Collisions between boxes
	for (auto it1 = _boxObjects.begin(); it1 != _boxObjects.end(); ++it1) {
		auto& object1 = it1->second;

		for (auto it2 = std::next(it1); it2 != _boxObjects.end(); ++it2) {
			auto& object2 = it2->second;

			HandleObjectsCollision(object1, object2);
		}
	}

	//SPHERES
	for (auto it = _sphereObjects.begin(); it != _sphereObjects.end(); ++it) {
		auto& object = it->second;

		ApplyGravity(object, deltaTime);
		ApplyFriction(object, 0.005f);
		UpdatePosition(object, deltaTime);
		HandleWallCollision(object);
	}

	
	_sphereCpuCollisions.clear();
	DetectSphereCollisions(_sphereCpuCollisions);
	ResolveSphereCollisions(_sphereCpuCollisions);

	//Collisions between spheres
	//for (auto it1 = _sphereObjects.begin(); it1 != _sphereObjects.end(); ++it1) {
	//	auto& object1 = it1->second;

	//	for (auto it2 = std::next(it1); it2 != _sphereObjects.end(); ++it2) {
	//		auto& object2 = it2->second;

	//		HandleObjectsCollision(object1, object2);
	//	}
	//}

	//Collisions between spheres and boxes
	for (auto it1 = _sphereObjects.begin(); it1 != _sphereObjects.end(); ++it1) {
		auto& object1 = it1->second;

		for (auto it2 = _boxObjects.begin(); it2 != _boxObjects.end(); ++it2) {
			auto& object2 = it2->second;

			HandleObjectsCollision(object2, object1);
		}
	}

	//--

}

void PhysicsController::UpdateGpu(float deltaTime) {


	//BOXES
	AllocCollisionArraysBox();

	if (_boxObjects.size() > 0) {


		GenerateBoxesArrays();

		//GPU_ApplyGravity(_boxes.Velocity, _boxObjects.size(), _gravity, deltaTime);
		//GPU_ApplyFriction(_boxes.Velocity, _boxObjects.size(), 0.005f, deltaTime);
		GPU_UpdateApplyingGravityAndFriction(_boxes.Position, _boxes.Velocity, _boxObjects.size(), _gravity, 0.005f, deltaTime);
		GPU_HandleBoxWallCollisions(_boxes.Position, _boxes.Velocity, _boxes.MinAABB, _boxes.MaxAABB, _boxMinBound, _boxMaxBound, _boxObjects.size());
		//GPU_HandleBoxCollisions(_boxes.Position, _boxes.Velocity, _boxes.MinAABB, _boxes.MaxAABB, _boxObjects.size(), _collisionRestitution);
		GPU_DetectBoxCollisions(_boxes.Position, _boxes.MinAABB, _boxes.MaxAABB, _boxObjects.size(), _generatedPointsCountBox, _collisionPointsBox, _collisionPointsMaxCountBox);

		PostGpuBoxes();

		ResolveBoxCollisionsGeneratedByGpu(_collisionPointsBox, _generatedPointsCountBox);
	}


	// SPHERES
	AllocCollisionArraysSphere();

	if (_sphereObjects.size() > 0) {

		GenerateSphereArrays();
		GPU_UpdateApplyingGravityAndFriction(_spheres.Position, _spheres.Velocity, _sphereObjects.size(), _gravity, 0.005f, deltaTime);
		GPU_HandleSphereWallCollisions(_spheres.Position, _spheres.Velocity, _spheres.Radius, _boxMinBound, _boxMaxBound, _sphereObjects.size());
		GPU_DetectSphereCollisions(_spheres.Position, _spheres.Radius, _sphereObjects.size(), _generatedPointsCountSphere, _collisionPointsSphere, _collisionPointsMaxCountSphere);

		PostGpuSpheres();

		ResolveSphereCollisionsGeneratedByGpu(_collisionPointsSphere, _generatedPointsCountSphere);

		//Collisions between spheres
		/*for (auto it1 = _sphereObjects.begin(); it1 != _sphereObjects.end(); ++it1) {
			auto& object1 = it1->second;

			for (auto it2 = std::next(it1); it2 != _sphereObjects.end(); ++it2) {
				auto& object2 = it2->second;

				HandleObjectsCollision(object1, object2);
			}
		}*/
	}
}

void PhysicsController::UpdatePosition(SphereObject* obj, float dt) {
	obj->SetPosition(obj->GetPosition() + obj->Velocity * dt);
}

void PhysicsController::UpdatePosition(BoxObject* obj, float dt) {
	obj->SetPosition(obj->GetPosition() + obj->Velocity * dt);
}

void PhysicsController::UpdatePosition(ObbObject* obj, float dt) {
	obj->SetPosition(obj->GetPosition() + obj->Velocity * dt);
}

void PhysicsController::ApplyGravity(SphereObject* obj, float dt) {
	obj->Velocity.y -= _gravity * dt;
}

void PhysicsController::ApplyGravity(BoxObject* obj, float dt) {
	obj->Velocity.y -= _gravity * dt;
}

void PhysicsController::ApplyGravity(ObbObject* obj, float dt) {
	obj->Velocity.y -= _gravity * dt;
}

void PhysicsController::ApplyFriction(BoxObject* obj, float dragCoefficient) {
	// Calculate the magnitude of the velocity
	float speed = obj->Velocity.Length();

	if (speed > 0.0f) { // Only apply drag if the object is moving
		// Normalize the velocity vector to get the direction
		rml::Vector3 velocityDirection = obj->Velocity;
		velocityDirection = velocityDirection.Normalize();

		// Calculate the drag force (opposing direction of motion)
		rml::Vector3 dragForce = -velocityDirection * dragCoefficient * speed;

		// Apply the drag force to the object's velocity
		obj->Velocity += dragForce;
	}
}

void PhysicsController::ApplyFriction(SphereObject* obj, float dragCoefficient) {
	// Calculate the magnitude of the velocity
	float speed = obj->Velocity.Length();

	if (speed > 0.0f) { // Only apply drag if the object is moving
		// Normalize the velocity vector to get the direction
		rml::Vector3 velocityDirection = obj->Velocity;
		velocityDirection = velocityDirection.Normalize();

		// Calculate the drag force (opposing direction of motion)
		rml::Vector3 dragForce = -velocityDirection * dragCoefficient * speed;

		// Apply the drag force to the object's velocity
		obj->Velocity += dragForce;
	}
}

void PhysicsController::ApplyFriction(ObbObject* obj, float dragCoefficient) {
	// Calculate the magnitude of the velocity
	float speed = obj->Velocity.Length();

	if (speed > 0.0f) { // Only apply drag if the object is moving
		// Normalize the velocity vector to get the direction
		rml::Vector3 velocityDirection = obj->Velocity;
		velocityDirection = velocityDirection.Normalize();

		// Calculate the drag force (opposing direction of motion)
		rml::Vector3 dragForce = -velocityDirection * dragCoefficient * speed;

		// Apply the drag force to the object's velocity
		obj->Velocity += dragForce;
	}
}

void PhysicsController::HandleWallCollision(SphereObject* obj) {

	rml::Vector3 center = obj->GetPosition() + obj->_boundingSphere->center;
	float radius = obj->GetBoundingSphere()->radius;
	if (center.x - radius < _boxMinBound.x) {
		obj->_position.x = _boxMinBound.x + radius;
		obj->Velocity.x *= -_wallRestitution;
	}
	else if (center.x + radius > _boxMaxBound.x) {
		obj->_position.x = _boxMaxBound.x - radius;
		obj->Velocity.x *= -_wallRestitution;
	}
	if (center.y - radius < _boxMinBound.y) {
		obj->_position.y = _boxMinBound.y + radius;
		obj->Velocity.y *= -_wallRestitution;
	}
	else if (center.y + radius > _boxMaxBound.y) {
		obj->_position.y = _boxMaxBound.y - radius;
		obj->Velocity.y *= -_wallRestitution;
	}
	if (center.z - radius < _boxMinBound.z) {
		obj->_position.z = _boxMinBound.z + radius;
		obj->Velocity.z *= -_wallRestitution;
	}
	else if (center.z + radius > _boxMaxBound.z) {
		obj->_position.z = _boxMaxBound.z - radius;
		obj->Velocity.z *= -_wallRestitution;
	}

}

void PhysicsController::HandleWallCollision(BoxObject* obj) {
	rml::Vector3 minBound = obj->GetBoundingVolume()->Min + obj->GetPosition();
	rml::Vector3 maxBound = obj->GetBoundingVolume()->Max + obj->GetPosition();

	if (minBound.x < _boxMinBound.x) {
		obj->_position.x = _boxMinBound.x + (obj->_position.x - minBound.x);  // Adjust position
		obj->Velocity.x *= -_wallRestitution;  // Reverse velocity for bounce
	}
	else if (maxBound.x > _boxMaxBound.x) {
		obj->_position.x = _boxMaxBound.x - (maxBound.x - obj->_position.x);
		obj->Velocity.x *= -_wallRestitution;
	}

	if (minBound.y < _boxMinBound.y) {
		obj->_position.y = _boxMinBound.y + (obj->_position.y - minBound.y);
		obj->Velocity.y *= -_wallRestitution;
	}
	else if (maxBound.y > _boxMaxBound.y) {
		obj->_position.y = _boxMaxBound.y - (maxBound.y - obj->_position.y);
		obj->Velocity.y *= -_wallRestitution;
	}

	if (minBound.z < _boxMinBound.z) {
		obj->_position.z = _boxMinBound.z + (obj->_position.z - minBound.z);
		obj->Velocity.z *= -_wallRestitution;
	}
	else if (maxBound.z > _boxMaxBound.z) {
		obj->_position.z = _boxMaxBound.z - (maxBound.z - obj->_position.z);
		obj->Velocity.z *= -_wallRestitution;
	}
}

void PhysicsController::HandleWallCollision(ObbObject* obj) {
	// Get the OBB
	static const float penetrationThreshold = 0.01f; // Small threshold to prevent overcorrection
	bool isColliding = false; // Track if there’s any collision this frame

	// Get the OBB
	OBB* obb = obj->_obbVolume;

	// Calculate all 8 corners of the OBB
	std::vector<rml::Vector3> corners(8);
	rml::Vector3 center = obb->center + obj->GetPosition();
	corners[0] = center + obb->axes[0] * obb->halfExtents.x + obb->axes[1] * obb->halfExtents.y + obb->axes[2] * obb->halfExtents.z;
	corners[1] = center - obb->axes[0] * obb->halfExtents.x + obb->axes[1] * obb->halfExtents.y + obb->axes[2] * obb->halfExtents.z;
	corners[2] = center + obb->axes[0] * obb->halfExtents.x - obb->axes[1] * obb->halfExtents.y + obb->axes[2] * obb->halfExtents.z;
	corners[3] = center - obb->axes[0] * obb->halfExtents.x - obb->axes[1] * obb->halfExtents.y + obb->axes[2] * obb->halfExtents.z;
	corners[4] = center + obb->axes[0] * obb->halfExtents.x + obb->axes[1] * obb->halfExtents.y - obb->axes[2] * obb->halfExtents.z;
	corners[5] = center - obb->axes[0] * obb->halfExtents.x + obb->axes[1] * obb->halfExtents.y - obb->axes[2] * obb->halfExtents.z;
	corners[6] = center + obb->axes[0] * obb->halfExtents.x - obb->axes[1] * obb->halfExtents.y - obb->axes[2] * obb->halfExtents.z;
	corners[7] = center - obb->axes[0] * obb->halfExtents.x - obb->axes[1] * obb->halfExtents.y - obb->axes[2] * obb->halfExtents.z;

	for (const auto& corner : corners) {
		// X-axis collision detection
		if (corner.x < _boxMinBound.x) {
			float penetration = _boxMinBound.x - corner.x;
			if (penetration > penetrationThreshold) {
				if (!isColliding) {
					obj->Velocity.x *= -_wallRestitution; // Reverse velocity only once per collision
				}
				obj->_position.x += penetration;  // Small correction
				isColliding = true;
			}
		}
		else if (corner.x > _boxMaxBound.x) {
			float penetration = corner.x - _boxMaxBound.x;
			if (penetration > penetrationThreshold) {
				if (!isColliding) {
					obj->Velocity.x *= -_wallRestitution;
				}
				obj->_position.x -= penetration;
				isColliding = true;
			}
		}

		// Y-axis collision detection
		if (corner.y < _boxMinBound.y) {
			float penetration = _boxMinBound.y - corner.y;
			if (penetration > penetrationThreshold) {
				if (!isColliding) {
					obj->Velocity.y *= -_wallRestitution;
				}
				obj->_position.y += penetration;
				isColliding = true;
			}
		}
		else if (corner.y > _boxMaxBound.y) {
			float penetration = corner.y - _boxMaxBound.y;
			if (penetration > penetrationThreshold) {
				if (!isColliding) {
					obj->Velocity.y *= -_wallRestitution;
				}
				obj->_position.y -= penetration;
				isColliding = true;
			}
		}

		// Z-axis collision detection
		if (corner.z < _boxMinBound.z) {
			float penetration = _boxMinBound.z - corner.z;
			if (penetration > penetrationThreshold) {
				if (!isColliding) {
					obj->Velocity.z *= -_wallRestitution;
				}
				obj->_position.z += penetration;
				isColliding = true;
			}
		}
		else if (corner.z > _boxMaxBound.z) {
			float penetration = corner.z - _boxMaxBound.z;
			if (penetration > penetrationThreshold) {
				if (!isColliding) {
					obj->Velocity.z *= -_wallRestitution;
				}
				obj->_position.z -= penetration;
				isColliding = true;
			}
		}
	}
}

void PhysicsController::HandleObjectsCollision(SphereObject* obj1, SphereObject* obj2) {
	rml::Vector3 center1 = obj1->GetPosition();
	rml::Vector3 center2 = obj2->GetPosition();

	rml::Vector3 distanceVec = center2 - center1;
	double distance = distanceVec.Length();
	double collisionDistance = obj1->GetBoundingSphere()->radius + obj2->GetBoundingSphere()->radius;

	if (distance - collisionDistance < 0.01f) {
		rml::Vector3 normal = distanceVec.Normalize();
		rml::Vector3 relativeVelocity = obj2->Velocity - obj1->Velocity;
		double relVelAlongNormal = relativeVelocity.x * normal.x +
			relativeVelocity.y * normal.y +
			relativeVelocity.z * normal.z;

		if (relVelAlongNormal < 0) {
			/*	obj1->Velocity += normal * relVelAlongNormal;
				obj2->Velocity -= normal * relVelAlongNormal;*/

			obj1->Velocity *= -_collisionRestitution;
			obj2->Velocity *= -_collisionRestitution;

			//obj1->Velocity.x *= _collisionRestitution;
			//obj2->Velocity.x *= _collisionRestitution;

			double penetrationDepth = collisionDistance - distance;

			// Push the spheres apart based on their masses
			//double totalMass = obj1->Mass + obj2->Mass;
			rml::Vector3 correction = normal * (penetrationDepth / 1);

			// Move each object out of the collision
			obj1->_position -= correction / 2;
			obj2->_position += correction / 2;
		}
		else {
			double penetrationDepth = collisionDistance - distance;

			// Push the spheres apart based on their masses
			//double totalMass = obj1->Mass + obj2->Mass;
			rml::Vector3 correction = normal * (penetrationDepth / 1);

			// Move each object out of the collision
			obj1->_position -= correction / 2;
			obj2->_position += correction / 2;
		}
	}
}

void PhysicsController::HandleObjectsCollision(BoxObject* obj1, BoxObject* obj2) {
	rml::Vector3 minBound1 = obj1->GetBoundingVolume()->Min + obj1->GetPosition();
	rml::Vector3 maxBound1 = obj1->GetBoundingVolume()->Max + obj1->GetPosition();
	rml::Vector3 minBound2 = obj2->GetBoundingVolume()->Min + obj2->GetPosition();
	rml::Vector3 maxBound2 = obj2->GetBoundingVolume()->Max + obj2->GetPosition();

	// Check for overlap between the bounding volumes (AABBs)
	if (CheckOverlap(minBound1, maxBound1, minBound2, maxBound2)) {
		// Determine the penetration depth along each axis
		rml::Vector3 penetration;

		penetration.x = std::min(maxBound1.x - minBound2.x, maxBound2.x - minBound1.x);
		penetration.y = std::min(maxBound1.y - minBound2.y, maxBound2.y - minBound1.y);
		penetration.z = std::min(maxBound1.z - minBound2.z, maxBound2.z - minBound1.z);

		// Threshold to avoid jitter on small penetrations
		const float penetrationThreshold = 0.01f;
		if (penetration.x < penetrationThreshold && penetration.y < penetrationThreshold && penetration.z < penetrationThreshold) {
			return;
		}

		// Find the axis of least penetration to separate objects along that axis
		if (penetration.x < penetration.y && penetration.x < penetration.z) {
			if (obj1->_position.x < obj2->_position.x) {
				obj1->_position.x -= penetration.x / 2;
				obj2->_position.x += penetration.x / 2;
			}
			else {
				obj1->_position.x += penetration.x / 2;
				obj2->_position.x -= penetration.x / 2;
			}
			obj1->Velocity.x *= -_collisionRestitution;
			obj2->Velocity.x *= -_collisionRestitution;
		}
		else if (penetration.y < penetration.z) {
			if (obj1->_position.y < obj2->_position.y) {
				obj1->_position.y -= penetration.y / 2;
				obj2->_position.y += penetration.y / 2;
			}
			else {
				obj1->_position.y += penetration.y / 2;
				obj2->_position.y -= penetration.y / 2;
			}
			obj1->Velocity.y *= -_collisionRestitution;
			obj2->Velocity.y *= -_collisionRestitution;
		}
		else {
			if (obj1->_position.z < obj2->_position.z) {
				obj1->_position.z -= penetration.z / 2;
				obj2->_position.z += penetration.z / 2;
			}
			else {
				obj1->_position.z += penetration.z / 2;
				obj2->_position.z -= penetration.z / 2;
			}
			obj1->Velocity.z *= -_collisionRestitution;
			obj2->Velocity.z *= -_collisionRestitution;
		}
	}
}

void PhysicsController::HandleObjectsCollision(ObbObject* obj1, ObbObject* obj2) {
	float minPenetration = std::numeric_limits<float>::max();
	rml::Vector3 minPenetrationAxis;

	OBB obb1 = *(obj1->_obbVolume);
	OBB obb2 = *(obj2->_obbVolume);

	// List of all possible separating axes (3 + 3 + 9 = 15)
	rml::Vector3 axes[15] = {
		obb1.axes[0], obb1.axes[1], obb1.axes[2],
		obb2.axes[0], obb2.axes[1], obb2.axes[2],
		obb1.axes[0].Cross(obb2.axes[0]), obb1.axes[0].Cross(obb2.axes[1]), obb1.axes[0].Cross(obb2.axes[2]),
		obb1.axes[1].Cross(obb2.axes[0]), obb1.axes[1].Cross(obb2.axes[1]), obb1.axes[1].Cross(obb2.axes[2]),
		obb1.axes[2].Cross(obb2.axes[0]), obb1.axes[2].Cross(obb2.axes[1]), obb1.axes[2].Cross(obb2.axes[2])
	};

	// Test each axis for separation
	for (auto& axis : axes) {
		if (axis.Dot(axis) < 1e-6) continue; // Skip near-zero axes

		if (!TestAxis(obj1, obj2, axis, minPenetration, minPenetrationAxis)) {
			// Found a separating axis, no collision
			return;
		}
	}

	// If we reach here, no separating axis was found, so there is a collision
	// Resolve collision by moving obj1 and obj2 along the axis of least penetration
	rml::Vector3 distanceVec = (obb2.center + obj2->GetPosition()) - (obb1.center + obj1->GetPosition());
	rml::Vector3 normal = distanceVec.Normalize();
	rml::Vector3 relativeVelocity = obj2->Velocity - obj1->Velocity;
	double relVelAlongNormal = relativeVelocity.x * normal.x +
		relativeVelocity.y * normal.y +
		relativeVelocity.z * normal.z;

	if (relVelAlongNormal < 0) {
		/*	obj1->Velocity += normal * relVelAlongNormal;
			obj2->Velocity -= normal * relVelAlongNormal;*/

			//obj1->Velocity -= minPenetrationAxis.Normalize() * _collisionRestitution ;
			//obj2->Velocity += -minPenetrationAxis.Normalize() * _collisionRestitution;

			/*obj1->Velocity = -minPenetrationAxis.Normalize();
			obj2->Velocity = minPenetrationAxis.Normalize();*/

		obj1->Velocity *= -_collisionRestitution;
		obj2->Velocity *= -_collisionRestitution;
	}

	// Calculate the correction vector based on minimum penetration depth and axis
	rml::Vector3 correction = minPenetrationAxis * minPenetration;

	obj1->_position -= correction / 2;
	obj2->_position += correction / 2;
}

void PhysicsController::HandleObjectsCollision(BoxObject* box, SphereObject* sphere) {

	rml::Vector3 boxMinBound = box->GetBoundingVolume()->Min + box->GetPosition();
	rml::Vector3 boxMaxBound = box->GetBoundingVolume()->Max + box->GetPosition();

	rml::Vector3 sphereCenter = sphere->GetBoundingSphere()->center + sphere->GetPosition();

	rml::Vector3 closestPoint;
	closestPoint.x = std::max(boxMinBound.x, std::min(sphereCenter.x, boxMaxBound.x));
	closestPoint.y = std::max(boxMinBound.y, std::min(sphereCenter.y, boxMaxBound.y));
	closestPoint.z = std::max(boxMinBound.z, std::min(sphereCenter.z, boxMaxBound.z));

	rml::Vector3 distanceVec = sphereCenter - closestPoint;
	double distance = distanceVec.Length();
	double collisionDistance = sphere->GetBoundingSphere()->radius;

	if (distance - collisionDistance < 0.01f) {

		rml::Vector3 normal = distanceVec.Normalize();
		rml::Vector3 relativeVelocity = sphere->Velocity - box->Velocity;
		double relVelAlongNormal = relativeVelocity.x * normal.x +
			relativeVelocity.y * normal.y +
			relativeVelocity.z * normal.z;

		if (relVelAlongNormal < 0) {
			/*	obj1->Velocity += normal * relVelAlongNormal;
				obj2->Velocity -= normal * relVelAlongNormal;*/

			box->Velocity *= -_collisionRestitution;
			sphere->Velocity *= -_collisionRestitution;

			//obj1->Velocity.x *= _collisionRestitution;
			//obj2->Velocity.x *= _collisionRestitution;

			double penetrationDepth = collisionDistance - distance;

			// Push the spheres apart based on their masses
			//double totalMass = obj1->Mass + obj2->Mass;
			rml::Vector3 correction = normal * (penetrationDepth / 1);

			// Move each object out of the collision
			box->_position -= correction / 2;
			sphere->_position += correction / 2;
		}
		else {
			double penetrationDepth = collisionDistance - distance;

			// Push the spheres apart based on their masses
			//double totalMass = obj1->Mass + obj2->Mass;
			rml::Vector3 correction = normal * (penetrationDepth / 1);

			// Move each object out of the collision
			box->_position -= correction / 2;
			sphere->_position += correction / 2;
		}
	}
}

float PhysicsController::ProjectOBB(OBB& obb, rml::Vector3& axis) {

	return std::abs(obb.halfExtents.x * obb.axes[0].Dot(axis)) +
		std::abs(obb.halfExtents.y * obb.axes[1].Dot(axis)) +
		std::abs(obb.halfExtents.z * obb.axes[2].Dot(axis));
}

bool PhysicsController::TestAxis(ObbObject* obj1, ObbObject* obj2, rml::Vector3& axis, float& minPenetration, rml::Vector3& minPenetrationAxis) {

	OBB obb1 = *(obj1->_obbVolume);
	OBB obb2 = *(obj2->_obbVolume);

	float proj1 = ProjectOBB(obb1, axis);
	float proj2 = ProjectOBB(obb2, axis);

	// Calculate the distance between centers projected onto the axis
	rml::Vector3 diff = (obb2.center + obj2->GetPosition()) - (obb1.center + obj1->GetPosition());
	float distance = std::abs(diff.Dot(axis));

	// Calculate overlap (penetration depth)
	float overlap = proj1 + proj2 - distance;
	if (overlap < 0) {
		// No collision on this axis
		return false;
	}

	// Update minimum penetration depth and axis if this axis has the smallest overlap
	if (overlap < minPenetration) {
		minPenetration = overlap;
		minPenetrationAxis = axis;
	}
	return true;
}

bool PhysicsController::CheckOverlap(const rml::Vector3& min1, const rml::Vector3& max1, const rml::Vector3& min2, const rml::Vector3& max2) {
	return (min1.x <= max2.x && max1.x >= min2.x) &&
		(min1.y <= max2.y && max1.y >= min2.y) &&
		(min1.z <= max2.z && max1.z >= min2.z);
}



void PhysicsController::AllocCollisionArraysBox() {

	if (_boxObjects.size() != _oldBoxesCount) {

		if (_generatedPointsCountBox) {
			delete _generatedPointsCountBox;
			_generatedPointsCountBox = nullptr;
		}
		if (_collisionPointsBox) {
			delete _collisionPointsBox;
			_collisionPointsBox = nullptr;
		}

		_collisionPointsMaxCountBox = _boxObjects.size() * _boxObjects.size();
		_generatedPointsCountBox = new int[_boxObjects.size()];
		_collisionPointsBox = new CollisionPoint[_collisionPointsMaxCountBox];

		_oldBoxesCount = _boxObjects.size();
	}
}

void PhysicsController::AllocCollisionArraysSphere() {
	if (_sphereObjects.size() != _oldSpheresCount) {

		if (_generatedPointsCountSphere) {
			delete _generatedPointsCountSphere;
			_generatedPointsCountSphere = nullptr;
		}
		if (_collisionPointsSphere) {
			delete _collisionPointsSphere;
			_collisionPointsSphere = nullptr;
		}

		_collisionPointsMaxCountSphere = _sphereObjects.size() * _sphereObjects.size();
		_generatedPointsCountSphere = new int[_sphereObjects.size()];
		_collisionPointsSphere = new CollisionPoint[_collisionPointsMaxCountSphere];

		_oldSpheresCount = _sphereObjects.size();
	}
}

void PhysicsController::GenerateBoxesArrays() {

	int i = 0;
	for (auto it = _boxObjects.begin(); it != _boxObjects.end(); ++it) {
		auto& object = it->second;

		_boxes.Position[i] = object->GetPosition();
		_boxes.IDs[i] = it->first;
		_boxes.Velocity[i] = object->Velocity;
		_boxes.MinAABB[i] = object->GetAABB()->Min;
		_boxes.MaxAABB[i] = object->GetAABB()->Max;
		i++;
	}
}
void PhysicsController::GenerateSphereArrays() {
	int i = 0;
	for (auto it = _sphereObjects.begin(); it != _sphereObjects.end(); ++it) {
		auto& object = it->second;
		_spheres.Position[i] = object->GetPosition();
		_spheres.IDs[i] = it->first;
		_spheres.Velocity[i] = object->Velocity;
		_spheres.Radius[i] = object->GetBoundingSphere()->radius;
		i++;
	}
}


void PhysicsController::PostGpuBoxes() {
	int i = 0;
	for (auto it = _boxObjects.begin(); it != _boxObjects.end(); ++it) {

		it->second->SetPosition(_boxes.Position[i]);
		it->second->Velocity = _boxes.Velocity[i];
		i++;
	}
}

void PhysicsController::PostGpuSpheres() {
	int i = 0;
	for (auto it = _sphereObjects.begin(); it != _sphereObjects.end(); ++it) {

		it->second->SetPosition(_spheres.Position[i]);
		it->second->Velocity = _spheres.Velocity[i];
		i++;
	}
}

void PhysicsController::DetectBoxCollisions(std::vector<CollisionPoint>& collisionPoints) {

	collisionPoints.clear();

	for (auto it1 = _boxObjects.begin(); it1 != _boxObjects.end(); ++it1) {
		auto& object1 = it1->second;

		for (auto it2 = std::next(it1); it2 != _boxObjects.end(); ++it2) {
			auto& object2 = it2->second;

			rml::Vector3 penetration = CalculateBoxesCollision(object1, object2);
			if (penetration.x != 0 || penetration.y != 0 || penetration.z != 0) {

				//store
				CollisionPoint cp;
				cp.Object1 = it1->first;
				cp.Object2 = it2->first;
				cp.Penetration = penetration;

				collisionPoints.push_back(cp);
			}
		}
	}
}

rml::Vector3 PhysicsController::CalculateBoxesCollision(BoxObject* obj1, BoxObject* obj2) {

	rml::Vector3 zero(0, 0, 0);

	rml::Vector3 minBound1 = obj1->GetBoundingVolume()->Min + obj1->GetPosition();
	rml::Vector3 maxBound1 = obj1->GetBoundingVolume()->Max + obj1->GetPosition();
	rml::Vector3 minBound2 = obj2->GetBoundingVolume()->Min + obj2->GetPosition();
	rml::Vector3 maxBound2 = obj2->GetBoundingVolume()->Max + obj2->GetPosition();

	// Check for overlap between the bounding volumes (AABBs)
	if (CheckOverlap(minBound1, maxBound1, minBound2, maxBound2)) {
		// Determine the penetration depth along each axis
		rml::Vector3 penetration;

		penetration.x = std::min(maxBound1.x - minBound2.x, maxBound2.x - minBound1.x);
		penetration.y = std::min(maxBound1.y - minBound2.y, maxBound2.y - minBound1.y);
		penetration.z = std::min(maxBound1.z - minBound2.z, maxBound2.z - minBound1.z);

		// Threshold to avoid jitter on small penetrations
		const float penetrationThreshold = 0.01f;
		if (penetration.x < penetrationThreshold && penetration.y < penetrationThreshold && penetration.z < penetrationThreshold) {
			return zero;
		}

		return penetration;
	}

	return zero;
}

void PhysicsController::ResolveBoxCollisions(std::vector<CollisionPoint>& collisionPoints) {

	for (int i = 0; i < collisionPoints.size(); i++) {

		rml::Vector3 penetration = collisionPoints[i].Penetration;
		BoxObject* obj1 = _boxObjects[collisionPoints[i].Object1];
		BoxObject* obj2 = _boxObjects[collisionPoints[i].Object2];

		// Find the axis of least penetration to separate objects along that axis
		if (penetration.x < penetration.y && penetration.x < penetration.z) {
			if (obj1->_position.x < obj2->_position.x) {
				obj1->_position.x -= penetration.x / 2;
				obj2->_position.x += penetration.x / 2;
			}
			else {
				obj1->_position.x += penetration.x / 2;
				obj2->_position.x -= penetration.x / 2;
			}
			obj1->Velocity.x *= -_collisionRestitution;
			obj2->Velocity.x *= -_collisionRestitution;
		}
		else if (penetration.y < penetration.z) {
			if (obj1->_position.y < obj2->_position.y) {
				obj1->_position.y -= penetration.y / 2;
				obj2->_position.y += penetration.y / 2;
			}
			else {
				obj1->_position.y += penetration.y / 2;
				obj2->_position.y -= penetration.y / 2;
			}
			obj1->Velocity.y *= -_collisionRestitution;
			obj2->Velocity.y *= -_collisionRestitution;
		}
		else {
			if (obj1->_position.z < obj2->_position.z) {
				obj1->_position.z -= penetration.z / 2;
				obj2->_position.z += penetration.z / 2;
			}
			else {
				obj1->_position.z += penetration.z / 2;
				obj2->_position.z -= penetration.z / 2;
			}
			obj1->Velocity.z *= -_collisionRestitution;
			obj2->Velocity.z *= -_collisionRestitution;
		}
	}


}

void PhysicsController::ResolveBoxCollisionsGeneratedByGpu(CollisionPoint* collisionPoints, int* collisionPointsCount) {

	for (int i = 0; i < _boxObjects.size(); i++) {

		int start = i * _boxObjects.size();
		for (int j = 0; j < collisionPointsCount[i]; j++) {



			rml::Vector3 penetration = collisionPoints[start + j].Penetration;
			BoxObject* obj1 = _boxObjects[_boxes.IDs[collisionPoints[start + j].Object1]];
			BoxObject* obj2 = _boxObjects[_boxes.IDs[collisionPoints[start + j].Object2]];

			// Find the axis of least penetration to separate objects along that axis
			if (penetration.x < penetration.y && penetration.x < penetration.z) {
				if (obj1->_position.x < obj2->_position.x) {
					obj1->_position.x -= penetration.x / 2;
					obj2->_position.x += penetration.x / 2;
				}
				else {
					obj1->_position.x += penetration.x / 2;
					obj2->_position.x -= penetration.x / 2;
				}
				obj1->Velocity.x *= -_collisionRestitution;
				obj2->Velocity.x *= -_collisionRestitution;
			}
			else if (penetration.y < penetration.z) {
				if (obj1->_position.y < obj2->_position.y) {
					obj1->_position.y -= penetration.y / 2;
					obj2->_position.y += penetration.y / 2;
				}
				else {
					obj1->_position.y += penetration.y / 2;
					obj2->_position.y -= penetration.y / 2;
				}
				obj1->Velocity.y *= -_collisionRestitution;
				obj2->Velocity.y *= -_collisionRestitution;
			}
			else {
				if (obj1->_position.z < obj2->_position.z) {
					obj1->_position.z -= penetration.z / 2;
					obj2->_position.z += penetration.z / 2;
				}
				else {
					obj1->_position.z += penetration.z / 2;
					obj2->_position.z -= penetration.z / 2;
				}
				obj1->Velocity.z *= -_collisionRestitution;
				obj2->Velocity.z *= -_collisionRestitution;
			}
		}
	}
}

void PhysicsController::DetectSphereCollisions(std::vector<CollisionPoint>& collisionPoints) {
	for (auto it1 = _sphereObjects.begin(); it1 != _sphereObjects.end(); ++it1) {
		auto& obj1 = it1->second;

		for (auto it2 = std::next(it1); it2 != _sphereObjects.end(); ++it2) {
			auto& obj2 = it2->second;

			rml::Vector3 center1 = obj1->GetPosition();
			rml::Vector3 center2 = obj2->GetPosition();

			rml::Vector3 distanceVec = center2 - center1;
			double distance = distanceVec.Length();
			double collisionDistance = obj1->GetBoundingSphere()->radius + obj2->GetBoundingSphere()->radius;

			if (distance - collisionDistance < 0.01f) {
				rml::Vector3 normal = distanceVec.Normalize();
				double penetrationDepth = collisionDistance - distance;
				
				CollisionPoint cp;
				cp.Object1 = it1->first;
				cp.Object2 = it2->first;
				cp.Penetration = normal * penetrationDepth;

				collisionPoints.push_back(cp);
			}
		}
	}
}

void PhysicsController::ResolveSphereCollisions(std::vector<CollisionPoint>& collisionPoints) {

	for (int i = 0; i < collisionPoints.size(); i++) {

		rml::Vector3 penetration = collisionPoints[i].Penetration;
		SphereObject* obj1 = _sphereObjects[collisionPoints[i].Object1];
		SphereObject* obj2 = _sphereObjects[collisionPoints[i].Object2];


		rml::Vector3 normal = collisionPoints[i].Penetration;
		normal = normal.Normalize();

		rml::Vector3 relativeVelocity = obj2->Velocity - obj1->Velocity;
		double relVelAlongNormal = relativeVelocity.x * normal.x +
			relativeVelocity.y * normal.y +
			relativeVelocity.z * normal.z;

		if (relVelAlongNormal < 0) {
			/*	obj1->Velocity += normal * relVelAlongNormal;
				obj2->Velocity -= normal * relVelAlongNormal;*/

			obj1->Velocity *= -_collisionRestitution;
			obj2->Velocity *= -_collisionRestitution;

			//obj1->Velocity.x *= _collisionRestitution;
			//obj2->Velocity.x *= _collisionRestitution;

			double penetrationDepth = collisionPoints[i].Penetration.Length();

			// Push the spheres apart based on their masses
			//double totalMass = obj1->Mass + obj2->Mass;
			rml::Vector3 correction = normal * (penetrationDepth / 1);

			// Move each object out of the collision
			obj1->_position -= correction / 2;
			obj2->_position += correction / 2;
		}
		else {
			double penetrationDepth = collisionPoints[i].Penetration.Length();

			// Push the spheres apart based on their masses
			//double totalMass = obj1->Mass + obj2->Mass;
			rml::Vector3 correction = normal * (penetrationDepth / 1);

			// Move each object out of the collision
			obj1->_position -= correction / 2;
			obj2->_position += correction / 2;
		}
	}
}

void PhysicsController::ResolveSphereCollisionsGeneratedByGpu(CollisionPoint* collisionPoints, int* collisionPointsCount) {

	for (int i = 0; i < _sphereObjects.size(); i++) {

		int start = i * _sphereObjects.size();
		for (int j = 0; j < collisionPointsCount[i]; j++) {

			SphereObject* obj1 = _sphereObjects[_spheres.IDs[collisionPoints[start + j].Object1]];
			SphereObject* obj2 = _sphereObjects[_spheres.IDs[collisionPoints[start + j].Object2]];

			rml::Vector3 normal = collisionPoints[start + j].Penetration;
			normal = normal.Normalize();

			rml::Vector3 relativeVelocity = obj2->Velocity - obj1->Velocity;
			double relVelAlongNormal = relativeVelocity.x * normal.x +
				relativeVelocity.y * normal.y +
				relativeVelocity.z * normal.z;

			if (relVelAlongNormal < 0) {
				/*	obj1->Velocity += normal * relVelAlongNormal;
					obj2->Velocity -= normal * relVelAlongNormal;*/

				obj1->Velocity *= -_collisionRestitution;
				obj2->Velocity *= -_collisionRestitution;

				//obj1->Velocity.x *= _collisionRestitution;
				//obj2->Velocity.x *= _collisionRestitution;

				double penetrationDepth = collisionPoints[start + j].Penetration.Length();

				// Push the spheres apart based on their masses
				//double totalMass = obj1->Mass + obj2->Mass;
				rml::Vector3 correction = normal * (penetrationDepth / 1);

				// Move each object out of the collision
				obj1->_position -= correction / 2;
				obj2->_position += correction / 2;
			}
			else {
				double penetrationDepth = collisionPoints[start + j].Penetration.Length();

				// Push the spheres apart based on their masses
				//double totalMass = obj1->Mass + obj2->Mass;
				rml::Vector3 correction = normal * (penetrationDepth / 1);

				// Move each object out of the collision
				obj1->_position -= correction / 2;
				obj2->_position += correction / 2;
			}
		}
	}
}
