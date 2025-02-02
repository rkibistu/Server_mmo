#include "PhysicsGpu.h"
#include "PhysicsController.h"

#include <iostream>
#include <stdio.h>

__global__ void UpdateBoxesKernel(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, float gravity, float dragCoefficient, rml::Vector3 boxMinBound, rml::Vector3 boxMaxBound, float deltaTime) {



	//gravity
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	velocity[i].y -= gravity * deltaTime;

	//friction
	//float vectorLength = sqrt(velocity[i].x * velocity[i].x + velocity[i].y * velocity[i].y + velocity[i].z * velocity[i].z);
	//if (vectorLength > 0.0f) { // Only apply drag if the object is moving
	//	// Normalize the velocity vector to get the direction
	//	// rml::Vector3 velocityDirection = velocity[i];
	//	// velocityDirection = velocityDirection.Normalize();
	//	float xNorm = velocity[i].x * (1.0f / vectorLength);
	//	float yNorm = velocity[i].y * (1.0f / vectorLength);
	//	float zNorm = velocity[i].z * (1.0f / vectorLength);

	//	// Calculate the drag force (opposing direction of motion)
	//	// rml::Vector3 dragForce = -velocityDirection * dragCoefficient * speed;

	//	// Apply the drag force to the object's velocity
	//	//velocity[i] += dragForce;
	//	velocity[i].x += -xNorm * dragCoefficient * vectorLength;
	//	velocity[i].y += -yNorm * dragCoefficient * vectorLength;
	//	velocity[i].z += -zNorm * dragCoefficient * vectorLength;
	//}

	////update pos
	position[i].x += velocity[i].x * deltaTime;
	position[i].y += velocity[i].y * deltaTime;
	position[i].z += velocity[i].z * deltaTime;

	//wall collisions
	float _wallRestitution = 0.1f;
	float minBoundX = minAABB[i].x + position[i].x;
	float minBoundY = minAABB[i].y + position[i].y;
	float minBoundZ = minAABB[i].z + position[i].z;

	float maxBoundX = maxAABB[i].x + position[i].x;
	float maxBoundY = maxAABB[i].y + position[i].y;
	float maxBoundZ = maxAABB[i].z + position[i].z;

	if (minBoundX < boxMinBound.x) {
		position[i].x = boxMinBound.x + (position[i].x - minBoundX);
		velocity[i].x *= -_wallRestitution;
	}
	else if (maxBoundX > boxMaxBound.x) {
		position[i].x = boxMaxBound.x - (maxBoundX - position[i].x);
		velocity[i].x *= -_wallRestitution;
	}

	if (minBoundY < boxMinBound.y) {
		position[i].y = boxMinBound.y + (position[i].y - minBoundY);
		velocity[i].y *= -_wallRestitution;
	}
	else if (maxBoundY > boxMaxBound.y) {
		position[i].y = boxMaxBound.y - (maxBoundY - position[i].y);
		velocity[i].y *= -_wallRestitution;
	}

	if (minBoundZ < boxMinBound.z) {
		position[i].z = boxMinBound.z + (position[i].z - minBoundZ);
		velocity[i].z *= -_wallRestitution;
	}
	else if (maxBoundZ > boxMaxBound.z) {
		position[i].z = boxMaxBound.z - (maxBoundZ - position[i].z);
		velocity[i].z *= -_wallRestitution;
	}
}

