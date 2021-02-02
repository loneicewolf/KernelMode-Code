#pragma once

#pragma warning(disable: 5030)
#pragma warning(push)
#pragma warning(disable:4201 5040)  
#include <ntifs.h>
#include <ntimage.h>
#include <intrin.h>
#pragma warning(pop)

#define GROK_NAME           L"Grok"
#define GROK_DRIVERNAME	    L"Grok.sys"
#define ALLOC_TAG           'KORG'


#define dprintf(Format, ...)	DbgPrint("[GROK] " Format, __VA_ARGS__)
#define DEBUG_BREAK
#if defined(DEBUG_BREAK)
#define DBG_BREAK   __debugbreak()
#else
#define DBG_BREAK   __noop
#endif

#define UNSAFE_GET_NT_HEADERS(base)    \
    ((PIMAGE_NT_HEADERS)((ULONG_PTR)(base) + ((PIMAGE_DOS_HEADER)(base))->e_lfanew))

#define PTR_TO_OFFSET(ptr, offset)  (PVOID)((ULONG_PTR)(ptr) + (offset))

#define PTR_TO_TYPE(type, ptr, offset)   \
    ((type*) PTR_TO_OFFSET((ptr), (offset)))

extern "C" {

    VOID  ASM_CallRsi();
    PVOID ASM_GetNextIPAddress();

    VOID  ASM_MemorySwitch(
        PVOID AddressToSwitchTo
    );

    PVOID ASM_HiddenCall(
        ULONG NumOfArgs,
        PVOID Arg1,
        PVOID Arg2,
        PVOID Arg3,
        PVOID Arg4,
        PVOID Arg5,
        PVOID Arg6);

    INT32 ASM_HashExportedFn(PCHAR);

    VOID ASM_LocateImageBaseFromRoutine(
        PVOID Routine,
        ULONG Size,
        ULONG_PTR* OutResult);

    NTSTATUS ASM_FixRelocations(
        PVOID ImageBase,
        ULONG SizeOfImage);

    INT ASM_StriCmp(char const*, char const*);
}


namespace GROK {
    
    extern UNICODE_STRING uDeviceName;
    extern UNICODE_STRING uDosDeviceName;
    extern PDRIVER_OBJECT DriverObj;
    extern PDEVICE_OBJECT DeviceObj;


    NTSTATUS DispatchHandler(PDEVICE_OBJECT, PIRP);
    NTSTATUS Explorations(ULONG_PTR NtosBase);
    NTSTATUS CreateHiddenDriver(bool Switch, PUCHAR DecompressedDriver);

    struct HIDDEN_DRIVER_INFO
    {
        ULONG  DriverSize;
        ULONG  FinalSize;
        PUCHAR CompressedDriver;
    };

}

namespace GROK::hidden {

    NTSTATUS QuerySysInfo(
        ULONG Class,
        PVOID Buffer,
        SIZE_T BufferSize,
        SIZE_T* RequiredSize,
        PVOID QueryInfo
    );


    PVOID Alloc(
        POOL_TYPE PoolType,
        SIZE_T BufferSize,
        PVOID Alloc
    );

    VOID Free(
        PVOID Buffer,
        PVOID Free
    );
}
