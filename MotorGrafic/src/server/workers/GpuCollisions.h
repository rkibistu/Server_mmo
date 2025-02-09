#pragma once

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <utils/Math.h>

cudaError_t GPU_DetectBoxCollisions(rml::Vector3* minAABB, rml::Vector3* maxAABB, int* moveIds, int* ids, int size, int* collisionMarkers, int collisionMarkersSize);

