#include <iostream>
#include <filesystem>
#include <string>
#include "utility.h"

#include <SQLiteCpp/SQLiteCpp.h>

int main()
{
	std::string encryptionKey;
	std::string hex;

	std::cout << "Enter the encryption key to the database: ";

	std::getline(std::cin, encryptionKey);

	std::cout << std::endl;

	std::cout << "Enter the Hex: ";
	std::getline(std::cin, hex);
	std::cout << std::endl;

	// Encryptions Details //
	/*
	Algorithim aes-256-gcm
	Iv Length 16 Bytes
	Tag Length 16 Bytes
	Encoding (hex)
	Salt length 64 Bytes
	PBKDF2 Iterations 100,000

	Discord-Tickets Function Constructor uses these Paramaters: 
	--> 	const cryptr = new Cryptr(process.env.ENCRYPTION_KEY); // NODE.JS

	They do not pass in any Special Options. so the defaults are used (shown above).
	If encryption is disabled the encryption function is binded to return itself, If you don't understand, heres an example (Node.js):
	function returnparams(...) {
		return ...;
	};
	*/


	std::cout << "-------------------------------------------\n";
	std::cout << "Discord Tickets Fast Dump (aes-256-gcm)\n";
	std::cout << "Encryption Key: " << encryptionKey << "\n";
	std::cout << "Hex: " << hex << "\n";
	std::cout << "-------------------------------------------\n";

	std::cout << "DUMPING DATABASE WITH ENC KEY..... \n \n\n\n\n";
	
	std::string result;
	if (decryptCryptr(hex, encryptionKey, result))
	{
		std::cout << "Decrypted Result: \n" << result << "\n";
	}
	else
	{
		std::cout << "Decryption failed. Please check the encryption key and hex input.\n";
	}


}