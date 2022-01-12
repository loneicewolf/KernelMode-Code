#pragma once
#include <Windows.h>

/* just use Process Hacker's definition: for complete compatability
 * phnt: #include <phnt_windows.h> <phnt.h> */

#pragma warning(push)
#pragma warning(disable: 4201)

using NTSTATUS = LONG;
#define STATUS_SUCCESS				((NTSTATUS)0x00000000L)
#define STATUS_UNSUCCESSFUL			((NTSTATUS)0xC0000001L)
#define STATUS_BUFFER_TOO_SMALL		((NTSTATUS)0xC0000023L)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_PARTIAL_COPY			((NTSTATUS)0x8000000DL)
#define STATUS_ACCESS_DENIED		((NTSTATUS)0xC0000022L)
#define NT_SUCCESS(status)			((NTSTATUS)(status) >= 0)

#define CURRENT_PROCESS		reinterpret_cast<HANDLE>(-1)


using KPRIORITY = LONG;
typedef struct
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;


enum class PoolType
{
	NonPaged,
	Paged,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS

};

enum class SysInfo
{
	Basic = 0,
	Processor = 1,
	Performance = 2,
	Path = 4,
	Process = 5,	// SYSTEM_PROCESS_INFORMATION
	Device = 7,
	ProcessorPerformance = 8,
	Module = 11, // RTL_PORCESS_MODULE_INFORMATION
	Handle = 16,
	Object = 17,  //could be wrong
	KernelDebugger = 35,
	SessionProcess = 53,
	ExtendedProcess = 57, //SYSTEM_EXTENDED_PROCESS_INFORMATION
	ExtendedHandle = 64, //SYSTEM_HANDLE_INFORMATION_EX
	ProcessId = 88, //SYSTEM_PROCESS_ID_INFORMATION 
									 /*	SYSTEM_PROCESS_ID_INFORMATION pinfo {};
									 pinfo.ImageName.MaximumLength = (USHORT) 0x100;
									 pinfo.ImageName.Length = 0;
									 pinfo.ImageName.Buffer = new wchar_t[0x100] {};
									 pinfo.ProcessId = (HANDLE)::GetCurrentProcessId();
									 ULONG_PTR rtrned {};
									 api.NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS) 88,
									 &pinfo,
									 0x18,
									 &rtrned);*/
	FullProcess = 148	//requires admin : SYSTEM_PROCESS_INFO w/ PSYSTEM_PROCESS_INFO_EXTENSION
};

enum class ThreadInfo
{
	Basic,
	Times,
	Priority,
	BasePriority,
	AffinityMask,
	ImpersonationToken,
	DescriptorTableEntry,
	EnableAlignmentFaultFixup,
	EventPair_Reusable,
	QuerySetWin32StartAddress,
	ZeroTlsCell,
	PerformanceCount,
	AmILast,
	IdealProcessor,
	PriorityBoost,
	SetTlsArrayAddress,
	IsIoPending,
	HideFromDebugger,
	BreakOnTermination,
	SwitchLegacyState,
	IsTerminated,
	LastSystemCall,
	IoPriority,
	CycleTime,
	PagePriority,
	ActualBasePriority,
	TebInformation,
	CSwitchMon,
	CSwitchPmu,
	Wow64Context,
	GroupInformation,
	UmsInformation,
	CounterProfiling,
	IdealProcessorEx,
	MaxThreadInfoClass
};

enum class FileInfo
{
	Directory = 1,
	FullDirectory,   // 2
	BothDirectory,   // 3
	Basic,           // 4
	Standard,        // 5
	Internal,        // 6
	Ea,              // 7
	Access,          // 8
	Name,            // 9
	Rename,          // 10
	Link,            // 11
	Names,           // 12
	Disposition,     // 13
	Position,        // 14
	FullEa,          // 15
	Mode,            // 16
	Alignment,       // 17
	All,             // 18
	Allocation,      // 19
	EndOf,       // 20
	AlternateName,   // 21
	Stream,          // 22
	Pipe,            // 23
	PipeLocal,       // 24
	PipeRemote,      // 25
	MailslotQuery,   // 26
	MailslotSet,     // 27
	Compression,     // 28
	ObjectId,        // 29
	Completion,      // 30
	MoveCluster,     // 31
	Quota,           // 32
	ReparsePoint,    // 33
	NetworkOpen,     // 34
	AttributeTag,    // 35
	Tracking,        // 36
	IdBothDirectory, // 37
	IdFullDirectory, // 38
	ValidDataLength, // 39
	ShortName,       // 40
	IoCompletionNotification, // 41
	IoStatusBlockRange,       // 42
	IoPriorityHint,           // 43
	SfioReserve,              // 44
	SfioVolume,               // 45
	HardLink,                 // 46
	ProcessIdsUsing,      // 47
	NormalizedName,           // 48
	NetworkPhysicalName,      // 49
	IdGlobalTxDirectory,      // 50
	IsRemoteDevice,           // 51
	Unused,                   // 52
	NumaNode,                 // 53
	StandardLink,             // 54
	RemoteProtocol,           // 55
											 //  These are special versions of these operations (defined earlier)
											 //  which can be used by kernel mode drivers only to bypass security
											 //  access checks for Rename and HardLink operations.  These operations
											 //  are only recognized by the IOManager, a  system should never
											 //  receive these.
											 //
											 RenameBypassAccessCheck,  // 56
											 LinkBypassAccessCheck,    // 57
											 MemoryPartition,          // 69
											 FileMaximumInformation
};

