#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include "SDK/Memory/memory.hpp"
#include "SDK/Data/data.hpp"
#include "SDK/Data/offsets.hpp"
#include "SDK/Helpers/helperFunctions.hpp"
#include "SDK/Cheats/cheats.hpp"

int main() {
	memory cs2("cs2.exe");

	const HANDLE driver = CreateFile("\\\\.\\TaylorsDriver", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (driver::attach(driver, cs2.processId)) {
		std::cout << "[+] Successfully attached to 0x" << cs2.processId << "!\n";
	}

	data::clientDll = cs2.GetModuleBaseAddress(L"client.dll");

	if (!data::clientDll || data::clientDll == 0) {
		std::cout << "[-] Failed to fetch client.dll\n";
		return 1;
	}

	std::cout << "[+] Extracted client.dll: " << "0x" << data::clientDll << '\n';
	data::localPlayer = driver::read<uintptr_t>(data::clientDll + offsets::dwLocalPlayer);

	std::vector<uintptr_t> entityVector;
	forEachEntity([&](uintptr_t entity, int i) {
		entityVector.push_back(entity);
	});

	std::cout << "Entity count: " << entityVector.size() << '\n';

	initBunnyHop();

	std::cout << "[+] Initialized bunnyhop!\n";
	CloseHandle(driver);
	std::cin.get();
}