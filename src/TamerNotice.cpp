#include "TamerNotice.hpp"

#include "GameObjects.hpp"
#include "Sound.hpp"

namespace
{
    constinit dtl::unique_ptr<ConfirmTextbox> notice;
    bool registered{};

    void renderNotice(int32_t)
    {
        if (GAME_STATE != 0) {
            cancelTamerNotice();
            return;
        }
        notice->render(5);
        notice->tick();
        if (notice->isClosed()) cancelTamerNotice();
    }
} // namespace

void prepareTamerNotice(dtl::runtime_array<AtlasString> text)
{
    if (notice && notice->isClosing()) cancelTamerNotice();
    if (notice)
        notice->replacePage(dtl::move(text));
    else {
        // Preparation owns the glyphs but has no visible frame, callback or sound.
        // Tamer drives completion; the pure confirmation result is deliberately ignored.
        notice.reset(new ConfirmTextbox(TAMER_NOTICE_BOUNDS,
                                        dtl::move(text),
                                        {},
                                        {.color = {}, .playOpenSound = false, .playCloseSound = false}));
    }
}

bool openTamerNotice(RECT origin)
{
    if (!notice) return false;
    if (registered) return true;
    if (!addObject(ObjectID::TAMER_NOTICE, 0, nullptr, renderNotice)) return false;
    registered = true;
    notice->openFrom(origin);
    playSound(0, 0);
    return true;
}

void closeTamerNotice(RECT target)
{
    if (!notice || notice->isClosing()) return;
    if (!registered) {
        cancelTamerNotice();
        return;
    }
    const bool wasOpened = notice->isOpened();
    notice->close(target);
    if (wasOpened) playSound(0, 1);
}

void cancelTamerNotice()
{
    if (registered) removeObject(ObjectID::TAMER_NOTICE, 0);
    registered = false;
    notice.reset();
}

bool isTamerNoticeClosing()
{
    return notice && notice->isClosing();
}