__global__ void ApplyGravityKernel(rml::Vector3* velocity, int size, float gravity, float deltaTime) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	velocity[i].y -= gravity * deltaTime;
}
__global__ void ApplyFrictionKernel(rml::Vector3* velocity, int size, float drag, float deltaTime) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;

	float vectorLength = sqrt(velocity[i].x * velocity[i].x + velocity[i].y * velocity[i].y + velocity[i].z * velocity[i].z);
	if (vectorLength > 0.0f) { // Only apply drag if the object is moving
		// Normalize the velocity vector to get the direction
		// rml::Vector3 velocityDirection = velocity[i];
		// velocityDirection = velocityDirection.Normalize();
		float xNorm = velocity[i].x * (1.0f / vectorLength);
		float yNorm = velocity[i].y * (1.0f / vectorLength);
		float zNorm = velocity[i].z * (1.0f / vectorLength);

		// Calculate the drag force (opposing direction of motion)
		// rml::Vector3 dragForce = -velocityDirection * dragCoefficient * speed;

		// Apply the drag force to the object's velocity
		//velocity[i] += dragForce;
		velocity[i].x += -xNorm * drag * vectorLength;
		velocity[i].y += -yNorm * drag * vectorLength;
		velocity[i].z += -zNorm * drag * vectorLength;
	}
}
__global__ void UpdateApplyingGravityAndFrictionKernel(rml::Vector3* position, rml::Vector3* velocity, int size, float gravity, float drag, float deltaTime) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;

	velocity[i].y -= gravity * deltaTime;

	float vectorLength = sqrt(velocity[i].x * velocity[i].x + velocity[i].y * velocity[i].y + velocity[i].z * velocity[i].z);
	if (vectorLength > 0.0f) { // Only apply drag if the object is moving
		// Normalize the velocity vector to get the direction
		// rml::Vector3 velocityDirection = velocity[i];
		// velocityDirection = velocityDirection.Normalize();
		float xNorm = velocity[i].x * (1.0f / vectorLength);
		float yNorm = velocity[i].y * (1.0f / vectorLength);
		float zNorm = velocity[i].z * (1.0f / vectorLength);

		// Calculate the drag force (opposing direction of motion)
		// rml::Vector3 dragForce = -velocityDirection * dragCoefficient * speed;

		// Apply the drag force to the object's velocity
		//velocity[i] += dragForce;
		velocity[i].x += -xNorm * drag * vectorLength;
		velocity[i].y += -yNorm * drag * vectorLength;
		velocity[i].z += -zNorm * drag * vectorLength;
	}

	// If you comment the line that udpates Y position and update it from the CPU, it works good.
	// If you update it here, all the objects go to left? How ? why ?
	position[i].x = position[i].x + velocity[i].x * deltaTime;
	position[i].y = position[i].y + velocity[i].y * deltaTime;
	position[i].z = position[i].z + velocity[i].z * deltaTime;
}

__global__ void HandleBoxWallCollisionsKernel(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, rml::Vector3 minBoxBound, rml::Vector3 maxBoxBound, int size) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;

	float _wallRestitution = 0.1f;
	float minBoundX = minAABB[i].x;//+ position[i].x;
	float minBoundY = minAABB[i].y;//+ position[i].y;
	float minBoundZ = minAABB[i].z;//+ position[i].z;

	float maxBoundX = maxAABB[i].x;//+ position[i].x;
	float maxBoundY = maxAABB[i].y;//+ position[i].y;
	float maxBoundZ = maxAABB[i].z;// +position[i].z;

	if (minBoundX < minBoxBound.x) {
		position[i].x = minBoxBound.x + (position[i].x - minBoundX);
		velocity[i].x *= -_wallRestitution;
	}
	else if (maxBoundX > maxBoxBound.x) {
		position[i].x = maxBoxBound.x - (maxBoundX - position[i].x);
		velocity[i].x *= -_wallRestitution;
	}

	if (minBoundY < minBoxBound.y) {
		position[i].y = minBoxBound.y + (position[i].y - minBoundY);
		velocity[i].y *= -_wallRestitution;
	}
	else if (maxBoundY > maxBoxBound.y) {
		position[i].y = maxBoxBound.y - (maxBoundY - position[i].y);
		velocity[i].y *= -_wallRestitution;
	}

	if (minBoundZ < minBoxBound.z) {
		position[i].z = minBoxBound.z + (position[i].z - minBoundZ);
		velocity[i].z *= -_wallRestitution;
	}
	else if (maxBoundZ > maxBoxBound.z) {
		position[i].z = maxBoxBound.z - (maxBoundZ - position[i].z);
		velocity[i].z *= -_wallRestitution;
	}
}
__global__ void HandleSphereWallCollisionsKernel(rml::Vector3* position, rml::Vector3* velocity, float* radius, rml::Vector3 minBoxBound, rml::Vector3 maxBoxBound, int size) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;

	float _wallRestitution = 0.5f;
	if (position[i].x - radius[i] < minBoxBound.x) {
		position[i].x = minBoxBound.x + radius[i];
		velocity[i].x *= -_wallRestitution;
	}
	else if (position[i].x + radius[i] > maxBoxBound.x) {
		position[i].x = maxBoxBound.x - radius[i];
		velocity[i].x *= -_wallRestitution;
	}
	if (position[i].y - radius[i] < minBoxBound.y) {
		position[i].y = minBoxBound.y + radius[i];
		velocity[i].y *= -_wallRestitution;
	}
	else if (position[i].y + radius[i] > maxBoxBound.y) {
		position[i].y = maxBoxBound.y - radius[i];
		velocity[i].y *= -_wallRestitution;
	}
	if (position[i].z - radius[i] < minBoxBound.z) {
		position[i].z = minBoxBound.z + radius[i];
		velocity[i].z *= -_wallRestitution;
	}
	else if (position[i].z + radius[i] > maxBoxBound.z) {
		position[i].z = maxBoxBound.z - radius[i];
		velocity[i].z *= -_wallRestitution;
	}
}

