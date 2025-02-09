#pragma once

#include <queue>
#include "server/shared/NetworkMessages.h"

struct ScheudelerPackage {
	SOCKET ClientSocker;
	NetworkPackage Package;
};

class Scheduler{
public:
	void Add(SOCKET clientSocket, NetworkPackage package);

	// Resolve all the apckeages stored
	// Should be called once per frame
	void Resolve();

private:
	std::queue<ScheudelerPackage> _packages;
};
