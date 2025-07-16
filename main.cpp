#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include "utility.h"

using json = nlohmann::json;

int main()
{
	std::string encryptionKey;
	std::string encryptedFile;

	std::cout << "Enter the encryption key to the database: ";

	std::getline(std::cin, encryptionKey); // I was using this while testing, i will re-implement it later.

	std::cout << std::endl;

	std::cout << "Enter the ENCRYPTED Dumped Json File: ";
	std::getline(std::cin, encryptedFile);
	std::cout << std::endl;

	std::ifstream fileContentsStream{ encryptedFile };
	if (!fileContentsStream.is_open()) {
		std::cerr << "Failed to open file: " << encryptedFile << "\n";
		return 1;
	}

	std::cout << "-------------------------------------------\n";
	std::cout << "Discord Tickets Fast Dump (aes-256-gcm)\n";
	std::cout << "Encryption Key: " << encryptionKey << "\n";
	std::cout << "Filename: " << encryptedFile << "\n";
	std::cout << "-------------------------------------------\n";

	std::cout << "DUMPING DATABASE WITH ENC KEY..... Lines/Objects Count: ";

	size_t lineCount = 0;
	std::string jsonLine;

	while (std::getline(fileContentsStream, jsonLine))
	{
		if (!jsonLine.empty()) lineCount++;
	}

	std::cout << lineCount << std::endl << "\n\n";


	lineCount = 0;

	// - This reopens the file - //

	fileContentsStream.clear();
	fileContentsStream.seekg(0);

	// While loop below is to actually decode the lines


	/*
	JSONL is for making json objects but faster (? jsons/per line, not sure)
	Encoded Variables:
	content
	avatar
	displayName
	username
	comment
	value
	topic

	Anything inside of these names should be decoded.
	*/

	while (std::getline(fileContentsStream, jsonLine))
	{
		lineCount++;
		if (jsonLine.empty())
		{
			std::cout << "[!!] Detected Empty Line At " << lineCount << "\n";
			continue;
		};

		std::cout << "printing out encrypted line # : " << lineCount;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		const json data = json::parse(jsonLine);

		std::cout << data.dump(4) << std::endl; break;
	}
}