__global__ void HandleBoxCollisionsKernel(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, int size, float collisionRestitution, int i) {
	int j = blockIdx.x * blockDim.x + threadIdx.x;


	if (i <= j) return;

	// Check for overlap between the bounding volumes (AABBs)
	if ((minAABB[i].x <= maxAABB[j].x && maxAABB[i].x >= minAABB[j].x) &&
		(minAABB[i].y <= maxAABB[j].y && maxAABB[i].y >= minAABB[j].y) &&
		(minAABB[i].z <= maxAABB[j].z && maxAABB[i].z >= minAABB[j].z)) {
		// Determine the penetration depth along each axis
		float penetrationX;
		float penetrationY;
		float penetrationZ;


		//penetration.x = std::min(maxAABB[i].x - minAABB[j].x, maxAABB[j].x - minAABB[i].x);
		//penetration.y = std::min(maxAABB[i].y - minAABB[j].y, maxAABB[j].y - minAABB[i].y);
		//penetration.z = std::min(maxAABB[i].z - minAABB[j].z, maxAABB[j].z - minAABB[i].z);
		penetrationX = (maxAABB[i].x - minAABB[j].x < maxAABB[j].x - minAABB[i].x) ?
			(maxAABB[i].x - minAABB[j].x) :
			(maxAABB[j].x - minAABB[i].x);

		penetrationY = (maxAABB[i].y - minAABB[j].y < maxAABB[j].y - minAABB[i].y) ?
			(maxAABB[i].y - minAABB[j].y) :
			(maxAABB[j].y - minAABB[i].y);

		penetrationZ = (maxAABB[i].z - minAABB[j].z < maxAABB[j].z - minAABB[i].z) ?
			(maxAABB[i].z - minAABB[j].z) :
			(maxAABB[j].z - minAABB[i].z);

		// Threshold to avoid jitter on small penetrations
/*		const float penetrationThreshold = 0.01f;
		if (penetration.x < penetrationThreshold && penetration.y < penetrationThreshold && penetration.z < penetrationThreshold) {
			return;
		}*/

		// Find the axis of least penetration to separate objects along that axis
		if (penetrationX < penetrationY && penetrationX < penetrationZ) {
			if (position[i].x < position[j].x) {
				position[i].x -= penetrationX / 2;
				position[j].x += penetrationX / 2;
			}
			else {
				position[i].x += penetrationX / 2;
				position[j].x -= penetrationX / 2;
			}
			velocity[i].x *= -collisionRestitution;
			velocity[j].x *= -collisionRestitution;
		}
		else if (penetrationY < penetrationZ) {
			if (position[i].y < position[j].y) {
				position[i].y -= penetrationY / 2;
				position[j].y += penetrationY / 2;
			}
			else {
				position[i].y += penetrationY / 2;
				position[j].y -= penetrationY / 2;
			}
			velocity[i].y *= -collisionRestitution;
			velocity[j].y *= -collisionRestitution;
		}
		else {
			if (position[i].z < position[j].z) {
				position[i].z -= penetrationZ / 2;
				position[j].z += penetrationZ / 2;
			}
			else {
				position[i].z += penetrationZ / 2;
				position[j].z -= penetrationZ / 2;
			}
			velocity[i].z *= -collisionRestitution;
			velocity[j].z *= -collisionRestitution;
		}
	}

}

