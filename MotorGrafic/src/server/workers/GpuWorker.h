#pragma once

#include "BaseWorker.h"

class GpuWorker : public BaseWorker {
public:
	void ResolveMovement() override;

private:
	void PrepareDataForGpu();

private:
	//std::vector<rml::Vector3> _allPos;
	rml::Vector3 _allMin[MAX_CONNECTIONS];
	rml::Vector3 _allMax[MAX_CONNECTIONS];
	int _movedIds[MAX_CONNECTIONS]; //ids of the objects that moved last frame
	int _ids[MAX_CONNECTIONS];
	int _size = 0;
};
