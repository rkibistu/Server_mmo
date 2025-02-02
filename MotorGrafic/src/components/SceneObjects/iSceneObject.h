#pragma once

#include <utils/Math.h>

#include <vector>
#include <iostream>
#include <string>

class Mesh;
class Material;
class Shader;
class AABB;

// The objects are tretead diffrent based on their type:
// - static adn semi_static are included in Octree and rendered using culling
// - kinematic are always rendered and not included in octree; This is because
//		we don t want to slow down physics simulation by adding/removing objects from octree
//		every time they move
enum ObjectMovementType {
	STATIC = 0,		//never moves - not used yet
	SEMI_STATIC,	//can move, but rarely. Not affected by physics
	KINEMATIC		//move often (usually afected by physics, but necessarly)
};


class iSceneObject {
	friend class SceneManager;
	friend class Renderer;

public:
	virtual ~iSceneObject() { ; };

	virtual void Start() = 0;
	virtual void Update(float deltaTime) = 0;

	virtual void Draw() = 0;

	virtual void AddMesh(Mesh* mesh) = 0;
	virtual void AddMaterial(Material* material) = 0;
	// Set the material from index position in materials array. Set all of them if all si true.
	virtual void SetMaterial(Material* material, unsigned int index, bool all = false) = 0;
	virtual void SetShader(Shader* shader) = 0;
	virtual inline Shader* GetShader() = 0;

	/**
	 * Sets the parents by calling AddChild method.
	 * This is done to configure all the links between objects,
	 * not simply change the parent pointer
	 * \param parent
	 */
	virtual void SetParent(iSceneObject* parent) = 0;
	virtual iSceneObject* GetParent() = 0;

	virtual inline void SetDrawMode(int drawMode) = 0;

	/**
	 * Add a child to the sceneobject.
	 * It does all the necessary work:
	 *	- remove child from the old parent
	 *  - set aprent pointer for the child
	 *  - add child to the new parent
	 *  - remove child from manager vector if necessary (if it was a root object)
	 *
	 * \param child The child object to add
	 */
	virtual void AddChild(iSceneObject* child) = 0;

	/**
	 * Remove a child from the sceneobject.
	 * It does all the necessary work:
	 *  - set parent pointer to null
	 *  - add object to manager vector (because without a parent, the object is now a root)
	 *  - remove child from the sceneobject
	 *
	 * \param id The id of the child that should be removed
	 */
	virtual void RemoveChild(unsigned int id) = 0;

	virtual inline std::string GetName() = 0;
	virtual inline void SetName(std::string name) = 0;
	/** Position relative to the parent */
	virtual inline rml::Vector3 GetPosition() = 0;
	/** Absolut position */
	virtual rml::Vector3 GetWorldPosition() = 0;
	/** Set the position relative to the parent */
	virtual inline void SetPosition(rml::Vector3 position) = 0;
	virtual inline rml::Vector3 GetRotation() = 0;
	virtual inline void SetRotation(rml::Vector3 rotation) = 0;
	virtual inline rml::Vector3 GetScale() = 0;
	virtual inline void SetScale(rml::Vector3 scale) = 0;
	virtual inline unsigned int GetID() = 0;

	virtual ObjectMovementType GetMovementType() = 0;

	/** Get the object and all children and children of children and etc */
	virtual std::vector<iSceneObject*> GetAllObjects() = 0;

	/** Return an updated version of AABB (scaled and moved to world coords) */
	virtual AABB* GetAABB() = 0;
};
