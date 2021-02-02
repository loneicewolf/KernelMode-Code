#include "pch.h"
#include "GROK_Resource.hpp"


GROK::Resource::Resource(
    HMODULE Module,
    int Resource,
    LPCWSTR Type
)
    : resource_ { ::FindResourceW(Module, MAKEINTRESOURCEW(Resource), Type) }
{
    if (resource_)
    {
        handle_ = ::LoadResource(Module, resource_);
        if (handle_)
        {
            buffer_ = (uint8_t*)::LockResource(handle_);
            size_   = ::SizeofResource(Module, resource_);
        }
    }
}

GROK::Resource::~Resource()
{
    if (handle_)
    {
        ::FreeResource(handle_);
    }
}

bool GROK::Resource::writeToFile(
    std::wstring const& FilePath
) const
{
    bool success { false };
    auto hFile = ::CreateFile(FilePath.c_str(),
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              nullptr,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              nullptr);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD bytesRead {};
        if (::WriteFile(hFile, buffer_, size_, &bytesRead, nullptr) &&
                        bytesRead == size_)
        {
            success = true;
        }
        ::CloseHandle(hFile);
    }

    return success;
}