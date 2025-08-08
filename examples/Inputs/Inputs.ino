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
    static std::array<bool, 8> input_list;

    // Read inputs
    for (int i = 0; i < 8; i++) {
        input_list[i] = M5StamPLC.readPlcInput(i);
    }

    // Print input reading result
    printf("Input: %d, %d, %d, %d, %d, %d, %d, %d\n", input_list[0], input_list[1], input_list[2], input_list[3],
           input_list[4], input_list[5], input_list[6], input_list[7]);

    delay(1000);
}