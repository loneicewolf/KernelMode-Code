#include "pch.h"
#include "GROK_Device.h"
#include "GROK_Resource.hpp"

std::wstring const GROK::Device::DRIVER_NAME      = L"GROK.sys"s;
std::wstring const GROK::Device::DRIVER_DIRECTORY = LR"(C:\Windows\System32\drivers\)"s;
std::wstring const GROK::Device::DRIVER_PATH      = GROK::Device::DRIVER_DIRECTORY + GROK::Device::DRIVER_NAME;

GROK::Device::Device(
    std::wstring const& DeviceName, // LR"(\\.\Grok)"
    uint32_t AccessType
) : deviceName_ { DeviceName }, accessType_ { AccessType}
{
    if (GROK::LoadDriver(L"GROK"))
    {
        ::Sleep(10);
        hDevice_ = ::CreateFileW(deviceName_.c_str(),
                                 accessType_,
                                 FILE_SHARE_READ,
                                 nullptr,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 nullptr);
        if (hDevice_ == INVALID_HANDLE_VALUE)
            WinError(L"CreateFile");
    }
    
}

bool GROK::Device::ctrlDispatch(
    GROK::IOCTL ControlCode, 
    void * InputBuf, 
    size_t InputBufSize, 
    void * OutputBuf, 
    size_t OutputBufSize
)
{
    DWORD _ {};
    return DeviceIoControl(hDevice_, 
                           (DWORD)ControlCode, 
                           InputBuf, InputBufSize,
                           OutputBuf, OutputBufSize, 
                           &_, 
                           nullptr);
}


bool GROK::LoadDriver(
    std::wstring const& GrokName
)
{
    auto ScManager = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (!ScManager)
    {
        WinError(L"OpenSCManager");
        return false;
    }
    
    auto Service =
        ::CreateService(ScManager,
                        GrokName.c_str(), GrokName.c_str(),
                        SERVICE_ALL_ACCESS,
                        SERVICE_KERNEL_DRIVER,
                        SERVICE_DEMAND_START,
                        SERVICE_ERROR_NORMAL,
                        (GROK::Device::DRIVER_DIRECTORY + GROK::Device::DRIVER_NAME).c_str(),
                        nullptr,
                        nullptr,
                        nullptr,
                        nullptr,
                        nullptr);

    if (Service)
    {
        return ::StartServiceW(Service, 0, nullptr) || ::GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;
    } else
    {
        WinError(L"CreateService");
        return false;
    }
}

bool GROK::CompressBuffer(
    uint8_t* GROK_UnCompressedDriver,
    HIDDEN_DRIVER_INFO& HiddenDriverInfo
)
{
    bool success { false };

    ULONG wsSize, fsSize;
    ::RtlGetCompressionWorkSpaceSize(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM,
                                     &wsSize, &fsSize);
    auto workspace = ::malloc(wsSize);

    if (::RtlCompressBuffer(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM,
                            GROK_UnCompressedDriver,
                            HiddenDriverInfo.DriverSize,
                            HiddenDriverInfo.CompressedDriver,
                            HiddenDriverInfo.DriverSize,
                            0x1000,
                            &HiddenDriverInfo.FinalSize,
                            workspace) == NT_SUCCESS)
    {
        success = true;
    }
    ::free(workspace);
    return success;
}