enum class ObjInfo
{
	Basic,
	Name,
	Type,
	Types,
	HandleFlag,
	Session,
	MaxObjectInfoClass
};

enum class PsInfo
{
	Basic,
	Quota,
	IoCounters,
	ProcessVmCounters,
	ProcessTimes,
	BasePriority,
	RaisePriority,
	DebugPort,
	ExceptionPort,
	AccessToken,
	LdtInformation,
	LdtSize,
	DefaultHardErrorMode,
	IoPortHandlers,
	PooledUsageAndLimits,
	WorkingSetWatch,
	UserModeIOPL,
	EnableAlignmentFaultFixup,
	PriorityClass,
	Wx86Information,
	HandleCount,
	AffinityMask,
	PriorityBoost,
	DeviceMap = 23,
	SessionInformation = 24,
	ForegroundInformation = 25,
	Wow64Information = 26,
	ImageFileName = 27,  //\Device\HarddiskVolume...\\... : use UNICODE_STRING as rtrn buff
	LUIDDeviceMapsEnabled = 28,
	BreakOnTermination = 29,
	DebugObjectHandle = 30,
	DebugFlags = 31,
	HandleTracing = 32,
	ExecuteFlags = 34,
	TlsInformation = 35,
	Cookie = 36,
	ImageInformation = 37,
	CycleTime = 38,
	PagePriority = 39,
	InstrumentationCallback = 40,
	ThreadStackAllocation = 41,
	WorkingSetWatchEx = 42,
	ImageFileNameWin32 = 43,  //C:\\<path><exe> -> win32api!QueryImageName : also UNICODE_STR as buf
	ImageFileMapping = 44,
	AffinityUpdateMode = 45,
	MemoryAllocationMode = 46,
	GroupInformation = 47,
	TokenVirtualizationEnabled = 48,
	ConsoleHost = 49,
	WindowInformation = 50,
	MaxProcessInfoClass
};

enum class MemInfo
{
	BasicInformation,				// MEMORY_BASIC_INFORMATION
	WorkingSetInformation,		// MEMORY_WORKING_SET_INFORMATION
	MappedFilenameInformation,	// UNICODE_STRING
	RegionInformation,			// MEMORY_REGION_INFORMATION
	WorkingSetExInformation,		// MEMORY_WORKING_SET_EX_INFORMATION
	SharedCommitInformation,		// MEMORY_SHARED_COMMIT_INFORMATION
	ImageInformation,				// MEMORY_IMAGE_INFORMATION
	RegionInformationEx,
	PrivilegedBasicInformation
};

typedef enum _KWAIT_REASON
{
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    WrKeyedEvent,
    WrTerminated,
    WrProcessInSwap,
    WrCpuRateControl,
    WrCalloutStack,
    WrKernel,
    WrResource,
    WrPushLock,
    WrMutex,
    WrQuantumEnd,
    WrDispatchInt,
    WrPreempted,
    WrYieldExecution,
    WrFastMutex,
    WrGuardedMutex,
    WrRundown,
    WrAlertByThreadId,
    WrDeferredPreempt,
    MaximumWaitReason
} KWAIT_REASON, *PKWAIT_REASON;



