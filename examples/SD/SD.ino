/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <Arduino.h>
#include <M5StamPLC.h>
#include <SD.h>

void setup()
{
    delay(3000);

    /* Enable SD card */
    auto config         = M5StamPLC.config();
    config.enableSdCard = true;
    M5StamPLC.config(config);
    M5StamPLC.begin();
}

void loop()
{
    static int count = 0;

    // Write file
    printf("\nSD card write test\n");
    auto file = SD.open("/test.txt", FILE_WRITE, true);
    if (file) {
        file.printf("Hello, World! Count: %d\n", count);
        file.close();
        printf("SD card write success\n");
    } else {
        printf("Failed to open file\n");
    }

    // Read file
    printf("\nSD card read test\n");
    file = SD.open("/test.txt");
    if (file) {
        printf("SD card read success:\n");
        while (file.available()) {
            printf("%c", file.read());
        }
    } else {
        printf("Failed to open file\n");
    }

    count++;
    delay(1000);
}
