//
// Created by cpasjuste on 29/05/18.
//

#include "c2dui.h"
#include "romlist.h"

#include "snesdb.h"

using namespace c2d;
using namespace c2dui;

struct SNESRom {

    const char name[512];
    const char parent[512];

};

static struct SNESRom SNESDB[SNES_ROMS_COUNT] = {

        {"ROM1", NULL},
        {"ROM2", "ROM1"}
};

PSNESRomList::PSNESRomList(C2DUIGuiMain *ui, const std::string &emuVersion) : C2DUIRomList(ui, emuVersion) {

    printf("PSNESRomList::PSNESRomList()\n");
}

void PSNESRomList::build() {

    printf("PSNESRomList::build()\n");

    char path[MAX_PATH];
    char pathUppercase[MAX_PATH]; // sometimes on FAT32 short files appear as all uppercase

    for (unsigned int i = 0; i < SNES_ROMS_COUNT; i++) {

        auto *rom = new Rom();

        rom->name = (char *) SNESDB[i].name;
        rom->parent = (char *) SNESDB[i].parent;

        /*
        char *zn;
        BurnDrvGetZipName(&zn, 0);
        strncpy(rom->zip, zn, 63);
        rom->drv = i;
        rom->drv_name = BurnDrvGetTextA(DRV_NAME);
        rom->parent = BurnDrvGetTextA(DRV_PARENT);
        rom->name = BurnDrvGetTextA(DRV_FULLNAME);
        rom->year = BurnDrvGetTextA(DRV_DATE);
        rom->manufacturer = BurnDrvGetTextA(DRV_MANUFACTURER);
        rom->system = BurnDrvGetTextA(DRV_SYSTEM);
        rom->genre = BurnDrvGetGenreFlags();
        rom->flags = BurnDrvGetFlags();
        rom->state = RomState::MISSING;
        rom->hardware = BurnDrvGetHardwareCode();
        */

        // add rom to "ALL" game list
        hardwareList->at(0).supported_count++;
        if (rom->parent) {
            hardwareList->at(0).clone_count++;
        }

        // add rom to specific hardware
        Hardware *hardware = getHardware(rom->hardware);
        if (hardware) {
            hardware->supported_count++;
            if (rom->parent) {
                hardware->clone_count++;
            }
        }

        snprintf(path, MAX_PATH, "%s.zip", rom->zip);
        for (int k = 0; k < MAX_PATH; k++) {
            pathUppercase[k] = (char) toupper(path[k]);
        }

        for (unsigned int j = 0; j < C2DUI_ROMS_PATHS_MAX; j++) {

            if (files[j].empty()) {
                continue;
            }

            auto file = std::find(files[j].begin(), files[j].end(), path);
            if (file == files[j].end()) {
                file = std::find(files[j].begin(), files[j].end(), pathUppercase);
            }

            if (file != files[j].end()) {

                //snprintf(rom->zip_path, 256, "%s%s", paths[j].c_str(), file->c_str());
                //rom->state = BurnDrvIsWorking() ? RomState::WORKING : RomState::NOT_WORKING;
                rom->state = RomState::WORKING;
                hardwareList->at(0).available_count++;

                if (rom->parent) {
                    hardwareList->at(0).available_clone_count++;
                }

                if (hardware) {
                    hardware->available_count++;
                    if (rom->parent) {
                        hardware->available_clone_count++;
                    }
                }
                break;
            }
        }

        // set "Io::File"" color for ui
        rom->color = COL_RED;
        if (rom->state == C2DUIRomList::RomState::NOT_WORKING) {
            rom->color = COL_ORANGE;
        } else if (rom->state == C2DUIRomList::RomState::WORKING) {
            rom->color = rom->parent == nullptr ? COL_GREEN : COL_YELLOW;
        }

        if (rom->state == RomState::MISSING) {
            hardwareList->at(0).missing_count++;
            if (hardware) {
                hardware->missing_count++;
            }
            if (rom->parent) {
                hardwareList->at(0).missing_clone_count++;
                if (hardware) {
                    hardware->missing_clone_count++;
                }
            }
        }

        list.push_back(rom);

        // UI
        if (i % 250 == 0) {
            sprintf(text_str, "Scanning... %i%% - ROMS : %i / %i",
                    (i * 100) / SNES_ROMS_COUNT, hardwareList->at(0).supported_count, SNES_ROMS_COUNT);
            text->setString(text_str);
            ui->getRenderer()->flip();
        }
        // UI
    }

    // cleanup
    C2DUIRomList::build();
}
