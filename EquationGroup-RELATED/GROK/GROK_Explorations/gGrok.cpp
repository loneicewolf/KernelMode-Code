#include "gCommon.hpp"

#pragma warning(push)
#pragma warning(disable: 4311 4302)
NTSTATUS GROK::hidden::QuerySysInfo(
    ULONG Class, 
    PVOID Buffer, 
    SIZE_T BufferSize, 
    SIZE_T * RequiredSize, 
    PVOID QueryInfo
)
{
    return (NTSTATUS) ASM_HiddenCall(4,
                                     (PVOID) Class,
                                     (PVOID) Buffer,
                                     (PVOID) BufferSize,
                                     (PVOID) RequiredSize,
                                     (PVOID) QueryInfo,
                                     (PVOID) ASM_CallRsi);
}

PVOID GROK::hidden::Alloc(
    POOL_TYPE PoolType, 
    SIZE_T BufferSize, 
    PVOID Alloc
)
{
    return ASM_HiddenCall(2,
                          (PVOID) PoolType,
                          (PVOID) BufferSize,
                          (PVOID) Alloc,
                          (PVOID) ASM_CallRsi,
                           nullptr, nullptr);
}

VOID GROK::hidden::Free(
    PVOID Buffer, 
    PVOID Free
)
{
    return (VOID) ASM_HiddenCall(1,
                                 (PVOID) Buffer,
                                 (PVOID) Free,
                                 (PVOID) ASM_CallRsi,
                                  nullptr, nullptr, nullptr);
}
#pragma warning(pop)

///<summary>
/// for the dummy test of "hidden" routines
///</summary>
struct SYSTEM_MODULE_ENTRY_INFORMATION
{
    HANDLE 	Section;
    PVOID 	MappedBase;
    PVOID 	ImageBase;
    ULONG 	ImageSize;
    ULONG 	Flags;
    USHORT 	LoadOrderIndex;
    USHORT 	InitOrderIndex;
    USHORT 	LoadCount;
    USHORT 	OffsetToFileName;
    UCHAR 	FullPathName[256];
};
struct SYSTEM_MODULES_INFORMATION
{
    ULONG NumberOfModules;
    SYSTEM_MODULE_ENTRY_INFORMATION Modules[1];
};

///<summary>
///"hidden" routines GROK uses
///</summary>
using QUERY_INFO = NTSTATUS(*)(ULONG, PVOID, SIZE_T, PSIZE_T);
using POOL_ALLOC = PVOID(*)(POOL_TYPE, SIZE_T);
using POOL_FREE  = VOID(*)(PVOID);
using DBG_PRINT  = VOID(*)(LPCSTR, ...);

///<summary>
///display loaded kernel modules and their base name for generic POC
///using the "hidden" routines.  Called from the manually mapped buffer
///</summary>
NTSTATUS GROK::Explorations(ULONG_PTR NtosBase)
{
    auto INTERNAL_GetFunctionFromHashDirty = [&NtosBase](INT32 FunctionHash) -> PVOID
    {
        auto NtHeader  = (PIMAGE_NT_HEADERS) (NtosBase + ((PIMAGE_DOS_HEADER) NtosBase)->e_lfanew);
        auto ExportDir = (PIMAGE_EXPORT_DIRECTORY) (NtosBase + NtHeader->OptionalHeader.DataDirectory[0].VirtualAddress);
        if (!ExportDir)
            return nullptr;

        auto Fns   = (PULONG) (NtosBase + ExportDir->AddressOfFunctions);
        auto Names = (PULONG) (NtosBase + ExportDir->AddressOfNames);
        auto Ords  = (PUSHORT) (NtosBase + ExportDir->AddressOfNameOrdinals);

        PVOID RoutineAddress {};
        for (UINT32 i {}; i < ExportDir->NumberOfNames; ++i)
        {
            auto NameOfRoutine = (PCHAR) (NtosBase + Names[i]);
            if (FunctionHash == ASM_HashExportedFn(NameOfRoutine))
            {
                RoutineAddress = (PVOID) (NtosBase + Fns[Ords[i]]);
                break;
            }
        }

        return RoutineAddress;
    };

    auto GROK_QueryInfo = (QUERY_INFO) INTERNAL_GetFunctionFromHashDirty(0x212FB41E);
    auto GROK_Alloc     = (POOL_ALLOC) INTERNAL_GetFunctionFromHashDirty(0xE016F24C);
    auto GROK_Free      = (POOL_FREE)  INTERNAL_GetFunctionFromHashDirty(0xCBD0FC0D);
    auto GROK_DbgPrint  = (DBG_PRINT)  INTERNAL_GetFunctionFromHashDirty(0x0E1749260);
    if (!GROK_QueryInfo || !GROK_Alloc || !GROK_Free)
        return STATUS_UNSUCCESSFUL;


    SIZE_T bufferSize {};
    auto status    = hidden::QuerySysInfo(0xB, nullptr, 0, &bufferSize, GROK_QueryInfo);
    auto driverInfo = (SYSTEM_MODULES_INFORMATION*) hidden::Alloc(NonPagedPoolNx, 
                                                                  bufferSize, 
                                                                  GROK_Alloc);
    if (!driverInfo)
        return STATUS_INSUFFICIENT_RESOURCES;

    status = hidden::QuerySysInfo(0xB, 
                                  driverInfo, 
                                  bufferSize, 
                                  nullptr, 
                                  GROK_QueryInfo);
    if (NT_SUCCESS(status))
    {
        for (ULONG i {}; i < driverInfo->NumberOfModules; ++i)
        {
            auto baseOffset = driverInfo->Modules[i].OffsetToFileName;
            auto driverName = (LPCSTR) &driverInfo->Modules[i].FullPathName[baseOffset];
            GROK_DbgPrint("--[ 0x%p | %s\n", driverInfo->Modules[i].ImageBase, driverName);
        }
    }
    hidden::Free(driverInfo, GROK_Free);

    return status;
}

