#pragma once

#include "Textbox.hpp"

inline constexpr RECT TAMER_NOTICE_BOUNDS{-130, 42, 262, 59};

void prepareTamerNotice(const TextboxRow* rows, uint32_t rowCount);
bool openTamerNotice(RECT origin);
void closeTamerNotice(RECT target);
void cancelTamerNotice();
bool isTamerNoticeClosing();
