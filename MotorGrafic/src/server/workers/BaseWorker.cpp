#include "BaseWorker.h"

void BaseWorker::HandleJoinGameRequest(SOCKET clientSocket, std::string messageContent) {
	Server& server = Server::GetInstance();
	JoinGameRequestData data(messageContent);

	// we use -1 to mark a failed login
	JoinGameResponseData response(-1);

	Player* player = server.HandleLogin(data.Username, data.Password);
	if (player == nullptr) {
		server.Send(clientSocket, NetworkTags::JoinGameResponse, response.Serialize());
		return;
	}

	Client* client = server.GetClient(clientSocket);
	client->SetPlayer(player);
	server.AddNewClientToIdDict(client->GetID(), client);
	std::cout << "Created player for client with socket" << clientSocket << "and ID: " << client->GetID() << "\n";


	// trimite informatiile despre noul client tuturor celorlalti
	InfoConnectedClientData info{ client->GetID(), client->GetUsername() };
	server.SendToAllExcept(clientSocket, InfoConnectedClient, info.Serialize());

	//trimite raspuns la cel care a initiat logarea
	response.Id = client->GetID();
	response.Pos = client->GetPlayer()->GetPosition();
	server.Send(clientSocket, NetworkTags::JoinGameResponse, response.Serialize());

	//trimite info clientului nou despre toti clientii conectati
	std::vector<InfoConnectedClientData> temp;
	for (auto client : server._conClients) {
		if (client.first != clientSocket)
			temp.push_back({ client.second->GetID(), client.second->GetUsername() });
	}
	InfoConnectedClientsData allConClients(temp);
	server.Send(clientSocket, NetworkTags::InfoConnectedClients, allConClients.Serialize());
}

void BaseWorker::HandleDisconnectClientRequest(SOCKET clientSocket, std::string messageContent) {
	Server& server = Server::GetInstance();

	server.DisconnectClient(clientSocket);
}

void BaseWorker::HandleMoveRequest(SOCKET clientSocket, std::string messageContent) {
	Server& server = Server::GetInstance();

	MoveRequestData data(messageContent);

	//update the magnitude so the user doesn't move faster than it is allowed
	float magnitude = data.Movement.Length();
	if (magnitude > MAX_MOVEMENT_MAGNITUDE) {
		data.Movement = data.Movement.Normalize() * MAX_MOVEMENT_MAGNITUDE;
	}

	//update the position on the server
	Player* player = server._conClients[clientSocket]->GetPlayer();
	if (player == nullptr)
		return;
	player->Move(data.Movement); //todo: add here some collision detection. and MOVE this
	// MOVE this movement method. This should be called at the end of the loop in a loop for all palyers that moved
	// easier to write parallel code like that

	// don t send the new position, it will be to much traffic
	//mark player as a moved player and send at the end of the frame the ifno about all players
	if (server._movedClientIds.find(player->GetId()) == server._movedClientIds.end()) {

		server._movedClientIds.insert(player->GetId());
		server._movedClients.push_back({ player->GetId(), player->GetPosition() });
	}
}
