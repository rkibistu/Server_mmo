#include "CpuWorker.h"

void CpuWorker::ResolveMovement() {

	Server& server = Server::GetInstance();

	memset(_collisionMarker, 0, MAX_CONNECTIONS);

	//cehck collisions with all other palyers
	Player* player1 = nullptr;
	Player* player2 = nullptr;
	BoundingVolume bv1, bv2;
	int index = 0;
	for (auto it = server._movedClientIds.begin(); it != server._movedClientIds.end(); it++) {
		
		for (auto it2 = server._conIdClients.begin(); it2 != server._conIdClients.end(); it2++) {
			player1 = server._conIdClients[*it]->GetPlayer();
			player2 = it2->second->GetPlayer();
			if (player1->GetId() == player2->GetId())
				continue;

			bv1 = player1->GetTransformedBoundingVolume();
			bv2 = player2->GetTransformedBoundingVolume();

			if (CheckOverlap(bv1.Min, bv1.Max, bv2.Min, bv2.Max)) {
				//one collision detected 
				//mark to rollback
				_collisionMarker[index] = 1;
				break;
			}
		}
		index++;
	}

	//check collisions with the environment

	//rollback movement for colliding players
	for (int i = 0; i < index; i++) {
		if (_collisionMarker[i] == 1) {
			MoveData data = server._movedClients[i];
			Player* player = server._conIdClients[data.Id]->GetPlayer();

			player->RollbackMovement();
		}
	}

	for (auto client : server._conIdClients) {
		client.second->GetPlayer()->ResetAccumulatedMovement();
	}
}

bool CpuWorker::CheckOverlap(const rml::Vector3& min1, const rml::Vector3& max1, const rml::Vector3& min2, const rml::Vector3& max2) {
	return (min1.x <= max2.x && max1.x >= min2.x) &&
		(min1.y <= max2.y && max1.y >= min2.y) &&
		(min1.z <= max2.z && max1.z >= min2.z);
}