#pragma warning( push )
#pragma warning( disable: 4505 )
///<summary>
///helper function for GROK::CreateHiddenDriver
///to manually resolve imports for the "hidden" driver
///Technically not necessary since no imports are used 
///in GROK::Explorations() but would be obviously be necessary if it were
///</summary>
static NTSTATUS ResolveNTOSImports(
    PVOID DriverImgBase,
    PVOID ModuleBaseHelper
)
{
    auto INTERNAL_GetProcAddress = [](PVOID ModuleBase, LPCSTR Name) -> ULONG_PTR
    {
        auto ntHeader = UNSAFE_GET_NT_HEADERS(ModuleBase);
        if (!ntHeader || ntHeader->Signature != IMAGE_NT_SIGNATURE)
            return 0;

        auto exportDirRVA  = ntHeader->OptionalHeader.DataDirectory[0].VirtualAddress;
        auto exportDirSize = ntHeader->OptionalHeader.DataDirectory[0].Size;
        if (!exportDirRVA || !exportDirSize)
            return 0;

        auto exportDir = PTR_TO_TYPE(IMAGE_EXPORT_DIRECTORY, ModuleBase, exportDirRVA);
        auto Fns   = PTR_TO_TYPE(ULONG, ModuleBase, exportDir->AddressOfFunctions);
        auto Names = PTR_TO_TYPE(ULONG, ModuleBase, exportDir->AddressOfNames);
        auto Ords  = PTR_TO_TYPE(USHORT, ModuleBase, exportDir->AddressOfNameOrdinals);

        ULONG Last { exportDir->NumberOfNames }, First {}, Middle {};
        signed int result;
        while (First <= Last)
        {
            Middle = (First + Last) / 2;
            auto routineName = PTR_TO_TYPE(char const, ModuleBase, Names[Middle]);
            result = ASM_StriCmp(routineName, Name);
            if (result == 0)        return (ULONG_PTR) PTR_TO_OFFSET(ModuleBase, Fns[Ords[Middle]]);
            else if (result > 0)    Last = Middle - 1;
            else                    First = Middle + 1;
        }

        return 0;
    };

    ULONG_PTR moduleBase {};
    if (!DriverImgBase || ((PIMAGE_DOS_HEADER) DriverImgBase)->e_magic != IMAGE_DOS_SIGNATURE)
        return STATUS_UNSUCCESSFUL;

    auto ntHeader = UNSAFE_GET_NT_HEADERS(DriverImgBase);
    if (!ntHeader || ntHeader->Signature != IMAGE_NT_SIGNATURE)
        return STATUS_UNSUCCESSFUL;

    auto impDescRVA  = ntHeader->OptionalHeader.DataDirectory[1].VirtualAddress;
    auto impDescSize = ntHeader->OptionalHeader.DataDirectory[1].Size;
    if (!impDescRVA || !impDescSize)
        return STATUS_UNSUCCESSFUL;

    auto impDesc    = PTR_TO_TYPE(IMAGE_IMPORT_DESCRIPTOR, DriverImgBase, impDescRVA);
    auto endImpDesc = PTR_TO_TYPE(IMAGE_IMPORT_DESCRIPTOR, DriverImgBase, (impDescRVA + impDescSize));

    while (impDesc < endImpDesc && impDesc->Name)
    {
        auto impINT     = PTR_TO_TYPE(IMAGE_THUNK_DATA64, DriverImgBase, impDesc->FirstThunk);
        auto impNAME    = PIMAGE_IMPORT_BY_NAME {};
        auto impIAT     = PTR_TO_TYPE(IMAGE_THUNK_DATA64, DriverImgBase, impDesc->OriginalFirstThunk);
        auto currentFn  = ULONG_PTR {};
        auto moduleName = PTR_TO_TYPE(char const, DriverImgBase, impDesc->Name);
        if (ASM_StriCmp(moduleName, "ntoskrnl.exe") == 0)
        {
            ASM_LocateImageBaseFromRoutine(ModuleBaseHelper, 0x1000, &moduleBase);
            if (moduleBase)
            {
                while (impINT->u1.AddressOfData)
                {
                    impNAME = PTR_TO_TYPE(IMAGE_IMPORT_BY_NAME,
                                          DriverImgBase,
                                          impINT->u1.AddressOfData);
                    
                    currentFn = INTERNAL_GetProcAddress((PVOID) moduleBase, impNAME->Name);
                    if (!currentFn)
                        return STATUS_UNSUCCESSFUL;
                    impIAT->u1.Function = currentFn;
                    
                    
                    ++impINT, ++impNAME;
                }
            }
        }
        ++impDesc;
    }

    return STATUS_SUCCESS;
}
#pragma warning( pop )


