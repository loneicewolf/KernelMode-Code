#pragma once

namespace GROK {

    ///<summary>
    ///main data for enabling driver to re-map itself
    ///</summary>
    struct HIDDEN_DRIVER_INFO
    {
        ULONG  DriverSize;
        ULONG  FinalSize;
        uint8_t* CompressedDriver;
    };

    enum class IOCTL : ULONG
    {
        SendCompressBuffer = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_IN_DIRECT, FILE_READ_DATA)
    };

    ///<summary> 
    ///compress driver buffer and store relevant info in HIDDEN_DRIVER_INFO
    ///</summary>
    bool CompressBuffer(
        uint8_t* GROK_UnCompressedDriver,
        HIDDEN_DRIVER_INFO& HiddenDriverInfo);

    ///<summary>
    ///load driver from ...\drivers directory
    ///using fixed GROK::Device::DRIVER_DIRECTORY | DRIVER_NAME vars
    ///</summary>
    bool LoadDriver(std::wstring const& DriverName);

    
    class Device
    {
    public:
        static std::wstring const DRIVER_DIRECTORY;
        static std::wstring const DRIVER_NAME;
        static std::wstring const DRIVER_PATH;

    public:
        Device(
            std::wstring const& DeviceName,
            uint32_t AccessType);

        explicit operator bool() const
        {
            return hDevice_ == INVALID_HANDLE_VALUE ? false : true;
        }

        bool ctrlDispatch(
            IOCTL ControlCode,
            void* InputBuf,
            size_t InputBufSize,
            void* OutputBuf = nullptr,
            size_t OutputBufSize = 0);

    private:
        HANDLE hDevice_ = INVALID_HANDLE_VALUE;
        std::wstring deviceName_;
        uint32_t accessType_;

    };
}


