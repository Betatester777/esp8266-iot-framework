#ifndef __UPDATER_H__
#define __UPDATER_H__

#include <Arduino.h>

class LittleFSUpdater
{

private:
    String filename;
    bool requestFlag = false;
    uint8_t status = 255;
    void flash(String filename);

public:
    void requestStart(String filename);
    void loop();
    uint8_t getStatus();
};

extern LittleFSUpdater updater;

#endif