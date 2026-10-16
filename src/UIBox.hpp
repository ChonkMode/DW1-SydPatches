#pragma once
#include "extern/dw1.hpp"
#include "extern/libgpu.hpp"

inline constexpr RGB8 UIBOX_DEFAULT_COLOR = {0x2D, 0x38, 0x40};

// Frame contract adapted from Aiko's PR #21, revision 583aac3c.
enum class UIBoxFill
{
    NONE,
    OPAQUE,
    SEMI,
    DOUBLE,
};
struct UIBoxStyle
{
    RGB8 color{UIBOX_DEFAULT_COLOR};
    UIBoxFill fill{UIBoxFill::SEMI};
    bool drawBorder{true};
    bool playOpenSound{true};
    bool playCloseSound{true};
};

struct UIBox
{
    static inline constexpr RGB8 DEFAULT_COLOR = UIBOX_DEFAULT_COLOR;
    using Fill                                 = UIBoxFill;
    using Style                                = UIBoxStyle;
    enum class State
    {
        CLOSED,
        OPENING,
        OPENED,
        CLOSING
    };

    UIBox() = default;
    UIBox(RECT final, Style style = {}, dtl::optional<RECT> start = {});
    void close();
    void closeTo(RECT target);
    void open();
    void tick();
    void render(int32_t depth);
    State getState() const { return state; }
    bool isOpen() const { return state == State::OPENED; }
    bool isClosed() const { return state == State::CLOSED; }

private:
    void renderOpeningClosing(int32_t depth);
    void renderOpened(int32_t depth);
    dtl::optional<RECT> start{};
    RECT final{};
    State state{State::CLOSED};
    Style style{};
    int16_t frame{};
    bool closingSoundPending{};
};
