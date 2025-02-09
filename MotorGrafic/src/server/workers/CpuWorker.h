#pragma once

#include "BaseWorker.h"

class CpuWorker : public BaseWorker{
public:
	void ResolveMovement() override;

private:
	bool CheckOverlap(const rml::Vector3& min1, const rml::Vector3& max1, const rml::Vector3& min2, const rml::Vector3& max2);
};
