#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>
#include "ThreadPool.hpp"
#include "utility.h"

using json = nlohmann::json;

int main()
{
    std::string encryptionKey;
    std::string encryptedFile;

    std::cout << "Enter the encryption key to the database: ";

    std::getline(std::cin, encryptionKey);
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

    std::vector<std::string> decryptedLines(lineCount); // MUST BE HERE!!

    lineCount = 0;

	// - This reopens the file - //

    fileContentsStream.clear();
    fileContentsStream.seekg(0);

	// While loop below is to actually decode the lines

    std::vector<std::future<void>> processingFutures;
    std::mutex coutMutex;
    auto totalStart = std::chrono::high_resolution_clock::now();

	/*
	JSONL is for making json objects but faster (? jsons/per line, not sure)
	Encoded Variables:
	content
	avatar
	displayName
	username
	comment
	value
	topic */

    while (std::getline(fileContentsStream, jsonLine)) 
    {
        lineCount++;
        if (jsonLine.empty()) 
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "[!!] Detected Empty Line At " << lineCount << "\n";
            continue;
        }

        const size_t currentLine = lineCount;
        const std::string lineCopy = jsonLine;

        processingFutures.emplace_back(ThreadPool->Run([=, &coutMutex, &decryptedLines]() {
            auto lineStart = std::chrono::high_resolution_clock::now();

            try
            {
                json data = json::parse(lineCopy);
                decryptNestedFields(data, encryptionKey);

                auto lineEnd = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(lineEnd - lineStart);

                decryptedLines[currentLine - 1] = data.dump();

                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "Line #" << currentLine << " decrypted in: " << duration.count() << " ms\n";
            }
            catch (const nlohmann::json_abi_v3_12_0::detail::type_error& e)
            {
                std::lock_guard<std::mutex> lock(coutMutex);
				std::cerr << "[!] Error processing line " << currentLine << ": " << e.what() << "\n"; // !! Important to catch this specific error !! //
            }
            catch (const std::exception& e)
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cerr << "[!] General error processing line " << currentLine << ": " << e.what() << "\n";
            }
            }));
    }

    for (auto& future : processingFutures)
    {
        future.wait();
    }

    std::ofstream outFile("decrypted_output.jsonl");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file for writing.\n";
        return 1;
    }

    for (const auto& line : decryptedLines) {
        outFile << line << "\n";
    }

    outFile.close();
    std::cout << "\nDecrypted output written to: decrypted_output.jsonl\n";
	fileContentsStream.close();

    auto totalEnd = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart);

    std::cout << "\nTotal processing time: " << totalDuration.count() << " milliseconds\n";
    std::cout << "Average time per line: " << (totalDuration.count() / lineCount) << " milliseconds\n";
}