#pragma once

class cfg
{
public:
    // Visual
    bool ESP = true;

    bool vBox = true;
    bool vLine = false;
    bool vHealth = true;
    bool vDistance = true;
    bool vName = false;

    bool aimbot = true;

    int fov = 200;
    bool fovCircle = true;
    float speed = 0.1;
};

extern cfg g;