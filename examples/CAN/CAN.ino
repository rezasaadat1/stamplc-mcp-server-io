/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <Arduino.h>
#include <M5StamPLC.h>

void setup()
{
    /* Enable CAN */
    auto config        = M5StamPLC.config();
    config.enableCan   = true;
    config.canBaudRate = 1000000;
    M5StamPLC.config(config);

    /* Init M5StamPLC */
    M5StamPLC.begin();
}

void loop()
{
    static uint32_t sending_time_count = 0;

    /* Send message */
    if (millis() - sending_time_count > 1000) {
        twai_message_t message;
        message.identifier       = 0xAAAA;
        message.extd             = 1;
        message.data_length_code = 8;
        for (int i = 0; i < message.data_length_code; i++) {
            message.data[i] = i;
        }

        if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
            printf(">> Message queued for transmission\n");
        } else {
            printf("Failed to queue message for transmission\n");
        }

        sending_time_count = millis();
    }

    /* Receive message */
    twai_message_t message;
    if (twai_receive(&message, pdMS_TO_TICKS(500)) == ESP_OK) {
        printf("<< Message received\n");

        if (message.extd) {
            printf("Message is in Extended Format\n");
        } else {
            printf("Message is in Standard Format\n");
        }

        printf("ID is %d\n", message.identifier);

        if (!(message.rtr)) {
            printf("Data: ");
            for (int i = 0; i < message.data_length_code; i++) {
                printf("%d ", message.data[i]);
            }
            printf("\n");
        }
    }
}