typedef struct _VM_COUNTERS
{
#if defined(_M_X64)
	SIZE_T PeakVirtualSize;
	SIZE_T PageFaultCount;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	SIZE_T QuotaPeakPagedPoolUsage;
	SIZE_T QuotaPagedPoolUsage;
	SIZE_T QuotaPeakNonPagedPoolUsage;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
	SIZE_T VirtualSize;
#else
	SIZE_T PeakVirtualSize;
	SIZE_T VirtualSize;
	ULONG PageFaultCount;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	SIZE_T QuotaPeakPagedPoolUsage;
	SIZE_T QuotaPagedPoolUsage;
	SIZE_T QuotaPeakNonPagedPoolUsage;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
#endif
} VM_COUNTERS;
typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID    Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
typedef struct _CLIENT_ID
{
#if defined(_M_X64)
	ULONGLONG UniqueProcess; //making it pvoid somehow causes OpenProc to work, prolly others
	ULONGLONG UniqueThread;
#else
	ULONG UniqueProcess; //making it pvoid somehow causes OpenProc to work, prolly others
	ULONG UniqueThread;
#endif
} CLIENT_ID, *PCLIENT_ID;
// TEB / PEB STUFF
typedef struct _RTL_USER_PROCESS_PARAMETERS
{
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;
//typedef VOID(NTAPI *PPS_POST_PROCESS_INIT_ROUTINE) (VOID);
using PPS_POST_PROCESS_INIT_ROUTINE = VOID(NTAPI*)(VOID);

typedef struct _MEMORY_WORKING_SET_BLOCK
{
	ULONG_PTR Protection : 5;
	ULONG_PTR ShareCount : 3;
	ULONG_PTR Shared : 1;
	ULONG_PTR Node : 3;
#ifdef _WIN64
	ULONG_PTR VirtualPage : 52;
#else
	ULONG VirtualPage : 20;
#endif
} MEMORY_WORKING_SET_BLOCK, *PMEMORY_WORKING_SET_BLOCK;
typedef struct _MEMORY_WORKING_SET_INFORMATION
{
	ULONG_PTR NumberOfEntries;
	MEMORY_WORKING_SET_BLOCK WorkingSetInfo[1];
} MEMORY_WORKING_SET_INFORMATION, *PMEMORY_WORKING_SET_INFORMATION;
typedef struct _MEMORY_REGION_INFORMATION
{
	PVOID AllocationBase;
	ULONG AllocationProtect;
	union
	{
		ULONG RegionType;
		struct
		{
			ULONG Private : 1;
			ULONG MappedDataFile : 1;
			ULONG MappedImage : 1;
			ULONG MappedPageFile : 1;
			ULONG MappedPhysical : 1;
			ULONG DirectMapped : 1;
			ULONG Reserved : 26;
		};
	};
	SIZE_T RegionSize;
	SIZE_T CommitSize;
} MEMORY_REGION_INFORMATION, *PMEMORY_REGION_INFORMATION;
typedef struct _MEMORY_WORKING_SET_EX_BLOCK
{
	union
	{
		struct
		{
			ULONG_PTR Valid : 1;
			ULONG_PTR ShareCount : 3;
			ULONG_PTR Win32Protection : 11;
			ULONG_PTR Shared : 1;
			ULONG_PTR Node : 6;
			ULONG_PTR Locked : 1;
			ULONG_PTR LargePage : 1;
			ULONG_PTR Priority : 3;
			ULONG_PTR Reserved : 3;
			ULONG_PTR SharedOriginal : 1;
			ULONG_PTR Bad : 1;
#ifdef _WIN64
			ULONG_PTR ReservedUlong : 32;
#endif
		};
		struct
		{
			ULONG_PTR Valid : 1;
			ULONG_PTR Reserved0 : 14;
			ULONG_PTR Shared : 1;
			ULONG_PTR Reserved1 : 5;
			ULONG_PTR PageTable : 1;
			ULONG_PTR Location : 2;
			ULONG_PTR Priority : 3;
			ULONG_PTR ModifiedList : 1;
			ULONG_PTR Reserved2 : 2;
			ULONG_PTR SharedOriginal : 1;
			ULONG_PTR Bad : 1;
#ifdef _WIN64
			ULONG_PTR ReservedUlong : 32;
#endif
		} Invalid;
	};
} MEMORY_WORKING_SET_EX_BLOCK, *PMEMORY_WORKING_SET_EX_BLOCK;
typedef struct _MEMORY_WORKING_SET_EX_INFORMATION
{
	PVOID VirtualAddress;
	union
	{
		MEMORY_WORKING_SET_EX_BLOCK VirtualAttributes;
		ULONG_PTR Long;
	} u1;
} MEMORY_WORKING_SET_EX_INFORMATION, *PMEMORY_WORKING_SET_EX_INFORMATION;
typedef struct _MEMORY_SHARED_COMMIT_INFORMATION
{
	SIZE_T CommitSize;
} MEMORY_SHARED_COMMIT_INFORMATION, *PMEMORY_SHARED_COMMIT_INFORMATION;
typedef struct _MEMORY_IMAGE_INFORMATION
{
	PVOID ImageBase;
	SIZE_T SizeOfImage;
	union
	{
		ULONG ImageFlags;
		struct
		{
			ULONG ImagePartialMap : 1;
			ULONG ImageNotExecutable : 1;
			ULONG Reserved : 30;
		};
	};
} MEMORY_IMAGE_INFORMATION, *PMEMORY_IMAGE_INFORMATION;

template<class T>
struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks; // PVOID Reserved1[2];
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitOrderLinks;// PVOID Reserved2[2];
	PVOID DllBase;
	PVOID EntryPoint; //PVOID Reserved3[2]; entrypoint, sizeofimage
	SIZE_T SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName; // BYTE Reserved4[8];
	PVOID Reserved5[3];
#pragma warning(push)
#pragma warning(disable: 4201) // we'll always use the Microsoft compiler
	union
	{
		ULONG CheckSum;
		PVOID Reserved6;
	} DUMMYUNIONNAME;
#pragma warning(pop)
	ULONG TimeDateStamp;
};
using LDR_DATA_TABLE_ENTRY32 = _LDR_DATA_TABLE_ENTRY<ULONG_PTR>;
using LDR_DATA_TABLE_ENTRY64 = _LDR_DATA_TABLE_ENTRY<ULONG_PTR>;

