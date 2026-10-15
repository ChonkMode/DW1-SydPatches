#pragma once

#include "AtlasFont.hpp"
#include "UIBox.hpp"

constexpr int16_t TEXTBOX_INSET_X     = 5;
constexpr int16_t TEXTBOX_INSET_Y     = 4;
constexpr int16_t TEXTBOX_LINE_HEIGHT = 13;

struct TextboxOption
{
    RECT bounds{};
    int32_t value{};
};

// Owners retain the concrete type; this hierarchy does not use virtual dispatch.
class Textbox
{
public:
    Textbox(RECT bounds,
            dtl::runtime_array<AtlasString> text,
            dtl::optional<RECT> origin = {},
            RGB8 color                 = UIBox::DEFAULT_COLOR,
            bool semiTrans             = false);
    void tick();
    void close();
    void render(int32_t depth);
    bool isOpened();
    bool isClosed();

protected:
    void renderText(int32_t depth);
    UIBox frame;
    RECT bounds;
    dtl::optional<RECT> origin;
    dtl::runtime_array<AtlasString> text;
};

class ConfirmTextbox : public Textbox
{
public:
    ConfirmTextbox(RECT bounds,
                   dtl::runtime_array<AtlasString> text,
                   dtl::optional<RECT> origin = {},
                   RGB8 color                 = UIBox::DEFAULT_COLOR,
                   bool semiTrans             = false);
    void tick();
    void update(bool closeOnResult = true);
    void render(int32_t depth);
    // Complete one authored page while retaining the existing animated frame.
    void replacePage(dtl::runtime_array<AtlasString> page);
    bool hasResult() const { return completed; }

protected:
    bool completed{};

private:
    uint8_t indicatorCounter{};
};

class SelectTextbox : public ConfirmTextbox
{
public:
    SelectTextbox(RECT bounds,
                  dtl::runtime_array<AtlasString> text,
                  dtl::runtime_array<TextboxOption> options,
                  dtl::optional<RECT> origin = {},
                  RGB8 color                 = UIBox::DEFAULT_COLOR,
                  bool semiTrans             = false,
                  uint32_t initial           = 0);
    using Textbox::tick;
    void update();
    void close();
    void render(int32_t depth);
    int32_t result() const { return completedValue; }
    uint32_t selection() const { return selected; }

private:
    dtl::runtime_array<TextboxOption> options;
    uint32_t selected;
    int32_t completedValue{};
};