#define FREE_BUFFER_RETURN_STATUS(buffer)   ExFreePool(buffer); return status

#pragma optimize( "", off )
///<summary>
///create new driver buffer and transfer execution to it
///</summary>
NTSTATUS GROK::CreateHiddenDriver(
    bool OnOff,
    PUCHAR DecompressedDriverBuf
)
{
    auto status   = STATUS_UNSUCCESSFUL;
    bool switchOn = OnOff;
    auto resolveNtosBaseHelper = (PVOID) NtQueryDirectoryFile;
    PUCHAR ogDriverImgBuf { DecompressedDriverBuf };

    if (!ogDriverImgBuf) 
    { 
        FREE_BUFFER_RETURN_STATUS(ogDriverImgBuf);
    }

    auto ogAddrOfRtrnAddress = _AddressOfReturnAddress();
    ULONG_PTR ogDriverBase {};
    ASM_LocateImageBaseFromRoutine(*(ULONGLONG**) ogAddrOfRtrnAddress,
                                   0x400,
                                   &ogDriverBase);

    if (!ogDriverBase) 
    { 
        FREE_BUFFER_RETURN_STATUS(ogDriverImgBuf);
    }

    auto ntHeader = UNSAFE_GET_NT_HEADERS(ogDriverImgBuf);
    if (!ntHeader || ntHeader->Signature != IMAGE_NT_SIGNATURE) 
    { 
        FREE_BUFFER_RETURN_STATUS(ogDriverImgBuf);
    }

    ULONG driverImageSize = 0x7000; // { ntHeader->OptionalHeader.SizeOfImage };

    auto hiddenDriver = ExAllocatePool(NonPagedPool, driverImageSize);
    if (!hiddenDriver) 
    { 
        FREE_BUFFER_RETURN_STATUS(ogDriverImgBuf);
    }

    ::RtlSecureZeroMemory(hiddenDriver, driverImageSize);
    ::memcpy_s(hiddenDriver, driverImageSize, ogDriverImgBuf, driverImageSize);

    auto section = IMAGE_FIRST_SECTION(ntHeader);
    for (ULONG i {}; i < ntHeader->FileHeader.NumberOfSections; ++i)
    {
        auto sizeOfRawData = section[i].SizeOfRawData;
        auto virtualSize = section[i].Misc.VirtualSize;
        ::memcpy(PTR_TO_OFFSET(hiddenDriver, section[i].VirtualAddress),
                 PTR_TO_OFFSET(ogDriverImgBuf, section[i].PointerToRawData),
                 sizeOfRawData > virtualSize ? sizeOfRawData : virtualSize);
    }

    ExFreePool(ogDriverImgBuf);

    status = ASM_FixRelocations(hiddenDriver, driverImageSize);
    if (!NT_SUCCESS(status))
        return status;

    auto nextAddr = (ULONG_PTR) ASM_GetNextIPAddress();
    if (switchOn)
    {
        /* can only be reached in new memory buffer 
         *  - this is where GROK resolves imports and finishes up
         *  status = ResolveNTOSImports(hiddenDriver, resolveNtosBaseHelper);
         *
         */
        
        ULONG_PTR ntosBase {};
        ASM_LocateImageBaseFromRoutine(resolveNtosBaseHelper, 0x1000, &ntosBase);
        if (ntosBase)
            status = GROK::Explorations(ntosBase);

    } 
    else
    {
        switchOn = true;

        auto routineToTransferCtrlToOffset = nextAddr - ogDriverBase;
        auto newRtrnAddrOffset = *(ULONG_PTR*) ogAddrOfRtrnAddress - ogDriverBase;
        *(ULONG_PTR*) ogAddrOfRtrnAddress = ((ULONG_PTR) hiddenDriver + newRtrnAddrOffset);

        ASM_MemorySwitch((PVOID) ((ULONG_PTR) hiddenDriver + routineToTransferCtrlToOffset));

    }

    return status;
}
#pragma optimize( "", on)

#undef FREE_BUFFER_RETURN_STATUS