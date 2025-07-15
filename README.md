# DiscordTicketsFastDump

A simple script to dump the Discord Tickets database (encrypted) to a `.zip` file. It is faster than Node.js (benchmarked as about 20 times faster), especially for larger databases.

## Dependencies
	
* [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview)
* [OpenSSL](https://github.com/openssl/openssl)
* [SQLiteCpp](https://github.com/SRombauts/SQLiteCpp)

## How to Use

> **IMPORTANT:** If you encounter errors during `vcpkg install`, delete the entire `vcpkg_installed` folder and try again.

1. Download the **Latest Release** from the **Releases** tab (Source Code).
2. Extract the `.zip` file.
3. Use `vcpkg install` to install the dependencies. (Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview) if you don't have it)
4. Build. Only tested with the MSVC Compiler.
5. Run the produced executable.