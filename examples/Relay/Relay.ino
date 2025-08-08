/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <Arduino.h>
#include <M5StamPLC.h>

void setup()
{
    /* Init M5StamPLC */
    M5StamPLC.begin();
}

void loop()
{
    static bool relay_state = false;

    /* Toggle relay state */
    relay_state = !relay_state;
    for (int i = 0; i < 4; i++) {
        M5StamPLC.writePlcRelay(i, relay_state);
        printf("Write Relay %d to %s\n", i, relay_state ? "ON" : "OFF");
        delay(500);
    }

    delay(1000);
}