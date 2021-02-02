#pragma once


namespace GROK {

    int32_t const DRIVER_TO_LOAD { 6969 };

    class Resource
    {
    public:
        Resource() = delete;
        Resource(HMODULE Module, int Resource, LPCWSTR Type = RT_RCDATA);
        ~Resource();
        Resource(Resource const & _) = delete;
        auto operator=(Resource const & _) = delete;
        explicit operator bool() const noexcept
        {
            return handle_ != (HANDLE)0;
        }

        uint8_t* getBuffer() const { return buffer_; }
        bool writeToFile(std::wstring const&) const;
    private:
        HRSRC resource_ {};
        HGLOBAL handle_ {};
        uint8_t* buffer_ {};
        DWORD size_;
    };
}

