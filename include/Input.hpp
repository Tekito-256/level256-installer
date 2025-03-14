#pragma once

#include <3ds.h>

enum Key
{
    A = 1,
    B = 1 << 1,
    Select = 1 << 2,
    Start = 1 << 3,
    DPadRight = 1 << 4,
    DPadLeft = 1 << 5,
    DPadUp = 1 << 6,
    DPadDown = 1 << 7,
    R = 1 << 8,
    L = 1 << 9,
    X = 1 << 10,
    Y = 1 << 11,
    ZL = 1 << 14,
    ZR = 1 << 15,
    Touchpad = 1 << 20,
    CStickRight = 1 << 24,
    CStickLeft = 1 << 25,
    CStickUp = 1 << 26,
    CStickDown = 1 << 27,
    CPadRight = 1 << 28,
    CPadLeft = 1 << 29,
    CPadUp = 1 << 30,
    CPadDown = 1 << 31,
};

struct TouchPos
{
    u16 x;
    u16 y;
};

class Input
{
public:
    static void Update(void);
    static bool IsPressed(Key key);
    static bool IsHeld(Key key);
    static bool IsReleased(Key key);
    static bool IsTouched(TouchPos p0, u16 width, u16 height);
    static TouchPos GetTouchPos(void);
};