typedef struct _PEB_LDR_DATA
{
	BYTE Reserved1[8]; // 0x0 Length, 0x04 Initialized, 0x8 SsHandle
	PVOID Reserved2; // 
	LIST_ENTRY	InLoadOrderModuleList;
	LIST_ENTRY	InMemoryOrderModuleList;
	LIST_ENTRY	InInitOrderModuleList; // PVOID Reserved3[3]; // InInitOrderModuleList
} PEB_LDR_DATA, *PPEB_LDR_DATA;

#pragma pack(push)
#pragma pack(1)
template<class T, class NGF, int A>
struct _PEB_T
{
	union
	{
		struct
		{
			BYTE InheritedAddressSpace;
			BYTE ReadImageFileExecOptions;
			BYTE BeingDebugged;
			BYTE BitField;
		};
		T dummy01;
	};
	T Mutant;
	T ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	T SubSystemData;
	T ProcessHeap;
	T FastPebLock;
	T AtlThunkSListPtr;
	T IFEOKey;
	T CrossProcessFlags;
	T UserSharedInfoPtr;
	DWORD SystemReserved;
	DWORD AtlThunkSListPtr32;
	T ApiSetMap;
	T TlsExpansionCounter;
	T TlsBitmap;
	DWORD TlsBitmapBits[2];
	T ReadOnlySharedMemoryBase;
	T HotpatchInformation;
	T ReadOnlyStaticServerData;
	T AnsiCodePageData;
	T OemCodePageData;
	T UnicodeCaseTableData;
	DWORD NumberOfProcessors;
	union
	{
		DWORD NtGlobalFlag;
		NGF dummy02;
	};
	LARGE_INTEGER CriticalSectionTimeout;
	T HeapSegmentReserve;
	T HeapSegmentCommit;
	T HeapDeCommitTotalFreeThreshold;
	T HeapDeCommitFreeBlockThreshold;
	DWORD NumberOfHeaps;
	DWORD MaximumNumberOfHeaps;
	T ProcessHeaps;
	T GdiSharedHandleTable;
	T ProcessStarterHelper;
	T GdiDCAttributeList;
	T LoaderLock;
	DWORD OSMajorVersion;
	DWORD OSMinorVersion;
	WORD OSBuildNumber;
	WORD OSCSDVersion;
	DWORD OSPlatformId;
	DWORD ImageSubsystem;
	DWORD ImageSubsystemMajorVersion;
	T ImageSubsystemMinorVersion;
	T ActiveProcessAffinityMask;
	T GdiHandleBuffer[A];
	T PostProcessInitRoutine;
	T TlsExpansionBitmap;
	DWORD TlsExpansionBitmapBits[32];
	T SessionId;
	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	T pShimData;
	T AppCompatInfo;
	UNICODE_STRING CSDVersion;
	T ActivationContextData;
	T ProcessAssemblyStorageMap;
	T SystemDefaultActivationContextData;
	T SystemAssemblyStorageMap;
	T MinimumStackCommit;
	T FlsCallback;
	LIST_ENTRY FlsListHead;
	T FlsBitmap;
	DWORD FlsBitmapBits[4];
	T FlsHighIndex;
	T WerRegistrationData;
	T WerShipAssertPtr;
	T pContextData;
	T pImageHeaderHash;
	T TracingFlags;
};
using  PEB32 = _PEB_T<DWORD, DWORD64, 34>;
using  PEB64 = _PEB_T<DWORD64, DWORD, 30>;
#pragma pack(pop)

#ifdef _M_IX86
using PEB = PEB32;
using PPEB = PEB32 * ;
using LDR_DATA_TABLE_ENTRY = LDR_DATA_TABLE_ENTRY32;
#elif defined(_M_X64)
using PEB = PEB64;
using PPEB = PEB64 * ;
using LDR_DATA_TABLE_ENTRY = LDR_DATA_TABLE_ENTRY64;
#else
#error "Unsupported architecture"
#endif
typedef struct _TEB
{
	PVOID Reserved1[12];
	PPEB ProcessEnvironmentBlock;
	PVOID Reserved2[399];
	BYTE Reserved3[1952];
	PVOID TlsSlots[64];
	BYTE Reserved4[8];
	PVOID Reserved5[26];
	PVOID ReservedForOle;  // Windows 2000 only
	PVOID Reserved6[4];
	PVOID TlsExpansionSlots;
} TEB, *PTEB;

