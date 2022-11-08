#include "Screen.h"

using namespace Paimon;

int Screen::s_width;
int Screen::s_height;
float Screen::s_aspect;

void Screen::SetWidth(int width)
{
    s_width = width;
    s_aspect = static_cast<float>(s_width) / static_cast<float>(s_height);
}

void Screen::SetHeight(int height) {
    s_height = height;
    s_aspect = static_cast<float>(s_width) / static_cast<float>(s_height);
}

void Screen::SetSize(int width, int height)
{
    s_width = width;
    s_height = height;
    s_aspect = static_cast<float>(s_width) / static_cast<float>(s_height);
}
