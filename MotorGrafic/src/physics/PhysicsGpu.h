#pragma once

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <utils/Math.h>

struct CollisionPoint;

cudaError_t GPU_ApplyGravity(rml::Vector3* velocity, int size, float gravity, float deltaTime);
cudaError_t GPU_ApplyFriction(rml::Vector3* velocity, int size, float drag, float deltaTime);
cudaError_t GPU_UpdateApplyingGravityAndFriction(rml::Vector3* position, rml::Vector3* velocity, int size, float gravity, float drag, float deltaTime);
cudaError_t GPU_HandleBoxWallCollisions(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, rml::Vector3 minBoxBound, rml::Vector3 maxBoxBound, int size);
cudaError_t GPU_HandleSphereWallCollisions(rml::Vector3* position, rml::Vector3* velocity, float* radius, rml::Vector3 minBoxBound, rml::Vector3 maxBoxBound, int size);
/**
 * generatedCollisionPoints count en zice cate coliziuni a genereta fiecare obiect, 
 *	ca sa stim cat citim din vectorul de collisionPoints.
 * vectorul de collisionPoints o sa aiba dimensiunea numarului deobiecte la patrat ca sa incapa toate coliziunile
 * fiecare thread va detecta coliziuni pt 1 obiect vs restul obiectelor si va popula o zona dedicata lui din collisionPoints vector
 */
cudaError_t GPU_DetectBoxCollisions(rml::Vector3* position, rml::Vector3* minAABB, rml::Vector3* maxAABB, int size, int* generateCollisonPointsCount, CollisionPoint* collisionPoints, int cpSize);
cudaError_t GPU_DetectSphereCollisions(rml::Vector3* position, float* radius, int size, int* generateCollisonPointsCount, CollisionPoint* collisionPoints, int cpSize);
cudaError_t GPU_HandleBoxCollisions(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, int size, float collisionRestitution);

cudaError_t UpdateBoxes(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, int size, float gravity, float dragCoefficient, rml::Vector3 boxMinBound, rml::Vector3 boxMaxBound, float deltaTime);

