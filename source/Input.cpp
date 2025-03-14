#include "Input.hpp"

void Input::Update(void)
{
    hidScanInput();
}

bool Input::IsPressed(Key key)
{
    return hidKeysDown() & key;
}

bool Input::IsHeld(Key key)
{
    return hidKeysHeld() & key;
}

bool Input::IsReleased(Key key)
{
    return hidKeysUp() & key;
}

bool Input::IsTouched(TouchPos p0, u16 width, u16 height)
{
    TouchPos p = GetTouchPos();
    return p.x >= p0.x && p.x <= p0.x + width && p.y >= p0.y && p.y <= p0.y + height;
}

TouchPos Input::GetTouchPos(void)
{
    TouchPos pos;
    hidTouchRead((touchPosition *)&pos);
    return pos;
}