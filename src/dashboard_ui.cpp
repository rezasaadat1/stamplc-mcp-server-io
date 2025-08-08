/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "dashboard_ui.h"
#include <M5GFX.h>
#include <queue>
#include "font_montserrat_semibolditalic_10.h"
#include "font_montserrat_semibolditalic_12.h"
#include "img_tag_console.h"
#include "img_tag_io.h"

DashboardUI::DashboardUI()
{
}

DashboardUI::~DashboardUI()
{
    if (_canvas) {
        delete _canvas;
    }
    if (_terminal_canvas) {
        delete _terminal_canvas;
    }
}

void DashboardUI::init(LovyanGFX* parent)
{
    /* Create full screen canvas */
    _canvas = new LGFX_Sprite(parent);
    _canvas->createSprite(parent->width(), parent->height());
}

void DashboardUI::render()
{
    _canvas->fillScreen((uint32_t)0xF3FFD5);
    render_status_panel();
    render_console_panel();
    render_io_panel();
    _canvas->pushSprite(0, 0);
}

void DashboardUI::console_log(const std::string& msg, bool autoNewLine)
{
    if (autoNewLine) {
        _console_queue.push('\n');
    }
    for (const auto& i : msg) {
        _console_queue.push(i);
    }
}

void DashboardUI::render_status_panel()
{
    constexpr int panel_x                 = 2;
    constexpr int panel_y                 = 2;
    constexpr int panel_w                 = 79;
    constexpr int panel_h                 = 39;
    constexpr int panel_r                 = 6;
    constexpr int inner_pannel_mt         = 19;
    constexpr int inner_pannel_ml         = 2;
    constexpr int inner_pannel_h          = 18;
    constexpr int inner_pannel_r          = 4;
    constexpr uint32_t color_pannel       = 0x3A3A3A;
    constexpr uint32_t color_inner_pannel = 0xF3FFD5;

    /* Render panel */
    _canvas->fillSmoothRoundRect(panel_x, panel_y, panel_w, panel_h, panel_r, color_pannel);
    _canvas->fillSmoothRoundRect(panel_x + inner_pannel_ml, panel_y + inner_pannel_mt, panel_w - inner_pannel_ml * 2,
                                 inner_pannel_h, inner_pannel_r, color_inner_pannel);

    /* Render labels */
    _canvas->setTextSize(1);
    _canvas->setTextDatum(middle_center);
    _canvas->loadFont(montserrat_semibolditalic_12);

    _canvas->setTextColor(color_inner_pannel);
    _canvas->drawString(statusTime.c_str(), panel_x + panel_w / 2, panel_y + 9);
    _canvas->setTextColor(color_pannel);
    _canvas->drawString(statusDate.c_str(), panel_x + panel_w / 2, panel_y + 28);
}

void DashboardUI::render_console_panel()
{
    constexpr int panel_x                 = 0 + 2;
    constexpr int panel_y                 = 45 - 2;
    constexpr int panel_w                 = 79;
    constexpr int panel_h                 = 90;
    constexpr int panel_r                 = 6;
    constexpr int inner_pannel_mt         = 2;
    constexpr int inner_pannel_ml         = 2;
    constexpr int inner_pannel_r          = 4;
    constexpr int canvas_w                = 65;
    constexpr int canvas_h                = 70;
    constexpr int canvas_mt               = 5;
    constexpr int canvas_ml               = 8;
    constexpr int tag_mt                  = 73;
    constexpr uint32_t color_pannel       = 0x3A3A3A;
    constexpr uint32_t color_inner_pannel = 0xF3FFD5;

    /* Create terminal canvas if not created */
    if (!_terminal_canvas) {
        _terminal_canvas = new LGFX_Sprite(_canvas);
        _terminal_canvas->createSprite(canvas_w, canvas_h);
        _terminal_canvas->setTextSize(1);
        _terminal_canvas->loadFont(montserrat_semibolditalic_10);
        _terminal_canvas->setTextScroll(true);
        _terminal_canvas->setTextColor(color_pannel);
        _terminal_canvas->setBaseColor(color_inner_pannel);
        _terminal_canvas->fillScreen(color_inner_pannel);

        // console_log("Console created :)", false);
    }

    // Update message pop out
    if (millis() - _console_msg_pop_out_time_count > _console_msg_pop_out_interval) {
        if (!_console_queue.empty()) {
            /* Cover old cursor */
            _terminal_canvas->fillRect(_terminal_canvas->getCursorX(), _terminal_canvas->getCursorY() + 10, 6, 3,
                                       color_inner_pannel);

            char c = 0;
            /* If queue is less than 60, pop out in every 20ms */
            if (_console_queue.size() < 60) {
                c = _console_queue.front();
                _console_queue.pop();
                _terminal_canvas->print(c);
            }
            /* If not, pop them all out */
            else {
                while (_console_queue.size() >= 60) {
                    c = _console_queue.front();
                    _console_queue.pop();
                    _terminal_canvas->print(c);
                }
            }

            /* New cursor */
            _terminal_canvas->fillRect(_terminal_canvas->getCursorX(), _terminal_canvas->getCursorY() + 10, 6, 2,
                                       color_pannel);
        }

        _console_msg_pop_out_time_count = millis();
    }

    /* Update cursor blink effect */
    if (millis() - _cursor_time_count > _cursor_blink_interval) {
        _current_cursor_state = !_current_cursor_state;
        _terminal_canvas->fillRect(_terminal_canvas->getCursorX(), _terminal_canvas->getCursorY() + 10, 6, 2,
                                   _current_cursor_state ? color_pannel : color_inner_pannel);
        _cursor_time_count = millis();
    }

    /* Render panel */
    _canvas->fillSmoothRoundRect(panel_x, panel_y, panel_w, panel_h, panel_r, color_pannel);
    _canvas->fillSmoothRoundRect(panel_x + inner_pannel_ml, panel_y + inner_pannel_mt, panel_w - inner_pannel_ml * 2,
                                 panel_h - inner_pannel_mt * 2, inner_pannel_r, color_inner_pannel);

    /* Render tag */
    _canvas->pushImage(panel_x, panel_y + tag_mt, 35, 17, image_data_img_tag_console);

    /* Push canvas */
    _terminal_canvas->pushSprite(panel_x + canvas_ml, panel_y + canvas_mt);
}

