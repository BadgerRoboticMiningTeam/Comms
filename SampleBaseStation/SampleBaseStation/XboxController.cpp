#include "XboxController.h"
#include <iostream>

constexpr int XBOX360_VENDOR_ID = 0x045E;
constexpr int XBOX360_PRODUCT_ID = 0x028E;

DIJOYSTATE joystickState;
LPDIRECTINPUTDEVICE8 joystick;
LPDIRECTINPUT8 di;
bool acquired = false;


static BOOL CALLBACK JoystickConfigCallback(const DIDEVICEOBJECTINSTANCE *instance, void *context)
{
    DIPROPRANGE propRange;
    LPDIRECTINPUTDEVICE8 joystick;

    if (!context)
        return DIENUM_STOP;

    memset(&propRange, 0, sizeof(DIPROPRANGE));
    propRange.diph.dwSize = sizeof(DIPROPRANGE);
    propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    propRange.diph.dwHow = DIPH_BYID;
    propRange.diph.dwObj = instance->dwType;
    propRange.lMin = -100;
    propRange.lMax = +100;

    // Set the range for the axis
    joystick = (LPDIRECTINPUTDEVICE8)context;
    if (FAILED(joystick->SetProperty(DIPROP_RANGE, &propRange.diph)))
        return DIENUM_STOP;

    return DIENUM_CONTINUE;
}

static BOOL CALLBACK EnumerateJoysticks(const DIDEVICEINSTANCE *instance, void *context)
{
    DIPROPDWORD dipdw;

    if (FAILED(di->CreateDevice(instance->guidInstance, &joystick, nullptr)))
        return DIENUM_CONTINUE;

    DIPROPGUIDANDPATH jsGuidPath;
    jsGuidPath.diph.dwSize = sizeof(DIPROPGUIDANDPATH);
    jsGuidPath.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    jsGuidPath.diph.dwHow = DIPH_DEVICE;
    jsGuidPath.diph.dwObj = 0;

    if (FAILED(joystick->GetProperty(DIPROP_GUIDANDPATH, &jsGuidPath.diph)))
    {
        joystick->Release();
        return DIENUM_CONTINUE;
    }

    // check vendor and product ID
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;

    if (FAILED(joystick->GetProperty(DIPROP_VIDPID, &dipdw.diph)))
    {
        joystick->Release();
        return DIENUM_CONTINUE;
    }

    if (LOWORD(dipdw.dwData) != XBOX360_VENDOR_ID || HIWORD(dipdw.dwData) != XBOX360_PRODUCT_ID)
    {
        joystick->Release();
        return DIENUM_CONTINUE;
    }

    // create and start tracking joystick
    // use DIJOYSTATE struct for data acquisition
    if (FAILED(joystick->SetDataFormat(&c_dfDIJoystick)))
    {
        joystick->Release();
        return DIENUM_CONTINUE;
    }

    // axis configuration to -100 -> 100
    if (FAILED(joystick->EnumObjects(JoystickConfigCallback, joystick, DIDFT_AXIS)))
    {
        joystick->Release();
        return DIENUM_CONTINUE;
    }

    // new joystick - add to map & acquire
    acquired = true;
    joystick->Acquire();
    return DIENUM_STOP;
}

XboxController::XboxController()
{
    this->running = false;
}

XboxController::~XboxController()
{
    this->running = false;
    if (this->pollThread.joinable())
        this->pollThread.join();

    if (joystick)
    {
        joystick->Unacquire();
        joystick->Release();
    }

    if (di)
        di->Release();
}

bool XboxController::Acquire()
{
    HRESULT hr;

    if (acquired)
        return false;

    // initialize directinput
    hr = DirectInput8Create(
        GetModuleHandle(nullptr),
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void **)&di,
        nullptr
    );

    if (FAILED(hr))
        return false;

    di->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumerateJoysticks, nullptr, DIEDFL_ATTACHEDONLY);
    if (!acquired)
        return false;

    this->running = true;
    this->pollThread = std::thread(&XboxController::PollJoystick, this);
    return true;
}

int XboxController::GetLeftX()
{
    if (!acquired)
        return 0;

    return joystickState.lX;
}

int XboxController::GetLeftY()
{
    if (!acquired)
        return 0;
    
    return joystickState.lY;
}

int XboxController::GetRightX()
{
    if (!acquired)
        return 0;

    return -joystickState.lRx;
}

int XboxController::GetRightY()
{
    if (!acquired)
        return 0;

    return -joystickState.lRy;
}

void XboxController::PollJoystick()
{
    if (!acquired)
        return;

    while (this->running)
    {
        HRESULT hr;
        DIJOYSTATE js;

        hr = joystick->Poll();
        if (FAILED(hr))
        {
            do
            {
                hr = joystick->Acquire();
            } while (hr == DIERR_INPUTLOST);

            // joystick fatal error
            if (hr == DIERR_INVALIDPARAM ||
                hr == DIERR_NOTINITIALIZED ||
                hr == DIERR_OTHERAPPHASPRIO)
            {
                std::cout << "Warning! Joystick reported fatal error! Error: " << GetLastError() << std::endl;
                continue;
            }
        }

        if (SUCCEEDED(joystick->GetDeviceState(sizeof(DIJOYSTATE), &js)))
            joystickState = js;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}