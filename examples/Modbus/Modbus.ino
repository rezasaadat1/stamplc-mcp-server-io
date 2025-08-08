/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <Arduino.h>
#include <M5StamPLC.h>

void setup()
{
    /* Enable Modbus */
    auto config              = M5StamPLC.config();
    config.enableModbusSlave = true;
    config.modbusBaudRate    = 115200;
    config.modbusSlaveId     = 1;
    M5StamPLC.config(config);

    /* Init M5StamPLC */
    M5StamPLC.begin();
}

void loop()
{
    /* Now you can controll StamPLC via Modbus RS485 */
    /*
     * Modbus Slave Configuration for M5StamPLC
     * =======================================
     *
     * Register Map:
     * -------------
     * 1. Coils (Read/Write)
     *    - Address 0: Relay 1 output (true/false)
     *    - Address 1: Relay 2 output (true/false)
     *    - Address 2: Relay 3 output (true/false)
     *    - Address 3: Relay 4 output (true/false)
     *
     * 2. Input Registers (Read-only)
     *    - Address 0-7:   Inputs (true/false) - 8 registers
     *    - Address 8-9:   Temperature (FLOAT32) - 2 registers
     *    - Address 10-11: Bus Voltage (FLOAT32) - 2 registers
     *    - Address 12-13: Shunt Current (FLOAT32) - 2 registers
     *
     * Note: FLOAT32 values use 2 consecutive registers (32 bits total)
     */
    delay(1000);
}
