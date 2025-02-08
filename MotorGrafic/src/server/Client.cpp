#include "Client.h"

Client::Client(SOCKET fd) {
	_fd = fd;
}

Client::~Client() {
	if (_player) {
		delete _player;
		_player = nullptr;
	}
}