__global__ void DetectBoxCollisionsKernel(rml::Vector3* position, rml::Vector3* minAABB, rml::Vector3* maxAABB, int size, int* generateCollisonPointsCount, CollisionPoint* collisionPoints, int cpSize) {
	//current eleemnt to check versus all of them
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i >= size) return;


	// fiecare thread poate folosi zona asta fara sa isi faca griji de acces concurent la memorie (vectorul collisionPoints)
	// i * size -> i * size + size - 1

	//the number fo collisions detected for object i
	int countCollisions = 0;
	float penetrationX, penetrationY, penetrationZ;
	for (int j = i + 1; j < size; j++) {

		//check if object i collides with object j
		if ((minAABB[i].x <= maxAABB[j].x && maxAABB[i].x >= minAABB[j].x) &&
			(minAABB[i].y <= maxAABB[j].y && maxAABB[i].y >= minAABB[j].y) &&
			(minAABB[i].z <= maxAABB[j].z && maxAABB[i].z >= minAABB[j].z)) {

			//Calculate penetration
			penetrationX = (maxAABB[i].x - minAABB[j].x < maxAABB[j].x - minAABB[i].x) ? (maxAABB[i].x - minAABB[j].x) : (maxAABB[j].x - minAABB[i].x);
			penetrationY = (maxAABB[i].y - minAABB[j].y < maxAABB[j].y - minAABB[i].y) ? (maxAABB[i].y - minAABB[j].y) : (maxAABB[j].y - minAABB[i].y);
			penetrationZ = (maxAABB[i].z - minAABB[j].z < maxAABB[j].z - minAABB[i].z) ? (maxAABB[i].z - minAABB[j].z) : (maxAABB[j].z - minAABB[i].z);

			//Check for threshold
			if (penetrationX > 0.01 || penetrationY > 0.01 || penetrationZ > 0.01) {
				//Add to the penetration array
				collisionPoints[i * size + countCollisions].Object1 = i;
				collisionPoints[i * size + countCollisions].Object2 = j;
				collisionPoints[i * size + countCollisions].Penetration.x = penetrationX;
				collisionPoints[i * size + countCollisions].Penetration.y = penetrationY;
				collisionPoints[i * size + countCollisions].Penetration.z = penetrationZ;

				countCollisions++;
			}
		}
	}

	generateCollisonPointsCount[i] = countCollisions;

	//generateCollisonPointsCount[i] = 1;

	//collisionPoints[i * size + 0].Penetration.x = 10;
	//collisionPoints[i * size + 0].Penetration.y = 10;
	//collisionPoints[i * size + 0].Penetration.z = 10;
}
__global__ void DetectSphereCollisionsKernel(rml::Vector3* position, float* radius, int size, int* generateCollisonPointsCount, CollisionPoint* collisionPoints, int cpSize) {
	//current eleemnt to check versus all of them
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i >= size) return;

	int countCollisions = 0;
	float penetrationX, penetrationY, penetrationZ;
	for (int j = i + 1; j < size; j++) {

		float distanceVecX = position[j].x - position[i].x;
		float distanceVecY = position[j].y - position[i].y;
		float distanceVecZ = position[j].z - position[i].z;
		double distance = sqrt(distanceVecX * distanceVecX + distanceVecY * distanceVecY + distanceVecZ * distanceVecZ);
		double collisionDistance = radius[i] + radius[j];

		if (distance - collisionDistance < 0.01f) {
			//rml::Vector3 normal = distanceVec.Normalize();
			float xNorm = distanceVecX;
			float yNorm = distanceVecY;
			float zNorm = distanceVecZ;
			if (distance != 0) {

				float xNorm = distanceVecX * (1.0f / distance);
				float yNorm = distanceVecY * (1.0f / distance);
				float zNorm = distanceVecZ * (1.0f / distance);
			}

			double penetrationDepth = collisionDistance - distance;

			float xPenetration = xNorm * penetrationDepth;
			float yPenetration = yNorm * penetrationDepth;
			float zPenetration = zNorm * penetrationDepth;

			collisionPoints[i * size + countCollisions].Object1 = i;
			collisionPoints[i * size + countCollisions].Object2 = j;
			collisionPoints[i * size + countCollisions].Penetration.x = xPenetration;
			collisionPoints[i * size + countCollisions].Penetration.y = yPenetration;
			collisionPoints[i * size + countCollisions].Penetration.z = zPenetration;

			countCollisions++;
		}
	}
	generateCollisonPointsCount[i] = countCollisions;
}

