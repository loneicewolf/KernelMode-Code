#include "gCommon.hpp"

extern "C" {
    DRIVER_INITIALIZE DriverEntry;
    NTSTATUS InitDevice();
}

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(INIT, InitDevice)


UNICODE_STRING GROK::uDeviceName;
UNICODE_STRING GROK::uDosDeviceName;
PDRIVER_OBJECT GROK::DriverObj;
PDEVICE_OBJECT GROK::DeviceObj;



NTSTATUS DriverEntry(
    PDRIVER_OBJECT DriverObj,
    PUNICODE_STRING
)
{
    GROK::DriverObj = DriverObj;

    auto status = ::InitDevice();
    if (!NT_SUCCESS(status))
    {
        dprintf("GROK::InitDevice failed.\n");
        return status;
    }

    GROK::DriverObj->DriverUnload = [](PDRIVER_OBJECT) -> VOID
    {
        IoDeleteSymbolicLink(&GROK::uDosDeviceName);
        IoDeleteDevice(GROK::DeviceObj);
    };

    return status;
}

NTSTATUS InitDevice()
{
    RtlInitUnicodeString(&GROK::uDeviceName, LR"(\Device\)" GROK_NAME);
    RtlInitUnicodeString(&GROK::uDosDeviceName, LR"(\??\)" GROK_NAME);

    auto status = IoCreateDevice(GROK::DriverObj,
                                 0,
                                 &GROK::uDeviceName,
                                 FILE_DEVICE_UNKNOWN,
                                 FILE_DEVICE_SECURE_OPEN,
                                 FALSE,
                                 &GROK::DeviceObj);
    if (!NT_SUCCESS(status))
    {
        dprintf("IoCreateDevice: 0x%08X [%d]\n", status, __LINE__);
        return status;
    }

    IoCreateSymbolicLink(&GROK::uDosDeviceName, &GROK::uDeviceName);

    GROK::DriverObj->MajorFunction[IRP_MJ_CREATE] =
        GROK::DriverObj->MajorFunction[IRP_MJ_CLOSE] =
        [](PDEVICE_OBJECT, PIRP Irp)
    {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    };

    GROK::DriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = GROK::DispatchHandler;


    return status;
}