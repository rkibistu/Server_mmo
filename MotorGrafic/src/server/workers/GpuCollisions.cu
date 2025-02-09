#include "GpuCollisions.h"

__global__ void DetectBoxCollisionsKernel(rml::Vector3* minAABB, rml::Vector3* maxAABB, int* moveIds, int* ids, int size, int* collisionMarkers) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i >= size) return;

	if (moveIds[i] == 0) return; // check onyl for objects that moved

	for (int j = 0; j < size; j++) {
		if (i == j)
			continue;

		if ((minAABB[i].x <= maxAABB[j].x && maxAABB[i].x >= minAABB[j].x) &&
			(minAABB[i].y <= maxAABB[j].y && maxAABB[i].y >= minAABB[j].y) &&
			(minAABB[i].z <= maxAABB[j].z && maxAABB[i].z >= minAABB[j].z)) {

			//int x = ids[i];

			collisionMarkers[ids[i]] = 1;
		}
	}
}

cudaError_t GPU_DetectBoxCollisions(rml::Vector3* minAABB, rml::Vector3* maxAABB, int* moveIds, int* ids, int size, int* collisionMarkers, int collisionMarkersSize) {

	rml::Vector3* devMinAABB;
	rml::Vector3* devMaxAABB;
	int* devCollisionMarkers;
	int* devMoveIds;
	int* devIds;

	cudaError_t cudaStatus;

	dim3 threadsPerBlock(256);
	dim3 numBlocks((size + threadsPerBlock.x - 1) / threadsPerBlock.x);

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&devMinAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed 0 !");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMaxAABB, size * sizeof(rml::Vector3));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed 1!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devCollisionMarkers, collisionMarkersSize * sizeof(int));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed 2!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devMoveIds, size * sizeof(int));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed 3!");
		goto Error;
	}
	cudaStatus = cudaMalloc((void**)&devIds, size * sizeof(int));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed 4!");
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
	cudaStatus = cudaMemcpy(devMoveIds, moveIds, size * sizeof(int), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy maxaabb failed!\n");
		goto Error;
	}
	cudaStatus = cudaMemcpy(devIds, ids, size * sizeof(int), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy maxaabb failed!\n");
		goto Error;
	}
	cudaMemset(devCollisionMarkers, 0, collisionMarkersSize * sizeof(int));


	//CallTheKernel
	// Launch a kernel on the GPU with one thread for each element.
	DetectBoxCollisionsKernel << <numBlocks, threadsPerBlock >> > (devMinAABB, devMaxAABB, devMoveIds, devIds, size, devCollisionMarkers);

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

	cudaStatus = cudaMemcpy(collisionMarkers, devCollisionMarkers, collisionMarkersSize * sizeof(int), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy genC failed!\n");
		goto Error;
	}


Error:
	cudaFree(devMinAABB);
	cudaFree(devMaxAABB);
	cudaFree(devCollisionMarkers);
	cudaFree(devMoveIds);
	cudaFree(devIds);

	return cudaStatus;
}
