/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <Arduino.h>
#include <M5GFX.h>
#include <queue>
#include <array>

class DashboardUI {
public:
    DashboardUI();
    ~DashboardUI();

    /* UI values */
    std::string statusTime;
    std::string statusDate;
    std::array<int, 8> inputStateList;
    std::array<int, 4> relayStateList;

    void init(LovyanGFX* parent);
    void render();
    void console_log(const std::string& msg, bool autoNewLine = true);

protected:
    LGFX_Sprite* _canvas          = nullptr;
    LGFX_Sprite* _terminal_canvas = nullptr;
    std::queue<char> _console_queue;
    uint32_t _console_msg_pop_out_interval   = 20;
    uint32_t _console_msg_pop_out_time_count = 0;
    uint32_t _cursor_blink_interval          = 500;
    uint32_t _cursor_time_count              = 0;
    bool _current_cursor_state               = false;

    void render_status_panel();
    void render_console_panel();
    void render_io_panel();
};
