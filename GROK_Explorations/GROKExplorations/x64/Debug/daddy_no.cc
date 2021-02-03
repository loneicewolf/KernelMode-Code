
/* BASED OFF OF THIS DLL INJECTION TECHNIQUE:
 * https://github.com/stormshield/Beholder-Win32/blob/master/Beholder's%20Head/Injector.cpp
 *
 */
 
 
 // NOTE - FORKED VERSION HERE: Will add some other things as well: https://github.com/loneicewolf/Beholder-Win32
 //  https://github.com/loneicewolf/Beholder-Win32/blob/master/Beholder's%20Head/Injector.cpp

NTSTATUS InjectDll( PVOID K32Address,
                    SIZE_T K32Size )
{
    ULONG const SECTION_PROPERTIES{
        SECTION_MAP_READ |
        SECTION_MAP_EXECUTE |
        SECTION_QUERY
    };

    // TODO(Nean): Make it target any process, specified by a client
    //             in user-mode, through a dispatch routine
    auto status = ObOpenObjectByPointer( PsGetCurrentProcess(),
                                         OBJ_KERNEL_HANDLE,
                                         nullptr,
                                         STANDARD_RIGHTS_READ,
                                         nullptr,
                                         KernelMode,
                                         &processHandle );

    if ( !NT_SUCCESS( status ) )
    {
        return status;
    }

    OBJECT_ATTRIBUTES objAttrs = RTL_CONSTANT_ATTRIBUTES( nullptr,
                                                          OBJ_KERNEL_HANDLE );

    status = ZwCreateSection( &dllSectionHandle,
                              SECTION_PROPERTIES,
                              &objAttrs,
                              nullptr,
                              PAGE_EXECUTE_READWRITE,
                              SEC_IMAGE,
                              is64 ? g_dllHandle64 : g_dllHandle32 );

    if ( !NT_SUCCESS( status ) )
    {
        ZwClose( processHandle );
        return status;
    }

    status = ZwMapViewOfSection( dllSectionHandle,
                                 processHandle,
                                 &dllMappingAddress,
                                 0,
                                 0,
                                 nullptr,
                                 &viewSize,
                                 ViewUnmap,
                                 0,
                                 PAGE_EXECUTE_READ );

    if ( !NT_SUCCESS( status ) )
    {
        ZwClose( dllSectionHandle );
        ZwClose( processHandle );
        return status;
    }

    LARGE_INTEGER mappingSize{};
    mappingSize.QuadPart = PAGE_SIZE;

    status = ZwCreateSection( &inputSectionHandle,
                              SECTION_MAP_READ | SECTION_QUERY,
                              &objAttrs,
                              &mappingSize,
                              PAGE_READONLY,
                              SEC_COMMIT | SEC_NO_CHANGE,
                              nullptr );

    if ( !NT_SUCCESS( status ) )
    {
        ZwUnmapViewOfSection( processHandle, dllSectionHandle );
        ZwClose( dllSectionHandle );
        ZwClose( processHandle );
        return status;
    }

    PVOID inputMappingAddr{};
    viewSize = PAGE_SIZE;

    status = ZwMapViewOfSection( inputSectionHandle,
                                 processHandle,
                                 &inputMappingAddr,
                                 0,
                                 PAGE_SIZE,
                                 0,
                                 &viewSize,
                                 ViewUnmap,
                                 0,
                                 PAGE_READONLY );

    if ( !NT_SUCCESS( status ) )
    {
        ZwUnmapViewOfSection( processHandle, dllSectionHandle );
        ZwClose( inputSectionHandle );
        ZwClose( dllSectionHandle );
        ZwClose( processHandle );
        return status;
    }

    auto paramMdl = IoAllocateMdl( inputMappingAddr,
                                   PAGE_SIZE,
                                   FALSE,
                                   FALSE,
                                   nullptr );

    if ( paramMdl == nullptr )
    {
        ZwUnmapViewOfSection( processHandle, inputMappingAddr );
        ZwUnmapViewOfSection( processHandle, dllSectionHandle );
        ZwClose( inputSectionHandle );
        ZwClose( dllSectionHandle );
        ZwClose( processHandle );
        return status;
    }

    __try
    {
        MmProbeAndLockPages( paramMdl, UserMode, IoReadAccess );
    }
    __except ( EXCEPTION_EXECUTE_HANDLE )
    {
        IoFreeMdl( paramMdl );
        ZwUnmapViewOfSection( processHandle, inputMappingAddr );
        ZwUnmapViewOfSection( processHandle, dllSectionHandle );
        ZwClose( inputSectionHandle );
        ZwClose( dllSectionHandle );
        ZwClose( processHandle );
        return status;
    }

    mdlSystemAddr = MmGetSystemAddressForMdlSafe( paramMdl, NormalPagePriority );
    if ( mdlSystemAddr == nullptr )
    {
        MmUnlockPages( paramMdl );
        IoFreeMdl( paramMdl );
        ZwUnmapViewOfSection( processHandle, inputMappingAddr );
        ZwUnmapViewOfSection( processHandle, dllSectionHandle );
        ZwClose( inputSectionHandle );
        ZwClose( dllSectionHandle );
        ZwClose( processHandle );
        return status;
    }

    SecureZeroMemory( mdlSystemAddr, PAGE_SIZE );

    dllParams             = (PDLL_PARAMS) mdlSystemAddr;
    dllParams->K32Address = K32Address;
    dllParams->K32Size    = K32Size;

    MmUnlockPages( paramMdl );
    IoFreeMdl( paramMdl );
}

