#include "memory.hpp"
#include <tlhelp32.h>

static HANDLE driverHandle = nullptr;

bool driver::attach(HANDLE driverHandleArg, const DWORD processId) {
	driverHandle = driverHandleArg;

	Request request{};
	request.processId = reinterpret_cast<HANDLE>(processId);

	return DeviceIoControl(driverHandle, codes::attach, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr);
}

template<class T>
T driver::read(uintptr_t address) {
	T buffer{};
	Request request{};

	request.target = reinterpret_cast<PVOID>(address);
	request.buffer = &buffer;
	request.size = sizeof(T);

	DeviceIoControl(driverHandle, codes::read, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr);
	return buffer;
}

template<class T>
void driver::write(uintptr_t address, const T& value) {
	Request request{};

	request.target = reinterpret_cast<PVOID>(address);
	request.buffer = (PVOID)&value;
	request.size = sizeof(T);

	DeviceIoControl(driverHandle, codes::write, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr);
}

// force template instantiation
template int     driver::read<int>(uintptr_t);
template float   driver::read<float>(uintptr_t);
template uintptr_t driver::read<uintptr_t>(uintptr_t);
template unsigned int driver::read<unsigned int>(uintptr_t address);

template void driver::write<unsigned int>(uintptr_t address, const unsigned int& value);
template void driver::write<int>(uintptr_t, const int&);
template void driver::write<float>(uintptr_t, const float&);
template void driver::write<uintptr_t>(uintptr_t, const uintptr_t&);

memory::memory(std::string_view processName) noexcept {
	PROCESSENTRY32 pe32{};
	pe32.dwSize = sizeof(pe32);

	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE)
		return;

	if (Process32First(snapshot, &pe32)) {
		do {
			if (processName == pe32.szExeFile) {
				processId = pe32.th32ProcessID;
				processHandle = OpenProcess(processAccess, FALSE, processId);
				break;
			}
		} while (Process32Next(snapshot, &pe32));
	}
	CloseHandle(snapshot);
}

memory::~memory() noexcept {
	if (processHandle) CloseHandle(processHandle);
}

memory::memory(memory&& other) noexcept
	: processId(other.processId), processHandle(other.processHandle) {
	other.processHandle = nullptr;
	other.processId = 0;
}

memory& memory::operator=(memory&& other) noexcept {
	if (this != &other) {
		if (processHandle) CloseHandle(processHandle);
		processId = other.processId;
		processHandle = other.processHandle;
		other.processHandle = nullptr;
		other.processId = 0;
	}
	return *this;
}

uintptr_t memory::GetModuleBaseAddress(std::wstring_view moduleName) const noexcept {
	if (!processHandle) return 0;

	uintptr_t moduleBaseAddress = 0;
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
	if (snapshot == INVALID_HANDLE_VALUE) return 0;

	MODULEENTRY32W moduleEntry{};
	moduleEntry.dwSize = sizeof(moduleEntry);

	if (Module32FirstW(snapshot, &moduleEntry)) {
		do {
			if (_wcsicmp(moduleEntry.szModule, moduleName.data()) == 0) {
				moduleBaseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
				break;
			}
		} while (Module32NextW(snapshot, &moduleEntry));
	}
	CloseHandle(snapshot);
	return moduleBaseAddress;
}
