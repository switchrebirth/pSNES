//
// Created by cpasjuste on 01/06/18.
//

#include "c2dui.h"
#include "uiEmu.h"

using namespace c2d;
using namespace c2dui;

PSNESGuiEmu::PSNESGuiEmu(C2DUIGuiMain *ui) : C2DUIGuiEmu(ui) {

    printf("PSNESGuiEmu()\n");
}

int PSNESGuiEmu::run(C2DUIRomList::Rom *rom) {

    // TODO

    return C2DUIGuiEmu::run(rom);
}

void PSNESGuiEmu::stop() {

    C2DUIGuiEmu::stop();
}

void PSNESGuiEmu::updateFb() {

    // TODO
}

void PSNESGuiEmu::renderFrame(bool draw, int drawFps, float fps) {

    getFpsText()->setVisibility(
            drawFps ? Visible : Hidden);

    if (!isPaused()) {

        // TODO
    }
}

void PSNESGuiEmu::updateFrame() {

    int showFps = getUi()->getConfig()->getValue(C2DUIOption::Index::ROM_SHOW_FPS, true);
    int frameSkip = getUi()->getConfig()->getValue(C2DUIOption::Index::ROM_FRAMESKIP, true);

    // TODO
}

int PSNESGuiEmu::update() {

    // TODO
    Input::Player *players = getUi()->getInput()->update();

    // process menu
    if ((players[0].state & Input::Key::KEY_START)
        && (players[0].state & Input::Key::KEY_COIN)) {
        pause();
        return UI_KEY_SHOW_MEMU_ROM;
    } else if ((players[0].state & Input::Key::KEY_START)
               && (players[0].state & Input::Key::KEY_FIRE5)) {
        pause();
        return UI_KEY_SHOW_MEMU_STATE;
    } else if (players[0].state & EV_RESIZE) {
        // useful for sdl resize event for example
        getVideo()->updateScaling();
    }

    // TODO
    //updateFrame();

    return 0;
}
