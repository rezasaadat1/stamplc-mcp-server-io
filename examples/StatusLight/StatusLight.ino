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
    /* Set status light to red */
    M5StamPLC.setStatusLight(1, 0, 0);
    printf("Set status light to red\n");
    delay(1000);

    /* Set status light to green */
    M5StamPLC.setStatusLight(0, 1, 0);
    printf("Set status light to green\n");
    delay(1000);

    /* Set status light to blue */
    M5StamPLC.setStatusLight(0, 0, 1);
    printf("Set status light to blue\n");
    delay(1000);

    /* Set status light to white */
    M5StamPLC.setStatusLight(1, 1, 1);
    printf("Set status light to white\n");
    delay(1000);

    /* Set status light to black */
    M5StamPLC.setStatusLight(0, 0, 0);
    printf("Set status light to black\n");
    delay(1000);
}