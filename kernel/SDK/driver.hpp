#pragma once

#include <ntifs.h>
extern "C" {
	NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
	NTSTATUS MmCopyVirtualMemory(
		PEPROCESS fromProcess, 
		PVOID fromAddress, 
		PEPROCESS toProcess, 
		PVOID toAddress, 
		SIZE_T bufferSize,
		KPROCESSOR_MODE previousMode, 
		PSIZE_T bytesCopied
	);
}

namespace driver {
	 // https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes 
	enum class IoCTL : ULONG {
		 Attach = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS),
		 Read = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS),
		 Write = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
	};

	struct Request {
		HANDLE processId;

		PVOID target;
		PVOID buffer;

		SIZE_T size;
		SIZE_T returnSize;
	};

	NTSTATUS handleCreate(PDEVICE_OBJECT deviceObject, PIRP irp);
	NTSTATUS handleClose(PDEVICE_OBJECT deviceObject, PIRP irp);
	NTSTATUS handleDeviceControl(PDEVICE_OBJECT deviceObject, PIRP irp);
}