cudaError_t GPU_ApplyGravity(rml::Vector3* velocity, int size, float gravity, float deltaTime) {
	rml::Vector3* devVelocity;
	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devVelocity, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(devVelocity, velocity, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Launch a kernel on the GPU with one thread for each element.
	ApplyGravityKernel << <numBlocks, threadsPerBlock >> > (devVelocity, size, gravity, deltaTime);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	cudaStatus = cudaMemcpy(velocity, devVelocity, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(devVelocity);

	return cudaStatus;
}

cudaError_t GPU_ApplyFriction(rml::Vector3* velocity, int size, float drag, float deltaTime) {
	rml::Vector3* devVelocity;
	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devVelocity, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(devVelocity, velocity, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Launch a kernel on the GPU with one thread for each element.
	ApplyFrictionKernel << <numBlocks, threadsPerBlock >> > (devVelocity, size, drag, deltaTime);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	cudaStatus = cudaMemcpy(velocity, devVelocity, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(devVelocity);

	return cudaStatus;
}

cudaError_t GPU_UpdateApplyingGravityAndFriction(rml::Vector3* position, rml::Vector3* velocity, int size, float gravity, float drag, float deltaTime) {
	rml::Vector3* devPosition;
	rml::Vector3* devVelocity;
	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devPosition, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devVelocity, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(devPosition, position, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devVelocity, velocity, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Launch a kernel on the GPU with one thread for each element.
	UpdateApplyingGravityAndFrictionKernel << <numBlocks, threadsPerBlock >> > (devPosition, devVelocity, size, gravity, drag, deltaTime);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	cudaStatus = cudaMemcpy(position, devPosition, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(velocity, devVelocity, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(devVelocity);
	cudaFree(devPosition);

	return cudaStatus;
}

cudaError_t GPU_HandleBoxWallCollisions(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, rml::Vector3 minBoxBound, rml::Vector3 maxBoxBound, int size) {
	rml::Vector3* devPosition;
	rml::Vector3* devVelocity;
	rml::Vector3* devMinAABB;
	rml::Vector3* devMaxAABB;
	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devPosition, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devVelocity, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMinAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMaxAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(devPosition, position, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devVelocity, velocity, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devMinAABB, minAABB, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devMaxAABB, maxAABB, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Launch a kernel on the GPU with one thread for each element.
	HandleBoxWallCollisionsKernel << <numBlocks, threadsPerBlock >> > (devPosition, devVelocity, devMinAABB, devMaxAABB, minBoxBound, maxBoxBound, size);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	cudaStatus = cudaMemcpy(position, devPosition, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(velocity, devVelocity, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(devVelocity);
	cudaFree(devPosition);
	cudaFree(devMinAABB);
	cudaFree(devMaxAABB);

	return cudaStatus;
}

cudaError_t GPU_HandleSphereWallCollisions(rml::Vector3* position, rml::Vector3* velocity, float* radius, rml::Vector3 minBoxBound, rml::Vector3 maxBoxBound, int size) {
	rml::Vector3* devPosition;
	rml::Vector3* devVelocity;
	float* devRadius;
	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devPosition, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devVelocity, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devRadius, size * sizeof(float));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}


	cudaStatus = cudaMemcpy(devPosition, position, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devVelocity, velocity, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devRadius, radius, size * sizeof(float), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}


	// Launch a kernel on the GPU with one thread for each element.
	HandleSphereWallCollisionsKernel << <numBlocks, threadsPerBlock >> > (devPosition, devVelocity, devRadius, minBoxBound, maxBoxBound, size);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	cudaStatus = cudaMemcpy(position, devPosition, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(velocity, devVelocity, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(devVelocity);
	cudaFree(devPosition);
	cudaFree(devRadius);

	return cudaStatus;
}

cudaError_t GPU_DetectBoxCollisions(rml::Vector3* position, rml::Vector3* minAABB, rml::Vector3* maxAABB, int size, int* generateCollisonPointsCount, CollisionPoint* collisionPoints, int cpSize) {

	rml::Vector3* devPosition;
	rml::Vector3* devMinAABB;
	rml::Vector3* devMaxAABB;
	int* devGenerateCollisionPointsCount;
	CollisionPoint* devCollisionPoints;

	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devPosition, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMinAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMaxAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devGenerateCollisionPointsCount, size * sizeof(int));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devCollisionPoints, cpSize * sizeof(CollisionPoint));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(devPosition, position, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy pos failed!\n");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devMinAABB, minAABB, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy minaabb failed!\n");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devMaxAABB, maxAABB, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy maxaabb failed!\n");
		goto Error;
	}
	cudaMemset(devGenerateCollisionPointsCount, 0, size * sizeof(int));
	cudaMemset(devCollisionPoints, 0, cpSize * sizeof(CollisionPoint));

	//CallTheKernel
	// Launch a kernel on the GPU with one thread for each element.
	DetectBoxCollisionsKernel << <numBlocks, threadsPerBlock >> > (devPosition, devMinAABB, devMaxAABB, size, devGenerateCollisionPointsCount, devCollisionPoints, cpSize);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	cudaStatus = cudaMemcpy(generateCollisonPointsCount, devGenerateCollisionPointsCount, size * sizeof(int), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy genC failed!\n");
		goto Error;
	}
	cudaStatus = cudaMemcpy(collisionPoints, devCollisionPoints, cpSize * sizeof(CollisionPoint), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy cp failed!\n");
		goto Error;
	}

Error:
	cudaFree(devPosition);
	cudaFree(devMinAABB);
	cudaFree(devMaxAABB);
	cudaFree(devGenerateCollisionPointsCount);
	cudaFree(devCollisionPoints);

	return cudaStatus;
}

cudaError_t GPU_DetectSphereCollisions(rml::Vector3* position, float* radius, int size, int* generateCollisonPointsCount, CollisionPoint* collisionPoints, int cpSize) {
	rml::Vector3* devPosition;
	float* devRadius;
	int* devGenerateCollisionPointsCount;
	CollisionPoint* devCollisionPoints;

	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devPosition, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devRadius, size * sizeof(float));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devGenerateCollisionPointsCount, size * sizeof(int));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devCollisionPoints, cpSize * sizeof(CollisionPoint));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(devPosition, position, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy pos failed!\n");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devRadius, radius, size * sizeof(float), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy minaabb failed!\n");
		goto Error;
	}
	cudaMemset(devGenerateCollisionPointsCount, 0, size * sizeof(int));
	cudaMemset(devCollisionPoints, 0, cpSize * sizeof(CollisionPoint));

	//CallTheKernel
	// Launch a kernel on the GPU with one thread for each element.
	DetectSphereCollisionsKernel << <numBlocks, threadsPerBlock >> > (devPosition, devRadius, size, devGenerateCollisionPointsCount, devCollisionPoints, cpSize);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	cudaStatus = cudaMemcpy(generateCollisonPointsCount, devGenerateCollisionPointsCount, size * sizeof(int), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy genC failed!\n");
		goto Error;
	}
	cudaStatus = cudaMemcpy(collisionPoints, devCollisionPoints, cpSize * sizeof(CollisionPoint), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy cp failed!\n");
		goto Error;
	}

Error:
	cudaFree(devPosition);
	cudaFree(devRadius);
	cudaFree(devGenerateCollisionPointsCount);
	cudaFree(devCollisionPoints);

	return cudaStatus;
}

cudaError_t GPU_HandleBoxCollisions(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, int size, float collisionRestitution) {
	rml::Vector3* devPosition;
	rml::Vector3* devVelocity;
	rml::Vector3* devMinAABB;
	rml::Vector3* devMaxAABB;
	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devPosition, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devVelocity, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMinAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMaxAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(devPosition, position, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devVelocity, velocity, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devMinAABB, minAABB, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devMaxAABB, maxAABB, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Launch a kernel on the GPU with one thread for each element.
	for (int i = 0; i < size; i++) {
		HandleBoxCollisionsKernel << <numBlocks, threadsPerBlock >> > (devPosition, devVelocity, devMinAABB, devMaxAABB, size, collisionRestitution, i);
		// Check for any errors launching the kernel
		cudaStatus = cudaGetLastError();
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
			goto Error;
		}

		// cudaDeviceSynchronize waits for the kernel to finish, and returns
		// any errors encountered during the launch.
		cudaStatus = cudaDeviceSynchronize();
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
			goto Error;
		}
	}





	cudaStatus = cudaMemcpy(position, devPosition, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(velocity, devVelocity, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(devVelocity);
	cudaFree(devPosition);
	cudaFree(devMinAABB);
	cudaFree(devMaxAABB);

	return cudaStatus;
}


cudaError_t UpdateBoxes(rml::Vector3* position, rml::Vector3* velocity, rml::Vector3* minAABB, rml::Vector3* maxAABB, int size, float gravity, float dragCoefficient, rml::Vector3 boxMinBound, rml::Vector3 boxMaxBound, float deltaTime) {

	rml::Vector3* devPosition;
	rml::Vector3* devVelocity;
	rml::Vector3* devMinAABB;
	rml::Vector3* devMaxAABB;
	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	//gpu buffers
	cudaStatus = cudaMalloc((void**)&devPosition, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devVelocity, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMinAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMaxAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;

	}

	// Copy input vectors from host memory to GPU buffers.
	cudaStatus = cudaMemcpy(devPosition, position, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devVelocity, velocity, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devMinAABB, minAABB, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devMaxAABB, maxAABB, size * sizeof(rml::Vector3), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Launch a kernel on the GPU with one thread for each element.
	UpdateBoxesKernel << <numBlocks, threadsPerBlock >> > (devPosition, devVelocity, devMinAABB, devMaxAABB, gravity, dragCoefficient, boxMinBound, boxMaxBound, deltaTime);

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
		goto Error;
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		goto Error;
	}

	// Copy output vector from GPU buffer to host memory.
	cudaStatus = cudaMemcpy(position, devPosition, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}
	cudaStatus = cudaMemcpy(velocity, devVelocity, size * sizeof(rml::Vector3), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

Error:
	cudaFree(devPosition);
	cudaFree(devVelocity);
	cudaFree(devMinAABB);
	cudaFree(devMaxAABB);

	return cudaStatus;
}
