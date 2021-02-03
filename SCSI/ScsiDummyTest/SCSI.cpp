#include "Common.h"
#include <scsi.h>
#include <ntddscsi.h>
#include "SCSI.h"

typedef struct
{
	SCSI_PASS_THROUGH_DIRECT DirectData;
	SENSE_DATA SenseData;
} 	SCSI_CMD_DATA, *PSCSI_CMD_DATA; //space


NTSTATUS
SCSI::
SendScsiCmd(
	PDEVICE_OBJECT DeviceObj,
	BYTE OperationCode,
	BYTE DataIn,
	PVOID DataBuffer,
	ULONG DataTransferLen,
	ULONG_PTR LogicalBlockAddress,
	USHORT TransferLen
	) {
	
	SCSI_CMD_DATA Data;
	KEVENT Event;
	IO_STATUS_BLOCK StatusBlk;


	if (DeviceObj == nullptr) return STATUS_UNSUCCESSFUL;
	
	/*-----------------------------------------------------------------------------
	maybe it's 2 consecutive memsets to null 2 structs and the compiler
	optimized it out because they're layed out directly next to each other in memory
	either way, size is 0x48 = SCSI_PASS_THROUGH_DIRECT + SENSE_DATA
	I can't tell but whatever this works fine
	-----------------------------------------------------------------------------*/
	
	RtlZeroMemory(&Data, 0x48);

	Data.DirectData.Length = (USHORT) sizeof(SCSI_PASS_THROUGH_DIRECT);
	Data.DirectData.DataIn = DataIn;
	Data.DirectData.SenseInfoOffset = 0x2C;
	Data.DirectData.SenseInfoLength = sizeof(SENSE_DATA);
	Data.DirectData.DataBuffer = DataBuffer;
	Data.DirectData.DataTransferLength = DataTransferLen;
	Data.DirectData.CdbLength = CDB10GENERIC_LENGTH;
	Data.DirectData.TimeOutValue = 5000;

	Data.DirectData.Cdb[0] = OperationCode;
	Data.DirectData.Cdb[2] = (BYTE) ((LogicalBlockAddress & 0xFF000000) >> 24);
	Data.DirectData.Cdb[3] = (BYTE) ((LogicalBlockAddress & 0xFF0000) >> 16);
	Data.DirectData.Cdb[4] = (LogicalBlockAddress & 0xFF00) >> 8;
	Data.DirectData.Cdb[5] = (LogicalBlockAddress & 0xFF);
	Data.DirectData.Cdb[6] = 0x7b;
	Data.DirectData.Cdb[7] = (BYTE) (TransferLen & 0xFF00);
	Data.DirectData.Cdb[8] = (TransferLen & 0xFF);

	KeInitializeEvent(&Event, SynchronizationEvent, FALSE);
	auto Irp = IoBuildDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
						 DeviceObj,
						 &Data, sizeof(Data),
						 &Data, sizeof(Data),
						 FALSE,
						 &Event,
						 &StatusBlk);
	if (Irp) {
		if (IofCallDriver(DeviceObj, Irp) == STATUS_PENDING) {
			KeWaitForSingleObject(&Event,
					      Executive,
					      KernelMode,
					      FALSE,
					      0);
			return Irp->IoStatus.Status;
		}
	}
	return StatusBlk.Status;
}

