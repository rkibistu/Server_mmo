#include "Scheduler.h"


#include "server/workers/BaseWorker.h"
#include "server/workers/CpuWorker.h"


Scheduler::Scheduler(WorkerType workerType) {

	if (workerType == WorkerType::CPU) {
		_worker = new CpuWorker();
	}
	else {

	}
}

void Scheduler::Add(SOCKET clientSocket, NetworkPackage package) {
	_packages.push({ clientSocket, package });
}

void Scheduler::Resolve() {

	while (_packages.empty() == false) {

		ScheudelerPackage package = _packages.front();
		_packages.pop();

		switch (package.Package.Tag) {
		case JoinGameRequest:
			_worker->HandleJoinGameRequest(package.ClientSocker, package.Package.Content);
			break;
		case DisconnectClientRequest:
			_worker->HandleDisconnectClientRequest(package.ClientSocker, package.Package.Content);
			break;
		case MoveRequest:
			_worker->HandleMoveRequest(package.ClientSocker, package.Package.Content);
			break;
		default:
			break;
		}
	}
}
