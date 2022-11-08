#pragma once

namespace Paimon
{

class Screen
{
public:
    static void SetWidth(int width);
    static int GetWidth() { return s_width; }

    static void SetHeight(int height);
    static int GetHeight() { return s_height; }

    static void SetSize(int width, int height);

    static float GetAspect() { return s_aspect; }

private:
    static int s_width;
    static int s_height;
    static float s_aspect;
}; // class Screen

} // namespace Paimon
