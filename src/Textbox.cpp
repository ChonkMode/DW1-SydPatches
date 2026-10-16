#include "Textbox.hpp"

#include "Helper.hpp"
#include "Input.hpp"
#include "Sound.hpp"
#include "UIElements.hpp"

namespace
{
    constexpr dtl::array<Sprite, 3> INDICATOR_FRAMES = {{
        {
            .uvX          = 0x4c,
            .uvV          = 0xb7,
            .width        = 16,
            .height       = 12,
            .texture_page = getTPage(0, 1, 320, 128),
            .clut         = getClut(96, 500),
        },
        {
            .uvX          = 0x5c,
            .uvV          = 0xb7,
            .width        = 16,
            .height       = 12,
            .texture_page = getTPage(0, 1, 320, 128),
            .clut         = getClut(96, 500),
        },
        {
            .uvX          = 0x6c,
            .uvV          = 0xb7,
            .width        = 16,
            .height       = 12,
            .texture_page = getTPage(0, 1, 320, 128),
            .clut         = getClut(96, 500),
        },
    }};
} // namespace

Textbox::Textbox(RECT bounds, dtl::runtime_array<AtlasString> text, dtl::optional<RECT> origin, UIBox::Style style)
    : frame(bounds, style, origin)
    , bounds(bounds)
    , origin(origin)
    , text(dtl::move(text))
{
    layoutText();
}

SelectTextbox::SelectTextbox(RECT bounds,
                             dtl::runtime_array<AtlasString> text,
                             dtl::runtime_array<uint32_t> options,
                             dtl::optional<RECT> origin,
                             UIBox::Style style,
                             uint32_t initial)
    : ConfirmTextbox(bounds, dtl::move(text), origin, style)
    , options(dtl::move(options))
    , selected(this->options.size() == 0 ? 0 : dtl::min(initial, this->options.size() - 1))
{
    for (const auto row : this->options)
        cursorWidth = dtl::max(cursorWidth, this->text[row].getWidth() + 2);
}

void Textbox::layoutText()
{
    for (uint32_t i = 0; i < text.size(); ++i)
        text[i].translate(bounds.x + TEXTBOX_INSET_X, bounds.y + TEXTBOX_INSET_Y + i * TEXTBOX_LINE_HEIGHT);
}

void Textbox::replacePage(dtl::runtime_array<AtlasString> page)
{
    text = dtl::move(page);
    layoutText();
}

void Textbox::tick()
{
    frame.tick();
}

bool ConfirmTextbox::tick()
{
    if (isOpened()) ++indicatorCounter;
    Textbox::tick();
    return isOpened() && isKeyDownPolled(InputButtons::BUTTON_CROSS);
}

dtl::optional<uint32_t> SelectTextbox::tick()
{
    Textbox::tick();
    if (!isOpened() || options.size() == 0) return {};
    if (isKeyDownPolled(InputButtons::BUTTON_CROSS)) return selection();
    if (isKeyDownPolled(InputButtons::BUTTON_UP)) {
        selected = selected == 0 ? options.size() - 1 : selected - 1;
        playSound(0, 2);
    }
    else if (isKeyDownPolled(InputButtons::BUTTON_DOWN)) {
        selected = selected + 1 == options.size() ? 0 : selected + 1;
        playSound(0, 2);
    }
    return {};
}

void Textbox::close(dtl::optional<RECT> target)
{
    if (frame.getState() == UIBox::State::CLOSING || isClosed()) return;
    if (target.has_value())
        frame.closeTo(target.value());
    else if (origin.has_value())
        frame.closeTo(origin.value());
    else
        frame.close();
    text = {};
}

bool Textbox::isOpened()
{
    return frame.getState() == UIBox::State::OPENED;
}

bool Textbox::isClosed()
{
    return frame.getState() == UIBox::State::CLOSED;
}

void Textbox::renderText(int32_t depth)
{
    if (!isOpened()) return;
    for (const auto& entry : text)
        entry.render(depth);
}

void Textbox::render(int32_t depth)
{
    renderText(depth);
    frame.render(depth);
}

void ConfirmTextbox::render(int32_t depth)
{
    renderText(depth);
    if (isOpened())
        INDICATOR_FRAMES[indicatorCounter / 4 % 3].render(bounds.x + bounds.width - 23,
                                                          bounds.y + bounds.height - 16,
                                                          depth,
                                                          0x40);
    frame.render(depth);
}

void SelectTextbox::render(int32_t depth)
{
    renderText(depth);
    if (isOpened() && options.size() != 0)
        renderSelectionCursor(bounds.x + TEXTBOX_INSET_X - 1,
                              bounds.y + TEXTBOX_INSET_Y + selection() * TEXTBOX_LINE_HEIGHT - 1,
                              cursorWidth,
                              TEXTBOX_LINE_HEIGHT,
                              depth);
    frame.render(depth);
}
