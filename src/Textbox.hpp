#pragma once

#include "AtlasFont.hpp"
#include "UIBox.hpp"

constexpr int16_t TEXTBOX_INSET_X     = 5;
constexpr int16_t TEXTBOX_INSET_Y     = 4;
constexpr int16_t TEXTBOX_LINE_HEIGHT = 13;

// Owners retain the concrete type; prepared rows have a logical origin of (0, 0).
class Textbox
{
public:
    Textbox(RECT bounds,
            dtl::runtime_array<AtlasString> text,
            dtl::optional<RECT> origin = {},
            UIBox::Style style         = {});
    void tick();
    void openFrom(RECT start);
    bool isClosing() const { return frame.getState() == UIBox::State::CLOSING; }
    void close(dtl::optional<RECT> target = {});
    void replacePage(dtl::runtime_array<AtlasString> page);
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

private:
    void layoutText();
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

class SelectTextbox : public ConfirmTextbox
{
public:
    // Each option names a valid logical row; the widest option determines the cursor width.
    SelectTextbox(RECT bounds,
                  dtl::runtime_array<AtlasString> text,
                  dtl::runtime_array<uint32_t> options,
                  dtl::optional<RECT> origin = {},
                  UIBox::Style style         = {},
                  uint32_t initial           = 0);
    dtl::optional<uint32_t> tick();
    void render(int32_t depth);
    uint32_t selection() const { return options.size() == 0 ? 0 : options[selected]; }

private:
    using Textbox::replacePage;
    dtl::runtime_array<uint32_t> options;
    uint32_t selected;
    int32_t cursorWidth{};
};
