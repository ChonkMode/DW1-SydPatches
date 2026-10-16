#pragma once

#include "AtlasFont.hpp"
#include "UIBox.hpp"

// Text must be non-null and NUL-terminated. Descriptors and strings are borrowed only for the call.
struct TextboxLine
{
    const char* text = "";
    RGB8 color       = TEXT_COLOR_WHITE;
};

// Runs and rows are borrowed only during synchronous text preparation.
struct TextboxRun
{
    const char* text = "";
    RGB8 color       = TEXT_COLOR_WHITE;
    int16_t gapBefore{};
    bool visible = true;
};

struct TextboxRow
{
    const TextboxRun* runs{};
    uint32_t runCount{};
};

// Owners retain the concrete type; textboxes own the rendered glyphs.
// TODO: replace lines pointer and lineCount with something like span/ranges
class Textbox
{
public:
    Textbox(RECT bounds,
            const TextboxLine* lines,
            uint32_t lineCount,
            dtl::optional<RECT> origin = {},
            UIBox::Style style         = {});
    Textbox(RECT bounds,
            const TextboxRow* rows,
            uint32_t rowCount,
            dtl::optional<RECT> origin = {},
            UIBox::Style style         = {});
    void tick();
    void openFrom(RECT start);
    bool isClosing() const { return frame.getState() == UIBox::State::CLOSING; }
    void close(dtl::optional<RECT> target = {});
    void replacePage(const TextboxLine* lines, uint32_t lineCount);
    void replacePage(const TextboxRow* rows, uint32_t rowCount);
    void render(int32_t depth);
    bool isOpened();
    bool isClosed();

protected:
    void renderText(int32_t depth);
    UIBox frame;
    RECT bounds;
    dtl::optional<RECT> origin;
    UIBox::Style style;
    dtl::runtime_array<AtlasString> text;
};

class ConfirmTextbox : public Textbox
{
public:
    using Textbox::Textbox;
    bool tick();
    void render(int32_t depth);

private:
    uint8_t indicatorCounter{};
};

class SelectTextbox : public Textbox
{
public:
    // Each option names a valid logical row; the widest option determines the cursor width.
    SelectTextbox(RECT bounds,
                  const TextboxLine* lines,
                  uint32_t lineCount,
                  dtl::runtime_array<uint32_t> options,
                  dtl::optional<RECT> origin = {},
                  UIBox::Style style         = {},
                  uint32_t initial           = 0);
    dtl::optional<uint32_t> tick();
    void render(int32_t depth);
    uint32_t hoveredOption() const { return selected; }

private:
    using Textbox::replacePage;
    uint32_t selectedRow() const { return options[selected]; }
    dtl::runtime_array<uint32_t> options;
    uint32_t selected;
    int32_t cursorWidth{};
};
