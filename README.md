# DiscordTicketsFastDump

A simple script to dump the Discord Tickets database (encrypted) to a `.zip` file. It is faster than Node.js (benchmarked as about 20 times faster), especially for larger databases.

## Dependencies
	
* [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview)
* [OpenSSL](https://github.com/openssl/openssl)
* [json](https://github.com/nlohmann/json)

## How to Use - Building

> **IMPORTANT:** If you encounter errors during `vcpkg install`, delete the entire `vcpkg_installed` folder and try again.

1. Download the **Latest Release** from the **Releases** tab (Source Code).
2. Extract the `.zip` file.
3. Use `vcpkg install` to install the dependencies. (Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview) if you don't have it)
4. Build. Only tested with the MSVC Compiler.
5. Run the produced executable.

## How to Use - Running

For Releases, please do the following.

a. Modify your discord-tickets solution TICKETS worker to not decrypt, instead, to serve the encrypted as plaintext.

b. Once you do that, restart your bot and panel, and export. Take the tickets.jsonl file and put it inside of your working directory.

c. Run the executable, and complete the prompts correctly.

d. Enjoy! Once done, rename the file as tickets.jsonl, and re-create the WinRar/7-Zip archive and import the files.