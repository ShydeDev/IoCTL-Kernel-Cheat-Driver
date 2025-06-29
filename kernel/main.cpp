#include <ntifs.h>
#include <intrin.h>
#include "SDK/driver.hpp"

UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\TaylorsDriver");
UNICODE_STRING symbolicLink = RTL_CONSTANT_STRING(L"\\??\\TaylorsDriver");
UNICODE_STRING driverName = RTL_CONSTANT_STRING(L"\\Driver\\TaylorsDriver");

EXTERN_C
PLIST_ENTRY PsLoadedModuleList;

typedef struct _KLDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	PVOID DllBase;
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;

NTSTATUS actualEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath) {
	UNREFERENCED_PARAMETER(registryPath);

	// Variables
	PDEVICE_OBJECT deviceObject = nullptr;

	// Create Device
	const NTSTATUS status = IoCreateDevice(
		driverObject, 0, 
		&deviceName, 
		FILE_DEVICE_UNKNOWN, 
		FILE_DEVICE_SECURE_OPEN, 
		FALSE, 
		&deviceObject
	);

	if (!NT_SUCCESS(status))
		return status;

	// Establish Symbolic Link
	if (!NT_SUCCESS(IoCreateSymbolicLink(const_cast<PUNICODE_STRING>(&symbolicLink), const_cast<PUNICODE_STRING>(&deviceName))))
		return STATUS_UNSUCCESSFUL;

	deviceObject->Flags |= DO_BUFFERED_IO; // Use buffered IO for IRP's

	// Setup Handlers
	driverObject->MajorFunction[IRP_MJ_CREATE] = driver::handleCreate;
	driverObject->MajorFunction[IRP_MJ_CLOSE] = driver::handleClose;
	driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driver::handleDeviceControl;
	driverObject->DriverUnload = [](PDRIVER_OBJECT drvObj) {
		IoDeleteSymbolicLink(const_cast<PUNICODE_STRING>(&symbolicLink));
		if (drvObj->DeviceObject)
			IoDeleteDevice(drvObj->DeviceObject);
	};

	deviceObject->Flags &= ~DO_DEVICE_INITIALIZING; // Clear this flag to ENABLE sending IO requests to the device

	// Unlink from PsLoadedModuleList
	// https://gist.github.com/muturikaranja/b7d4b59c72611e76aed94b2f0bf33aa2
	// https://github.com/DErDYAST1R/PsLoadedModuleList-Dkom-Unlinking

	PLIST_ENTRY current = PsLoadedModuleList->Flink;

	while (current != PsLoadedModuleList) {
		PKLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(current, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (entry->DllBase == driverObject->DriverStart) {
			KIRQL oldIrql = KeRaiseIrqlToDpcLevel();

			entry->InLoadOrderLinks.Blink->Flink = entry->InLoadOrderLinks.Flink;
			entry->InLoadOrderLinks.Flink->Blink = entry->InLoadOrderLinks.Blink;

			entry->InLoadOrderLinks.Flink = (PLIST_ENTRY)entry;
			entry->InLoadOrderLinks.Blink = (PLIST_ENTRY)entry;

			KeLowerIrql(oldIrql);
			break;
		}

		current = current->Flink;
	}

	return status;
}

NTSTATUS DriverEntry() {
	return IoCreateDriver(&driverName, reinterpret_cast<PDRIVER_INITIALIZE>(actualEntry));
}