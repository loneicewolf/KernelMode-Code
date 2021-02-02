/* quick, dirty routines that can be used in other parts of code.
   Some in fact do rely on other parts, but their intent is obvious.
*/


HANDLE
kGetPidFromName(
	PCWSTR PsName
)
{
	ULONG BufSize {};
	HANDLE Pid {};
	auto Status = ZwQuerySystemInformation(SystemProcessInformation,
					       nullptr,
					       0,
					       &BufSize);
	if (Status != STATUS_INFO_LENGTH_MISMATCH){
		dprintf("0x%08X - ZwQuerySystemInfo\n", Status);
		return Pid;
	}
	auto PsInfo = (PSYSTEM_PROCESS_INFORMATION) ExAllocatePoolWithTag(NonPagedPoolNx,
									  BufSize,
									  KEXP_TAG);
	if (!PsInfo)
		return Pid;

	Status = ZwQuerySystemInformation(SystemProcessInformation,
	                                  PsInfo,
					  BufSize,
					  nullptr);
	if (!NT_SUCCESS(Status)) {
		dprintf("0x%08X - ZwQuerySystemInfo\n", Status);
		ExFreePoolWithTag(PsInfo, KEXP_TAG);
		return Pid;
	}
	
	auto OriginalPsInfo = PsInfo;
	UNICODE_STRING ProcessName;
	RtlInitUnicodeString(&ProcessName, PsName);
	
	while (PsInfo->NextEntryOffset) {
		if (PsInfo->ImageName.Buffer != nullptr) {
			if (RtlCompareUnicodeString(&PsInfo->ImageName, &ProcessName, TRUE) == 0){
				Pid = PsInfo->UniqueProcessId;
				break;
			}
		}
		PsInfo = (PSYSTEM_PROCESS_INFORMATION) ((ULONG_PTR) PsInfo + PsInfo->NextEntryOffset);
	}

	ExFreePoolWithTag(OriginalPsInfo, KEXP_TAG);
	return Pid;
}


/* parsing the PEB to get k32 base.  Though not done here, accesing the PEB 
   user address (like any user-mode VAS from kernel mode) needs to be
   probed for in try/execept block */
PVOID
kGetK32BaseAddress()
{
	auto Pid = kGetPidFromName(L"csrss.exe");
	if (!Pid)
		return nullptr;
	
	PEPROCESS Process {};
	auto Status = PsLookupProcessByProcessId(Pid, &Process);
	if (!NT_SUCCESS(Status))
		return nullptr;
	auto Peb = (__PEB*) PsGetProcessPeb(Process);
	if (!Peb)
		return nullptr;

	KAPC_STATE ApcState {};
	PVOID k32Base {};
	UNICODE_STRING k32 = RTL_CONSTANT_STRING(L"kernel32.dll");

	KeStackAttachProcess((PRKPROCESS) Process, &ApcState);
	/* not safely accessed for demo purposes */
	auto CurrentEntry = Peb->Ldr->InLoadOrderModuleList.Flink;
	LDR_DATA_TABLE_ENTRY* Current {};
	while (CurrentEntry != &Peb->Ldr->InLoadOrderModuleList && CurrentEntry != nullptr) {
		Current = CONTAINING_RECORD(CurrentEntry,
                                            LDR_DATA_TABLE_ENTRY,
                                            InLoadOrderLinks);
		if (RtlCompareUnicodeString(&k32, &Current->BaseDllName, FALSE) == 0) {
			k32Base = Current->DllBase;
			break;
		}
		CurrentEntry = CurrentEntry->Flink;
	}
	
	KeUnstackDetachProcess(&ApcState);
	
	dprintf("0x%p - Kernel32.dll\n", k32Base);
	
	return k32Base;
}


VOID
ImageCallback(
	PUNICODE_STRING ImageName,
	HANDLE Pid,
	PIMAGE_INFO ImageInfo
)
{
	if (!Pid)
		return;
	UNICODE_STRING k32 = RTL_CONSTANT_STRING(L"\\Windows\\System32\\ntdll.dll");
	if (RtlCompareUnicodeString(ImageName, &k32, TRUE) == 0) {
		dprintf("0x%p: %wZ \n", ImageInfo->ImageBase, ImageName);
	}

}

#define KNOWN_DLLS_PATH			L"\\KnownDlls\\"
#define SYSTEM_PATH			L"\\SystemRoot\\System32\\"
#define KERNEL32			L"kernel32.dll"
#define NTDLL				L"ntdll.dll"


enum class SYSTEM_DLL
{
	ntdll,
	kernel32
};

PVOID
kGetSystemDllBase(
	SYSTEM_DLL Module
)
{	
	PVOID DllBase {};
	HANDLE SectionHandle;
	UNICODE_STRING SectionName;
	wchar_t ModulePath[100];

	switch (Module) {
		case SYSTEM_DLL::ntdll:
		{
			RtlInitUnicodeString(&SectionName, KNOWN_DLLS_PATH NTDLL);
			wcscpy(ModulePath, SYSTEM_PATH NTDLL);
			break;
		}
		case SYSTEM_DLL::kernel32:
		{
			RtlInitUnicodeString(&SectionName, KNOWN_DLLS_PATH KERNEL32);
			wcscpy(ModulePath, SYSTEM_PATH KERNEL32);
			break;
		}
		default:
			return nullptr;
	}


	OBJECT_ATTRIBUTES SectionAttrs;
	InitializeObjectAttributes(&SectionAttrs,
                                   &SectionName,
                                   OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
                                   nullptr,
                                   nullptr);

	auto Status = ZwOpenSection(&SectionHandle,
                                    SECTION_QUERY,
                                    &SectionAttrs);
	if (!NT_SUCCESS(Status)) {
		dprintf("0x%08X - ZwOpenSection()\n", Status);
		return DllBase;
	}

	SECTION_IMAGE_INFORMATION SecImageInfo {};
	Status = ZwQuerySection(SectionHandle,
                                SectionImageInformation,
                                &SecImageInfo,
                                sizeof(SecImageInfo),
                                nullptr);

	ZwClose(SectionHandle);
	if (!NT_SUCCESS(Status)) {
		dprintf("0x%08X - ZwQuerySection()\n", Status);
		return DllBase;
	}


	PUCHAR ModuleBase {};
	ULONG ModuleSize {};
        /* load the dll into system buffer, needs to be freed */
	Status = KExplorer::kOpenFile(ModulePath,
                                      &ModuleBase,
                                      &ModuleSize);
	if (NT_SUCCESS(Status)) {
		auto Dos = (PIMAGE_DOS_HEADER) ModuleBase;
		auto Nt = (PIMAGE_NT_HEADERS) (ModuleBase + Dos->e_lfanew);
		DllBase = (PVOID) ((ULONG_PTR) SecImageInfo.TransferAddress - Nt->OptionalHeader.AddressOfEntryPoint);

		ExFreePoolWithTag(ModuleBase, KEXP_TAG);
	}

	return DllBase;
}