typedef struct
{
	SIZE_T	FileNameLength;
	WCHAR	FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

typedef enum
{
	DirectoryNotifyInformation = 1,
	DirectoryNotifyExtendedInformation // 2
} DIRECTORY_NOTIFY_INFORMATION_CLASS, *PDIRECTORY_NOTIFY_INFORMATION_CLASS;

//
// Define the various structures which are returned on query operations
//
typedef struct
{
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;
typedef struct
{
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG NumberOfLinks;
	BOOLEAN DeletePending;
	BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;
#if (_WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD)
typedef struct
{
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG NumberOfLinks;
	BOOLEAN DeletePending;
	BOOLEAN Directory;
	BOOLEAN AlternateStream;
	BOOLEAN MetadataAttribute;
} FILE_STANDARD_INFORMATION_EX, *PFILE_STANDARD_INFORMATION_EX;
#endif 
// THREAD INFORMATION
typedef struct
{
	NTSTATUS  ExitStatus;
	PVOID     TebBaseAddress;
	CLIENT_ID ClientId;
	ULONG_PTR AffinityMask;
	LONG      Priority;
	LONG      BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;
typedef struct
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    KWAIT_REASON WaitReason;
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;
typedef struct
{
	SYSTEM_THREAD_INFORMATION ThreadInfo;
	PVOID StackBase;
	PVOID StackLimit;
	PVOID Win32StartAddress;
	PVOID TebAddress; /* This is only filled in on Vista and above */
	ULONG_PTR Reserved1;
    ULONG_PTR Reserved2;
    ULONG_PTR Reserved3;
}SYSTEM_EXTENDED_THREAD_INFORMATION, *PSYSTEM_EXTENDED_THREAD_INFORMATION;
// PROCESS INFORMATION
typedef struct
{
	PVOID		ExitStatus;
	PPEB		PebBaseAddress;
	PVOID		AffinityMask;
	PVOID		BasePriority;
	ULONG_PTR	UniqueProcessId;
	PVOID		InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;
typedef struct
{
	ULONG NextEntryDelta;
	ULONG NumberOfThreads;
	ULONG Reserved1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY BasePriority;
	SIZE_T ProcessId;  // this and the next 3 originally ULONG
	SIZE_T InheritedFromProcessId;
	ULONG HandleCount;
	ULONG SessionId;
	SIZE_T Reserved2[2];
	VM_COUNTERS VmCounters;
#if _WIN32_WINNT >= 0x500
	IO_COUNTERS IoCounters;
#endif
	SYSTEM_THREAD_INFORMATION Threads[1];
} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize; // since VISTA
    ULONG HardFaultCount; // since WIN7
    ULONG NumberOfThreadsHighWatermark; // since WIN7
    ULONGLONG CycleTime; // since WIN7
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR UniqueProcessKey; // since VISTA (requires SystemExtendedProcessInformation)
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    SYSTEM_THREAD_INFORMATION Threads[1]; // SystemProcessInformation
                                          // SystemExtendedProcessinformation
                                          // SYSTEM_EXTENDED_THREAD_INFORMATION + SYSTEM_PROCESS_INFORMATION_EXTENSION // SystemFullProcessInformation
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;



typedef struct
{
	ULONG SessionId;
	SIZE_T BufferLength;
	PVOID Buffer;
} SYSTEM_SESSION_PROCESS, *PSYSTEM_SESSION_PROCESS;
typedef struct
{
	HANDLE ProcessId;
	UNICODE_STRING ImageName;
} SYSTEM_PROCESS_ID_INFORMATION;
// HANDLE INFORMATION
typedef struct
{
	ULONG OwnerPid;
	BYTE ObjectType;
	BYTE HandleFlags;
	USHORT HandleValue;
	PVOID ObjPointer;
	ULONG AccessMask;
}SYSTEM_HANDLE_ENTRY, *PSYSTEM_HANDLE_ENTRY;
typedef struct
{
	USHORT		UniqueProcessId; //ULONG
	USHORT		CreatorBackTraceIndex; //SIZE_T	
	UCHAR		ObjectTypeIndex;
	UCHAR		HandleAttributes;
	USHORT		HandleValue;
	PVOID		Object;
	ULONG		GrantedAccess;
}SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;
typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG HandleCount;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
/* System Information Class 0x40 */
typedef struct
{
	PVOID     Object;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR HandleValue;
	ULONG     GrantedAccess;
	USHORT    CreatorBackTraceIndex;
	USHORT    ObjectTypeIndex;
	ULONG     HandleAttributes;
	ULONG     Reserved;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;
typedef struct
{
	ULONG_PTR  Count;
	ULONG_PTR  Reserved;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handle[1];
} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;
// CPU INFO : SystemInformationClass 0x01 
typedef struct
{
	WORD Architecture, Level, Revision, Reserved /*always 0*/;
	ULONG FeatureSet; //see bit flags below
} SYSTEM_CPU_INFORMATION, *PSYSTEM_CPU_INFORMATION;
/* definitions of bits in the Feature set for the x86 processors */
#define CPU_FEATURE_VME    0x00000005   /* Virtual 86 Mode Extensions */
#define CPU_FEATURE_TSC    0x00000002   /* Time Stamp Counter available */
#define CPU_FEATURE_CMOV   0x00000008   /* Conditional Move instruction*/
#define CPU_FEATURE_PGE    0x00000014   /* Page table Entry Global bit */
#define CPU_FEATURE_PSE    0x00000024   /* Page Size Extension */
#define CPU_FEATURE_MTRR   0x00000040   /* Memory Type Range Registers */
#define CPU_FEATURE_CX8    0x00000080   /* Compare and eXchange 8 byte instr. */
#define CPU_FEATURE_MMX    0x00000100   /* Multi Media eXtensions */
#define CPU_FEATURE_X86    0x00000200   /* seems to be always ON, on the '86 */
#define CPU_FEATURE_PAT    0x00000400   /* Page Attribute Table */
#define CPU_FEATURE_FXSR   0x00000800   /* FXSAVE and FXSTORE instructions */
#define CPU_FEATURE_SEP    0x00001000   /* SYSENTER and SYSEXIT instructions */
#define CPU_FEATURE_SSE    0x00002000   /* SSE extensions (ext. MMX) */
#define CPU_FEATURE_3DNOW  0x00004000   /* 3DNOW instructions available */
#define CPU_FEATURE_SSE2   0x00010000   /* SSE2 extensions (XMMI64) */
#define CPU_FEATURE_DS     0x00020000   /* Debug Store */
#define CPU_FEATURE_HTT    0x00040000   /* Hyper Threading Technology */

// OBJECT INFORMATION
typedef struct
{
	UNICODE_STRING Name;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG TotalPagedPoolUsage;
	ULONG TotalNonPagedPoolUsage;
	ULONG TotalNamePoolUsage;
	ULONG TotalHandleTableUsage;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	ULONG HighWaterPagedPoolUsage;
	ULONG HighWaterNonPagedPoolUsage;
	ULONG HighWaterNamePoolUsage;
	ULONG HighWaterHandleTableUsage;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccess;
	BOOLEAN SecurityRequired;
	BOOLEAN MaintainHandleCount;
	USHORT MaintainTypeList;
	PoolType PoolType;
	ULONG PagedPoolUsage;
	ULONG NonPagedPoolUsage;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;
typedef struct
{
	UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;
// PROCESS MODULES
typedef struct
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
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;
typedef struct
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

typedef struct
{
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

using PKNORMAL_ROUTINE = VOID(NTAPI*)(_In_ PVOID NormalContext OPTIONAL,
									  _In_ PVOID SystemArgument1 OPTIONAL,
									  _In_ PVOID SystemArgument2 OPTIONAL);

using PKKERNEL_ROUTINE = VOID(NTAPI*)(_In_ struct _KAPC *Apc,
									  _In_ _Out_ PKNORMAL_ROUTINE *NormalRoutine OPTIONAL,
									  _In_ _Out_ PVOID *NormalContext OPTIONAL,
									  _In_ _Out_ PVOID *SystemArgument1 OPTIONAL,
									  _In_ _Out_ PVOID *SystemArgument2 OPTIONAL);
// DEBUGES FLAGS AND ACCESS MASKS
#define DEBUG_READ_EVENT			0x0001
#define DEBUG_PROCESS_ASSIGN		0x0002
#define DEBUG_SET_INFORMATION		0x0004
#define DEBUG_QUERY_INFORMATION		0x0008
#define DEBUG_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
		DEBUG_READ_EVENT | DEBUG_PROCESS_ASSIGN | DEBUG_SET_INFORMATION | \
		DEBUG_QUERY_INFORMATION

#define OBJ_INHERIT								0x00000002L
#define OBJ_PERMANENT							0x00000010L
#define OBJ_EXCLUSIVE							0x00000020L
#define OBJ_CASE_INSENSITIVE					0x00000040L
#define OBJ_OPENIF								0x00000080L
#define OBJ_OPENLINK							0x00000100L
#define OBJ_KERNEL_HANDLE						0x00000200L
#define OBJ_FORCE_ACCESS_CHECK					0x00000400L
#define OBJ_IGNORE_IMPERSONATED_DEVICEMAP		0x00000800L
#define OBJ_DONT_REPARSE						0x00001000L
#define OBJ_VALID_ATTRIBUTES					0x00001FF2L

/*  inits OBJECT_ATTRIBUTES struct, which specifis the properties of an obj handle
to routines that open handles */
#define InitializeObjectAttributes(p,n,a,r,s) \
      do { \
          (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
          (p)->RootDirectory = r; \
          (p)->Attributes = a; \
          (p)->ObjectName = n; \
          (p)->SecurityDescriptor = s; \
          (p)->SecurityQualityOfService = NULL; \
     } while (0)


extern "C"
{
	NTSTATUS
	NtQuerySystemInformation(
		SysInfo,
		PVOID,
		SIZE_T,
		PSIZE_T
	);

	NTSTATUS
	NtQueryInformationProcess(
		HANDLE,
		PsInfo,
		PVOID,
		SIZE_T,
		PSIZE_T
	);

	NTSTATUS
	NtQueryInformationThread(
		HANDLE,
		ThreadInfo,
		PVOID,
		SIZE_T,
		PSIZE_T
	);

	NTSTATUS
	NtOpenProcess(
		_Out_	PHANDLE,
		_In_	ACCESS_MASK,
		_In_	POBJECT_ATTRIBUTES,
		_In_	PCLIENT_ID
	);

	NTSTATUS
	NtOpenThread(
		_Out_ PHANDLE,
		_In_  ACCESS_MASK,
		_In_  POBJECT_ATTRIBUTES,
		_In_  PCLIENT_ID
	);

	NTSTATUS
	NtGetContextThread(
		HANDLE,
		PCONTEXT
	);
	
	NTSTATUS
	NtSetContextThread(
		HANDLE,
		PCONTEXT
	);

	NTSTATUS
	NtSuspendThread(
		HANDLE,
		PULONG
	);

	NTSTATUS
	NtResumeThread(
		HANDLE,
		PULONG
	);

	NTSTATUS
	NtCreateThreadEx(
		_Out_ PHANDLE					hThread,
		_In_  ACCESS_MASK				DesiredAccess,
		_In_  POBJECT_ATTRIBUTES		ObjectAttributes,
		_In_  HANDLE					ProcessHandle,
		_In_  LPTHREAD_START_ROUTINE	lpStartAddress,
		_In_  LPVOID					lpParameter,
		_In_  BOOL						CreateSuspended,
		_In_  SIZE_T					StackZeroBits,
		_In_  SIZE_T					SizeOfStackCommit,
		_In_  SIZE_T					SizeOfstackReserve,
		_Out_ LPVOID					lpBytesBuffer
	);

	NTSTATUS
	NtWaitForSingleObject(
		HANDLE,
		BOOLEAN,
		PLARGE_INTEGER
	);

	NTSTATUS
	NtClose(
		_In_ HANDLE
	);

	NTSTATUS
	NtAllocateVirtualMemory(
		_In_	HANDLE,
		_Inout_ PVOID		UBaseAddress,
		_In_	ULONG_PTR	ZeroBits,
		_Inout_ PSIZE_T		URegionSize,
		_In_	ULONG		AllocationType,
		_In_	ULONG       Protect

	);

	NTSTATUS
	NtAllocateVirtualMemory(
		_In_	HANDLE		ProcessHandle,
		_Inout_	PVOID		UBaseAddress,
		_In_	ULONG_PTR	ZeroBits,
		_Inout_ PSIZE_T		URegionSize,
		_In_	ULONG		AllocationType,
		_In_	ULONG           Protect
	);

	NTSTATUS
	NtWriteVirtualMemory(
		_In_ HANDLE,
		_In_ PVOID,
		_In_ PVOID,
		_In_ SIZE_T,
		_Out_ PULONG
	);

	NTSTATUS 
	NtReadVirtualMemory(
		HANDLE hProcess,
		PVOID AddressToRead,
		PVOID BufferToStoreDate,
		ULONG szBuffer,
		PULONG bRead
	
	);
	NTSTATUS
	NtFreeVirtualMemory(
		HANDLE ProcessHandle,
		PVOID* UBaseAddress,  //beginning of buffer returned by NtAlloc
		PSIZE_T URegiosSize,  //size of that buffer
		ULONG FreeType
		);

	NTSTATUS
	NtQueryVirtualMemory(
		_In_	HANDLE     ProcessHandle,
		_In_	PVOID		BaseAddress,
		_In_	MemInfo		MemoryInformationClass,
		_Out_	PVOID		Buffer,
		_In_	ULONG_PTR	Length, //was originally ULONG, out also PULONG: changed to SIZE_T no more access violation ??!!??
		_Out_opt_ PULONG_PTR	ResultLength OPTIONAL
		);

	NTSTATUS
	NtQueueApcThread(
		_In_ HANDLE			 ThreadHandle,
		_In_ PKNORMAL_ROUTINE ApcRoutine,
		_In_ PVOID			 NormalContext,
		_In_ PVOID			 SysArgument1,
		_In_ PVOID			 SysArgument2
	);

	NTSTATUS
	NtDelayExecution(
		_In_ BOOLEAN, 
		_Inout_ PLARGE_INTEGER
	);

	NTSTATUS
	NtUnmapViewOfSection(
		_In_ HANDLE,
		_In_ PVOID
	);

	PIMAGE_NT_HEADERS
	RtlImageNtHeader(
		_In_ PVOID
	);

	void
	RtlInitUnicodeString(
		_Inout_ PUNICODE_STRING,
		_In_	LPWSTR
	);
    
    NTSTATUS
    NtCreateSection(
        _Out_ PHANDLE             SectionHandle,
        _In_ ULONG                DesiredAccess,
        _In_ POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL,
        _In_ PLARGE_INTEGER       MaximumSize OPTIONAL,
        _In_ ULONG                PageAttributess,
        _In_ ULONG                SectionAttributes,
        _In_ HANDLE               FileHandle OPTIONAL
    );

    NTSTATUS
    ZwMapViewOfSection(
        _In_ HANDLE               SectionHandle,
        _In_ HANDLE               ProcessHandle,
        _In_ OUT PVOID            *BaseAddress OPTIONAL,
        _In_ ULONG                ZeroBits OPTIONAL,
        _In_ ULONG                CommitSize,
        _In_ OUT PLARGE_INTEGER   SectionOffset OPTIONAL,
        _In_ OUT PULONG           ViewSize,
        _In_ BOOLEAN              InheritDisposition,
        _In_ ULONG                AllocationType OPTIONAL,
        _In_ ULONG                Protect
    );


}

#pragma warning(pop)

//
// NT NAMESPACE 
//

#ifdef _WIN64
#define GET_TEB()   \
     reinterpret_cast<u8*>(__readgsqword(0x30))

#define GET_PEB()   \
     reinterpret_cast<void*>(__readgsqword(0x60))

#else

#define GET_TEB() \
     reinterpret_cast<u8*>(__readfsdword(0x18))
#define GET_PEB() \
     reinterpret_cast<void*>(__readfsdword(0x30))

#endif


namespace nt::internal {
    
    SIZE_T QuerySystemInfo(
        SysInfo Class,
        void* Buffer = nullptr,
        SIZE_T SizeOfBuffer = 0
    );

    PVOID QueryProcessPeb(
        HANDLE Process
    );

    PVOID QueryProcessPeb32(
        HANDLE Process
    );

    u32 GetTID(
        HANDLE Thread
    );

    u32 GetPidOfThread(
        HANDLE Thread
    );

    u64 QueryVM(
        HANDLE Process,
        PVOID   Address,
        MemInfo Class,
        PVOID   Buffer,
        SIZE_T  BufferSize
    );

    bool ReadVM(
        HANDLE Process,
        PVOID  Address,
        PVOID  Buffer,
        ULONG  BufferSize
    );
}




namespace nt {

    void* GetPeb(HANDLE Process);
    void* GetPeb32(HANDLE Process);
    void* GetTeb(HANDLE Process);
    u64 QueryVMBasicInfo(
        HANDLE  Process,
        PVOID   AddressToQuery,
        PVOID   OutputBuffer
    );

    //
    // Used to load executable module specified in ImagePath
    //
    void* MapSectionView(LPCWSTR ImagePath);

    //
    // RtlImageNtHeader wrapper
    //
    PIMAGE_NT_HEADERS GetNtHeaders(LPVOID BaseAddress);

    //
    // Binary search for RoutineName in Module
    //  - targets only Modules loaded in memory
    //
    void* GetProcAddress(
        HMODULE Module,
        LPCSTR  RoutineName
    );


    //
    // primary process enumaration routine: ExtendedInfo
    //
    struct ProcessInfo; struct ThreadInfo;
    using Processes     = std::vector<nt::ProcessInfo>;
    using Threads       = std::vector<nt::ThreadInfo>;

    struct ThreadInfo
    {
        u32 Tid;
        void* StartAddress {};
        bool isMain { false };
        void* TebBaseAddress;
    };

    struct ProcessInfo
    {
        wchar_t Name[MAX_PATH * 2] {};
        wchar_t CreateTime[60] {};

        u32 Pid;
        u32 ParentPid;
        u32 HandleCount;
        u32 SessionId;
        u32 ThreadCount;

        Threads Threads;
    };

    

    Processes EnumProcesses();
    ProcessInfo EnumProcess(std::wstring const& ProcessName);
    ProcessInfo EnumProcess(u32 Pid);

}
