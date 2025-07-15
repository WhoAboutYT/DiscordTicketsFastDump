#include <iostream>
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

int main()
{
	std::string encryptionKey;
	std::string dbFilePath;

	std::cout << "Enter the encryption key to the database: ";

	std::getline(std::cin, encryptionKey);

	std::cout << std::endl;

	std::cout << "Enter the sqlite database file path: ";
	std::getline(std::cin, dbFilePath);
	std::cout << std::endl;

	std::cout << "-------------------------------------------\n";
	std::cout << "Encryption Key: " << encryptionKey << "\n";
	std::cout << "Database File Path: " << dbFilePath << "\n";
	std::cout << "-------------------------------------------\n";

	std::cout << "DUMPING DATABASE WITH ENC KEY..... \n \n\n\n\n";


}