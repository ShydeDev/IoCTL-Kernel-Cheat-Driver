#pragma once

#include <windows.h>
#include <string_view>
#include <iostream>

class memory {
private:
	HANDLE processHandle = nullptr;
	static constexpr DWORD processAccess = PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION;

public:
	uintptr_t processId = 0;

	explicit memory(std::string_view processName) noexcept;
	~memory() noexcept;

	memory(const memory&) = delete;
	memory& operator=(const memory&) = delete;
	memory(memory&& other) noexcept;
	memory& operator=(memory&& other) noexcept;

	[[nodiscard]] uintptr_t GetModuleBaseAddress(std::wstring_view moduleName) const noexcept;
};

namespace driver {
	namespace codes {
		constexpr ULONG attach = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
		constexpr ULONG read = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
		constexpr ULONG write = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
	}

	struct Request {
		HANDLE processId;
		PVOID target;
		PVOID buffer;
		SIZE_T size;
		SIZE_T returnSize;
	};

	bool attach(HANDLE driverHandleArg, const DWORD processId);

	template<class T>
	T read(uintptr_t address);

	template<class T>
	void write(uintptr_t address, const T& value);
}
