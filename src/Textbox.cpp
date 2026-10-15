#include "Textbox.hpp"

#include "Helper.hpp"
#include "Input.hpp"
#include "Sound.hpp"
#include "UIElements.hpp"

namespace
{
    constexpr dtl::array<Sprite, 3> INDICATOR_FRAMES = {{
        {.uvX          = 0x4c,
         .uvV          = 0xb7,
         .width        = 16,
         .height       = 12,
         .texture_page = getTPage(0, 1, 320, 128),
         .clut         = getClut(96, 500)},
        {.uvX          = 0x5c,
         .uvV          = 0xb7,
         .width        = 16,
         .height       = 12,
         .texture_page = getTPage(0, 1, 320, 128),
         .clut         = getClut(96, 500)},
        {.uvX          = 0x6c,
         .uvV          = 0xb7,
         .width        = 16,
         .height       = 12,
         .texture_page = getTPage(0, 1, 320, 128),
         .clut         = getClut(96, 500)},
    }};
} // namespace

Textbox::Textbox(RECT bounds,
                 dtl::runtime_array<AtlasString> text,
                 dtl::optional<RECT> origin,
                 RGB8 color,
                 bool semiTrans)
    : frame(bounds, color, semiTrans, origin)
    , bounds(bounds)
    , origin(origin)
    , text(dtl::move(text))
{
}

ConfirmTextbox::ConfirmTextbox(RECT bounds,
                               dtl::runtime_array<AtlasString> text,
                               dtl::optional<RECT> origin,
                               RGB8 color,
                               bool semiTrans)
    : Textbox(bounds, dtl::move(text), origin, color, semiTrans)
{
}

SelectTextbox::SelectTextbox(RECT bounds,
                             dtl::runtime_array<AtlasString> text,
                             dtl::runtime_array<TextboxOption> options,
                             dtl::optional<RECT> origin,
                             RGB8 color,
                             bool semiTrans,
                             uint32_t initial)
    : ConfirmTextbox(bounds, dtl::move(text), origin, color, semiTrans)
    , options(dtl::move(options))
    , selected(this->options.size() == 0 ? 0 : dtl::min(initial, this->options.size() - 1))
{
}

void ConfirmTextbox::replacePage(dtl::runtime_array<AtlasString> page)
{
    text      = dtl::move(page);
    completed = false;
}

void Textbox::tick()
{
    frame.tick();
}

void ConfirmTextbox::tick()
{
    if (isOpened()) ++indicatorCounter;
    Textbox::tick();
}

void ConfirmTextbox::update(bool closeOnResult)
{
    if (!isOpened() || completed) return;
    if (isKeyDownPolled(InputButtons::BUTTON_CROSS)) {
        completed = true;
        if (closeOnResult) close();
    }
}

void SelectTextbox::update()
{
    if (!isOpened() || completed || options.size() == 0) return;
    if (isKeyDownPolled(InputButtons::BUTTON_CROSS)) {
        completedValue = options[selected].value;
        completed      = true;
        close();
        return;
    }
    if (isKeyDownPolled(InputButtons::BUTTON_UP)) {
        selected = selected == 0 ? options.size() - 1 : selected - 1;
        playSound(0, 2);
    }
    else if (isKeyDownPolled(InputButtons::BUTTON_DOWN)) {
        selected = selected + 1 == options.size() ? 0 : selected + 1;
        playSound(0, 2);
    }
}

void Textbox::close()
{
    if (frame.getState() == UIBox::State::CLOSING || isClosed()) return;
    if (origin.has_value())
        frame.closeTo(origin.value());
    else
        frame.close();
    text = {};
}

void SelectTextbox::close()
{
    Textbox::close();
    options = {};
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
    if (isOpened())
        for (auto i = text.size(); i != 0; --i)
            text[i - 1].render(depth);
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
    if (isOpened() && options.size() != 0) {
        const auto& rect = options[selected].bounds;
        renderSelectionCursor(rect.x, rect.y, rect.width, rect.height, depth);
    }
    frame.render(depth);
}
