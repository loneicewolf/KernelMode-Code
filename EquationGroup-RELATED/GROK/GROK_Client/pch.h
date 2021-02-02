
#ifndef PCH_H
#define PCH_H

#include <Windows.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

using namespace std::literals::string_literals;

static
std::wstring
LastErrorToString(
    const DWORD MsgError = ::GetLastError()
)
{
    if (!MsgError)
    {
        return  {};
    }

    wchar_t* Buffer {};
    const DWORD Flags {
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS
    };
    const auto Count = ::FormatMessageW(Flags,
                                        nullptr,
                                        MsgError,
                                        MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                                        (LPWSTR)&Buffer,
                                        0,
                                        nullptr);
    std::wstring Message(Buffer, Count);
    ::HeapFree(::GetProcessHeap(), 0, Buffer);
    return Message;
}


struct WinError
{
    WinError()
        : ErrorMsg { LastErrorToString() }
    {
        ::wprintf(ErrorMsg.c_str());
    }
    WinError(const std::wstring& ExtraInfo)
        : ErrorMsg { LastErrorToString() }
    {
        ::wprintf((ExtraInfo + L"\n\t" + ErrorMsg).c_str());
    }
    std::wstring ErrorMsg { L"No Errors."s };
};

#pragma comment( lib, "ntdll" )
#define NT_SUCCESS  (LONG)0
extern "C"
{
    NTSTATUS RtlCompressBuffer(
        USHORT CompressionFormatAndEngine,
        PUCHAR UncompressedBuffer,
        ULONG  UncompressedBufferSize,
        PUCHAR CompressedBuffer,
        ULONG  CompressedBufferSize,
        ULONG  UncompressedChunkSize,
        PULONG FinalCompressedSize,
        PVOID  WorkSpace
    );

    NTSTATUS RtlGetCompressionWorkSpaceSize(USHORT, PULONG, PULONG);
    PIMAGE_NT_HEADERS RtlImageNtHeader(PVOID);
}


#endif //PCH_H