void
SCSI::
ScsiQueryCapacity(
	PDEVICE_OBJECT DeviceObj
)
{
	IO_STATUS_BLOCK StatusBlk;
	KEVENT Event;
	ULONG LogicalBlockAddress, BytesPerBlock;

	auto ReadCapacityData = (PREAD_CAPACITY_DATA) ExAllocatePoolWithTag(NonPagedPool,
										0x8,
										'iscs');
										//8, 	   /*	anoth. Arch */
										//'iscs'); /*	anoth. Arch */
	SCSI_PASS_THROUGH_DIRECT ScsiData;
	RtlSecureZeroMemory(&ScsiData, sizeof(ScsiData));

	ScsiData.Length = (USHORT) sizeof(SCSI_PASS_THROUGH_DIRECT);
	ScsiData.PathId = 0;
	ScsiData.TargetId = 1;
	ScsiData.Lun = 0;
	ScsiData.CdbLength = CDB10GENERIC_LENGTH;
	ScsiData.DataIn = SCSI_IOCTL_DATA_IN;
	ScsiData.SenseInfoLength = 0;
	ScsiData.DataTransferLength = 8ul;
	ScsiData.TimeOutValue = 2ul;
	ScsiData.DataBuffer = ReadCapacityData;
	ScsiData.SenseInfoOffset = 0ul;
	ScsiData.Cdb[0] = SCSIOP_READ_CAPACITY;

	KeInitializeEvent(&Event,
			  SynchronizationEvent,
			  FALSE);

	auto Irp = IoBuildDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
						 DeviceObj,
						 &ScsiData, sizeof(ScsiData),
						 &ScsiData, sizeof(ScsiData),
						 FALSE,
						 &Event,
						 &StatusBlk);
	if (Irp) {
		auto Status = IoCallDriver(DeviceObj, Irp);

		if (Status == STATUS_PENDING) 
		{
			KeWaitForSingleObject(&Event, 
			                      Executive,
					      KernelMode,
					      FALSE,
					      0);
			if (StatusBlk.Status == STATUS_SUCCESS)
				goto PrintShit;
		}
		
		if (NT_SUCCESS(Status)) {
		
		/* DbgPrints LogicalBlockAddr(int). And BytesPerBlock (int) */
		PrintShit:
			REVERSE_BYTES(&BytesPerBlock, &ReadCapacityData->BytesPerBlock);
			REVERSE_BYTES(&LogicalBlockAddress, &ReadCapacityData->LogicalBlockAddress);
			
			DbgPrint("[+] ---------------- Worked! ----------------- [+]"); // Extra print out's
			
			DbgPrint("LogicalBlockAddress: %d : "
					 "BytesPerBlock:%d\n", LogicalBlockAddress, BytesPerBlock);
			
		}
	}
	if (ReadCapacityData)
		ExFreePoolWithTag(ReadCapacityData, 'iscs');
	PsTerminateSystemThread(STATUS_SUCCESS);
}


NTSTATUS
SCSI::
InitializeSCSISystemThread()
{
	auto ThreadRoutine = [](PVOID Context)
	{
		UNREFERENCED_PARAMETER(Context);
		IO_STATUS_BLOCK StatusBlk;
		UNICODE_STRING TargetName = RTL_CONSTANT_STRING(L"\\Device\\Harddisk0\\DR0");
		OBJECT_ATTRIBUTES ObjAttrs;
		HANDLE FileHandle;
		PFILE_OBJECT FileObj;
		
		InitializeObjectAttributes(&ObjAttrs,
					   &TargetName,
					   OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
					   nullptr,
					   nullptr);
		
		auto Status = IoCreateFile(&FileHandle,
					   1,
					   &ObjAttrs,
					   &StatusBlk,
					   nullptr,
					   0,
					   FILE_SHARE_VALID_FLAGS,
					   FILE_OPEN,
					   0,
					   nullptr,
					   0,
					   CreateFileTypeNone,
					   0,
					   0x400);
		if (!NT_SUCCESS(Status))
			return;

		Status = ObReferenceObjectByHandle(FileHandle,
						   0,
						   *IoFileObjectType,
						   KernelMode,
						   (PVOID*) &FileObj,
						   nullptr);
		
		ZwClose(FileHandle);
		if (NT_SUCCESS(Status))
		{
			ObDereferenceObject(FileObj);
			auto DataBuffer = (BYTE*) ExAllocatePoolWithTag(NonPagedPool,
									0x200,
									'iScS');

			auto DevCapacity = (PREAD_CAPACITY_DATA) ExAllocatePoolWithTag(NonPagedPool,
										       0x8,
										       'iScS');
			RtlSecureZeroMemory(DataBuffer, sizeof(DataBuffer));
			RtlSecureZeroMemory(DevCapacity, sizeof(DevCapacity));

			/* demo to test since ScsiQueryCapacity works */
			SCSI::SendScsiCmd(FileObj->DeviceObject,
					  SCSIOP_READ_CAPACITY,
					  SCSI_IOCTL_DATA_IN,
					  DevCapacity,
					  8,
					  0,
					  0);
			
			ExFreePoolWithTag(DevCapacity, 'iScS');
			SCSI::SendScsiCmd(FileObj->DeviceObject,
					  SCSIOP_READ,
					  SCSI_IOCTL_DATA_IN,
					  DataBuffer,
					  0x200,
					  0,
					  1);
					/*------------------------------------------------------
					| SCSI::SendScsiCmd(FileObj->DeviceObject,
					| SCSIOP_READ,
					| SCSI_IOCTL_DATA_IN,
					| DataBuffer,
					| 0x200,
					| 0x08,
					| 1);
					|
					------------------------------------------------------*/
			
			ExFreePoolWithTag(DataBuffer, 'iScS');
			SCSI::ScsiQueryCapacity(FileObj->DeviceObject);
		}

	};

	HANDLE SysThreadHandle {};
	auto Status = PsCreateSystemThread(&SysThreadHandle,
					   THREAD_ALL_ACCESS,
					   nullptr,
					   nullptr,
					   nullptr,
					   ThreadRoutine,
					   nullptr);
	if (SysThreadHandle > 0)
		ZwClose(SysThreadHandle);
	return Status;
}
