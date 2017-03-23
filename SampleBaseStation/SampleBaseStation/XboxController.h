#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <windows.h>
#include <thread>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

enum class POV : int
{
    POV_NONE = 0,                               /**< POV hat is not in use.              */
    POV_WEST = 1 << 0,                          /**< POV hat is facing left.             */
    POV_EAST = 1 << 1,                          /**< POV hat is facing right.            */
    POV_NORTH = 1 << 2,                         /**< POV hat is facing up.               */
    POV_SOUTH = 1 << 3,                         /**< POV hat is facing down.             */
    POV_NORTHWEST = POV_NORTH | POV_WEST,       /**< POV hat is facing up and left.      */
    POV_NORTHEAST = POV_NORTH | POV_EAST,       /**< POV hat is facing up and right.     */
    POV_SOUTHWEST = POV_SOUTH | POV_WEST,       /**< POV hat is facing south and left.   */
    POV_SOUTHEAST = POV_SOUTH | POV_EAST        /**< POV hat is facing south and right.  */
};

class XboxController
{
public:
    XboxController();
    ~XboxController();
    bool Acquire();
    int GetLeftX();
    int GetLeftY();
    int GetRightX();
    int GetRightY();

private:
    void PollJoystick();

    std::thread pollThread;
    bool running;
};