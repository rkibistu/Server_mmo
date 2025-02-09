#include "GpuWorker.h"

#include "GpuCollisions.h"

void GpuWorker::ResolveMovement() {
	Server& server = Server::GetInstance();
	if (server._conIdClients.size() <= 0)
		return;

	memset(_collisionMarker, 0, MAX_CONNECTIONS);

	int index = 0;
	//Call Gpu method for player-player collision detection
	// output -> collisionMarker
	PrepareDataForGpu();
	GPU_DetectBoxCollisions(_allMin, _allMax, _movedIds, _ids, _size, _collisionMarker, MAX_CONNECTIONS);

	//check collisions with the environment

	//rollback movement for colliding players
	//for (int i = 0; i < MAX_CONNECTIONS; i++) {
	//	if (_collisionMarker[i] != -1) {
	//		MoveData data = server._movedClients[i];
	//		Player* player = server._conIdClients[data.Id]->GetPlayer();

	//		player->RollbackMovement();
	//	}
	//}

	for (auto client : server._conIdClients) {

		Player* player = client.second->GetPlayer();
		if (player != nullptr) {
			if (_collisionMarker[player->GetId()]) {
				player->RollbackMovement();
			}
			else {
				client.second->GetPlayer()->ResetAccumulatedMovement();
			}
		}
	}
}

void GpuWorker::PrepareDataForGpu() {
	Server& server = Server::GetInstance();
	int size = server._conIdClients.size();

	Player* player = nullptr;
	BoundingVolume bv;
	int x;
	int index = 0;
	for (auto client : server._conIdClients) {
		player = client.second->GetPlayer();
		if (player) {
			bv = player->GetTransformedBoundingVolume();
			_allMin[index] = (bv.Min);
			_allMax[index] = (bv.Max);

			x = (player->HasMoved()) ? 1 : 0;
			_movedIds[index] = (x);

			_ids[index] = (player->GetId());
		}
		index++;
	}

	_size = index;
}
