#include "driver.hpp"
#include <optional>
using namespace driver;

namespace {
	inline NTSTATUS CompleteIrp(PIRP irp, NTSTATUS status, ULONG_PTR info = 0) {
		irp->IoStatus.Status = status;
		irp->IoStatus.Information = info;
		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return status;
	}
}

NTSTATUS driver::handleCreate(PDEVICE_OBJECT, PIRP irp) {
	return CompleteIrp(irp, STATUS_SUCCESS);
}

NTSTATUS driver::handleClose(PDEVICE_OBJECT, PIRP irp) {
	return CompleteIrp(irp, STATUS_SUCCESS);
}

NTSTATUS driver::handleDeviceControl(PDEVICE_OBJECT, PIRP irp) {
	const PIO_STACK_LOCATION irpStackLoc = IoGetCurrentIrpStackLocation(irp); // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_stack_location

	if (!irpStackLoc || !irp->AssociatedIrp.SystemBuffer)
		return CompleteIrp(irp, STATUS_INVALID_PARAMETER);

	const ULONG ioctlCode = irpStackLoc->Parameters.DeviceIoControl.IoControlCode;
	using driver::Request;
	Request* request = reinterpret_cast<Request*>(irp->AssociatedIrp.SystemBuffer); // Each IRP is sent to driver and has a stack of parameters called stack locations, different for each driver
	// The IoGetCurrentIrpStackLocation returns place where the driver can find the details of this request. When usermode sends an IOCTL with METHOD_BUFFERED transfer method, windows copies the io data into a system buffer that I access via irp->AssociatedIrp.SystemBuffer.
	static PEPROCESS targetProcess = nullptr;
	const driver::IoCTL controlCodeEnum = static_cast<driver::IoCTL>(ioctlCode);
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if (controlCodeEnum == IoCTL::Read || controlCodeEnum == IoCTL::Write)
		if (!targetProcess || !request->buffer || !request->target || request->size == 0)
			return CompleteIrp(irp, STATUS_INVALID_PARAMETER);

	switch (controlCodeEnum) {
		case IoCTL::Attach:
			status = PsLookupProcessByProcessId(request->processId, &targetProcess);
			break;

		case IoCTL::Read:
			status = MmCopyVirtualMemory(
				targetProcess,
				request->target,
				PsGetCurrentProcess(),
				request->buffer,
				request->size,
				KernelMode,
				&request->returnSize
			);
			break;

		case IoCTL::Write:
			status = MmCopyVirtualMemory(
				PsGetCurrentProcess(),
				request->buffer,
				targetProcess,
				request->target,
				request->size,
				KernelMode,
				&request->returnSize
			);
			break;

		default:
			status = STATUS_INVALID_DEVICE_REQUEST;
			break;
	}

	return CompleteIrp(irp, status, request->returnSize);
}
