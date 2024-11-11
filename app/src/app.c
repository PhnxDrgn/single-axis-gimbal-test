#include "main.h"
#include "app.h"
#include <stdint.h>

void app()
{
    while (1)
    {
        static uint32_t refMillis = 0;

        uint32_t currentMillis = millis();

        if (currentMillis - refMillis > 1000)
        {
            refMillis = currentMillis;
            toggleLed();
        }
    }
}