#include "Font.hpp"

#include "VanillaText.hpp"
#include "extern/libgpu.hpp"

// Naming requests vanilla twelve-pixel cells.
extern "C" int8_t SCRIPT_TEXT_FIXED_CELLS;

namespace
{
    void clearPixels(uint8_t* data, int32_t width, int32_t start, int32_t count)
    {
        const auto end = start + count < width ? start + count : width;
        if (start < 0) start = 0;
        for (int32_t row = 0; row < 12; ++row)
            for (int32_t x = start; x < end; ++x)
                data[row * width / 2 + x / 2] &= ~(15 << ((x & 1) * 4));
    }

    void drawGlyphRow(uint8_t* data, int32_t width, int32_t position, int32_t glyph)
    {
        const auto glyphWidth = vanillaFont.getGlyphWidth(glyph);
        for (int32_t y = 0; y < 12; ++y) {
            const auto pixels = vanillaFont.getGlyphRow(glyph, y);
            for (int32_t x = 0; x < glyphWidth; ++x) {
                const auto target = position + x;
                if (target < 0 || target >= width) continue;
                auto& byte       = data[y * width / 2 + target / 2];
                const auto shift = (target & 1) * 4;
                const auto color = (pixels & (0x8000 >> x)) ? 0 : COLORCODE_LOWBITS;
                byte             = (byte & ~(15 << shift)) | (color << shift);
            }
        }
    }
} // namespace

extern "C" int32_t drawScriptTextLine(const uint8_t* text, int16_t x, int16_t y, int32_t width)
{
    // Callers supply the owned full-row or half-row extent (255 or 127 pixels).
    alignas(4) dtl::array<uint8_t, 256 * 12 / 2> row{};
    width = (width + 3) & ~3;
    if (width > 256) width = 256;
    int32_t position = 0;
    int32_t result   = 1;
    while (true) {
        auto code = *text++;
        if (code == 0 || code == 0x0d) {
            result = code == 0;
            break;
        }
        if (code == 1) {
            setTextColor(*text++);
            continue;
        }
        if (code == 2 || code == 3) {
            text += code == 2 ? 1 : 3;
            continue;
        }
        if (code == 0x0c || code == 0x0e) {
            ++text;
            const auto cells     = code == 0x0c ? 8 : 11;
            const auto remainder = position / 12 % cells;
            if (remainder) {
                const auto count = (cells - remainder) * 12;
                clearPixels(row.data(), width, position, count);
                position += count;
            }
            continue;
        }
        if (code == 0x0f) {
            ++text;
            clearPixels(row.data(), width, position, 6);
            position += 6;
            continue;
        }
        if (code >= 0x16 && code <= 0x1c) {
            ++text;
            static constexpr dtl::array<int16_t, 7> positions{{105, 105, 96, 156, 180, 160, 100}};
            const auto target = positions[code - 0x16];
            clearPixels(row.data(), width, position - (code == 0x16 ? 6 : 0), target);
            position = target;
            continue;
        }
        uint16_t point = code;
        if (code >= 0x80) point = (code << 8) | *text++;
        const auto glyph   = vanillaFont.getGlyphIndex(point);
        const auto advance = SCRIPT_TEXT_FIXED_CELLS ? 12 : vanillaFont.getGlyphWidth(glyph);
        if (point == 0x20 || point == 0x8140)
            clearPixels(row.data(), width, position, advance);
        else
            drawGlyphRow(row.data(), width, position, glyph);
        position += advance;
    }
    RECT rect{.x      = static_cast<int16_t>(704 + x / 4),
              .y      = static_cast<int16_t>(256 + y),
              .width  = static_cast<int16_t>(width / 4),
              .height = 12};
    libgpu_LoadImage(&rect, reinterpret_cast<uint32_t*>(row.data()));
    // The queued upload must complete before this stack row goes out of scope.
    libgpu_DrawSync(0);
    return result;
}
