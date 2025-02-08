#pragma once

#include <iostream>
#include <pqxx/pqxx>

class DatabaseManager {
public:
	// Constructor: Establish connection
	DatabaseManager() {
		try {
			_conn = new pqxx::connection("dbname=postgres user=postgres password=Parola.1234 host=localhost port=5432");
			if (_conn->is_open()) {
				std::cout << "Connected to database successfully!" << std::endl;
			}
			else {
				std::cerr << "Failed to connect to the database." << std::endl;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Connection error: " << e.what() << std::endl;
			_conn = nullptr;
		}
	}

	// Destructor: Close connection
	~DatabaseManager() {
		if (_conn) {
			_conn->close();
			delete _conn;
			std::cout << "Database connection closed." << std::endl;
		}
	}

	// Insert into Users table
	bool insertUser(const std::string& username, const std::string& password, double x, double y, double z) {
		if (!_conn || !_conn->is_open()) {
			std::cerr << "Database not connected!" << std::endl;
			return false;
		}

		try {
			pqxx::work txn(*_conn);
			std::string query = "INSERT INTO public.\"Users\" (username, password, \"positionX\", \"positionY\", \"positionZ\") VALUES("
				"'" + username + "', "
				"'" + password + "', "
				+ std::to_string(x) + ", "
				+ std::to_string(y) + ", "
				+ std::to_string(z) + ");";
			pqxx::result r = txn.exec(query);
			txn.commit();
			std::cout << "User inserted successfully!" << std::endl;
			return true;
		}
		catch (const std::exception& e) {
			std::cerr << "Insert error: " << e.what() << std::endl;
			return false;
		}
	}

	// Select all users
	void selectUsers() {
		if (!_conn || !_conn->is_open()) {
			std::cerr << "Database not connected!" << std::endl;
			return;
		}

		try {
			pqxx::work  txn(*_conn);
			pqxx::result res = txn.exec("SELECT id, username, password, \"positionX\", \"positionY\", \"positionZ\" FROM public.\"Users\"; ");
			txn.commit();
			std::cout << res[0][0].c_str() << std::endl;
			for (auto row : res) {
				std::cout << row[0].c_str() << std::endl;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Select error: " << e.what() << std::endl;
		}
	}

	// Select all users
	void selectUser(std::string username) {
		if (!_conn || !_conn->is_open()) {
			std::cerr << "Database not connected!" << std::endl;
			return;
		}

		try {
			pqxx::work  txn(*_conn);
			pqxx::result res = txn.exec("SELECT id, username, password, \"positionX\", \"positionY\", \"positionZ\" FROM public.\"Users\"; ");
			txn.commit();
			std::cout << res[0][0].c_str() << std::endl;
			for (auto row : res) {
				auto [dbId, dbUsername, dbPassword, dbPosX, dbPosY, dbPosZ] = row.as<int, std::string, std::string, float, float, float>();
				
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Select error: " << e.what() << std::endl;
		}
	}
private:
	pqxx::connection* _conn;
};

// Main function to test DatabaseManager
//int main() {
//    DatabaseManager db;
//
//    // Insert a test user
//    db.insertUser("testUser", "testPass", 1.1, 2.2, 3.3);
//
//    // Select and display all users
//    db.selectUsers();
//
//    return 0;
//}