void DashboardUI::render_io_panel()
{
    constexpr int panel_x                 = 85 - 2;
    constexpr int panel_y                 = 0 + 2;
    constexpr int panel_w                 = 155;
    constexpr int pannel_input_h          = 55;
    constexpr int pannel_input_r          = 6;
    constexpr int inner_pannel_input_mt   = 2;
    constexpr int inner_pannel_input_ml   = 2;
    constexpr int inner_pannel_input_r    = 4;
    constexpr int pannel_output_mt        = 76;
    constexpr int tag_mt                  = 48;
    constexpr uint32_t color_pannel       = 0x3A3A3A;
    constexpr uint32_t color_inner_pannel = 0xF3FFD5;

    /* Render tag */
    _canvas->pushImage(panel_x, panel_y + tag_mt, 155, 35, image_data_img_tag_io);

    /* ------------------------------- Input panel ------------------------------ */
    constexpr int block_input_w                     = 14;
    constexpr int block_input_h                     = 39;
    constexpr int block_input_mt                    = 8;
    constexpr int block_input_ml                    = 8;
    constexpr int block_input_gap                   = 4;
    constexpr int block_input_r                     = 4;
    constexpr uint32_t color_block_input_high       = 0xFF6B6B;
    constexpr uint32_t color_block_input_low        = 0x29DC97;
    constexpr uint32_t color_block_input_label_high = 0xC53131;
    constexpr uint32_t color_block_input_label_low  = 0x11A25D;

    /* Panel */
    _canvas->fillSmoothRoundRect(panel_x, panel_y, panel_w, pannel_input_h, pannel_input_r, color_pannel);
    _canvas->fillSmoothRoundRect(panel_x + inner_pannel_input_ml, panel_y + inner_pannel_input_mt,
                                 panel_w - inner_pannel_input_ml * 2, pannel_input_h - inner_pannel_input_mt * 2,
                                 inner_pannel_input_r, color_inner_pannel);

    /* State blocks */
    for (int i = 0; i < inputStateList.size(); i++) {
        _canvas->fillSmoothRoundRect(panel_x + block_input_ml + (block_input_w + block_input_gap) * i,
                                     panel_y + block_input_mt, block_input_w, block_input_h, block_input_r,
                                     inputStateList[i] ? color_block_input_high : color_block_input_low);

        _canvas->setTextSize(1);
        _canvas->loadFont(montserrat_semibolditalic_12);
        _canvas->setTextDatum(middle_center);
        _canvas->setTextColor(inputStateList[i] ? color_block_input_label_high : color_block_input_label_low);
        _canvas->drawNumber(i + 1, panel_x + block_input_ml + (block_input_w + block_input_gap) * i + block_input_w / 2,
                            panel_y + block_input_mt + block_input_h / 2);
    }

    /* ------------------------------- Relay panel ------------------------------ */
    constexpr int block_output_w   = 32;
    constexpr int block_output_h   = 41;
    constexpr int block_output_mt  = 83;
    constexpr int block_output_ml  = 7;
    constexpr int block_output_gap = 4;
    constexpr int block_output_r   = 4;

    /* Panel */
    _canvas->fillSmoothRoundRect(panel_x, panel_y + pannel_output_mt, panel_w, pannel_input_h, pannel_input_r,
                                 color_pannel);
    _canvas->fillSmoothRoundRect(panel_x + inner_pannel_input_ml, panel_y + inner_pannel_input_mt + pannel_output_mt,
                                 panel_w - inner_pannel_input_ml * 2, pannel_input_h - inner_pannel_input_mt * 2,
                                 inner_pannel_input_r, color_inner_pannel);

    /* State blocks */
    for (int i = 0; i < relayStateList.size(); i++) {
        _canvas->fillSmoothRoundRect(panel_x + block_output_ml + (block_output_w + block_output_gap) * i,
                                     panel_y + block_output_mt, block_output_w, block_output_h, block_output_r,
                                     relayStateList[i] ? color_block_input_high : color_block_input_low);

        _canvas->setTextSize(1);
        _canvas->loadFont(montserrat_semibolditalic_12);
        _canvas->setTextDatum(middle_center);
        _canvas->setTextColor(relayStateList[i] ? color_block_input_label_high : color_block_input_label_low);
        _canvas->drawNumber(i + 1,
                            panel_x + block_output_ml + (block_output_w + block_output_gap) * i + block_output_w / 2,
                            panel_y + block_output_mt + block_output_h / 2);
    }
}
