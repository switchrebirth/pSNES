//
// Created by cpasjuste on 29/05/18.
//

#include "c2dui.h"
#include "romlist.h"
#include "tinyxml2.h"

using namespace c2d;
using namespace c2dui;
using namespace tinyxml2;

static XMLDocument doc;

PSNESRomList::PSNESRomList(C2DUIGuiMain *ui, const std::string &emuVersion) : C2DUIRomList(ui, emuVersion) {

    printf("PSNESRomList::PSNESRomList()\n");
}

void PSNESRomList::buildNoDb() {

    printf("PSNESRomList::buildNoDb()\n");

    for (unsigned int i = 0; i < C2DUI_ROMS_PATHS_MAX; i++) {

        if (files[i].empty()) {
            continue;
        }

        for (unsigned int j = 0; j < files[i].size(); j++) {
            auto *rom = new Rom();
            rom->name = rom->drv_name = files[i][j].c_str();
            rom->path = files[i][j].c_str();
            rom->state = RomState::WORKING;
            hardwareList->at(0).supported_count++;
            hardwareList->at(0).available_count++;
            rom->color = COL_GREEN;
            list.push_back(rom);
        }
    }

    C2DUIRomList::build();
}

void PSNESRomList::build() {

    printf("PSNESRomList::build()\n");

    char path[MAX_PATH];
    char pathUppercase[MAX_PATH]; // sometimes on FAT32 short files appear as all uppercase

    char xmlPath[512];
    snprintf(xmlPath, 511, "%sdb.xml", ui->getConfig()->getHomePath()->c_str());
    XMLError e = doc.LoadFile(xmlPath);
    if (e != XML_SUCCESS) {
        printf("error: %s\n", tinyxml2::XMLDocument::ErrorIDToName(e));
        ui->getUiMessageBox()->show("ERROR", "Could not load db.xml\n\nWill just add any found files...");
        buildNoDb();
        return;
    }

    // try "http://datomatic.no-intro.org/" format
    XMLNode *pRoot = doc.FirstChildElement("datafile");
    if (!pRoot) {
        // try "http://hyperspin-fe.com/" format
        pRoot = doc.FirstChildElement("menu");
        if (!pRoot) {
            printf("error: incorrect db.xml format\n\nWill just add any found files...");
            ui->getUiMessageBox()->show("ERROR", "incorrect db.xml format");
            buildNoDb();
            return;
        }
    }

    XMLNode *pGame = pRoot->FirstChildElement("game");
    if (!pGame) {
        printf("error: <game> node not found, incorrect format\n\nWill just add any found files...");
        ui->getUiMessageBox()->show("ERROR", "incorrect db.xml format");
        buildNoDb();
        return;
    }

    while (pGame != nullptr) {

        auto *rom = new Rom();

        // get "name"
        rom->name = rom->drv_name = rom->path = pGame->ToElement()->Attribute("name");
        // get "cloneof"
        XMLElement *element = pGame->FirstChildElement("cloneof");
        if (element != nullptr && element->GetText()) {
            rom->parent = element->GetText();
        } else {
            rom->parent = pGame->ToElement()->Attribute("cloneof");
        }
        // get "year"
        element = pGame->FirstChildElement("year");
        if (element != nullptr && element->GetText()) {
            rom->year = element->GetText();
        }
        // get "manufacturer"
        element = pGame->FirstChildElement("manufacturer");
        if (element && element->GetText()) {
            rom->manufacturer = element->GetText();
        }

        // add rom to "ALL" game list
        hardwareList->at(0).supported_count++;
        if (rom->parent) {
            hardwareList->at(0).clone_count++;
        }

        snprintf(path, 511, "%s.zip", rom->name);
        for (int k = 0; k < (int) strlen(path); k++) {
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
                rom->path = file->c_str();
                rom->state = RomState::WORKING;
                hardwareList->at(0).available_count++;
                if (rom->parent) {
                    hardwareList->at(0).available_clone_count++;
                }
                break;
            }
        }

        // set "Io::File"" color for ui
        rom->color = COL_RED;
        if (rom->state == C2DUIRomList::RomState::NOT_WORKING) {
            rom->color = COL_ORANGE;
        } else if (rom->state == C2DUIRomList::RomState::WORKING) {
            rom->color = rom->parent ? COL_YELLOW : COL_GREEN;
        }

        if (rom->state == RomState::MISSING) {
            hardwareList->at(0).missing_count++;
            if (rom->parent) {
                hardwareList->at(0).missing_clone_count++;
            }
        }

        //printf("add rom: %s\n", rom->name);
        list.push_back(rom);
        pGame = pGame->NextSibling();

        // UI
        if (list.size() % 250 == 0) {
            snprintf(text_str, 511, "Scanning... %i / %i",
                     hardwareList->at(0).available_count, (int) list.size());
            text->setString(text_str);
            ui->getRenderer()->flip();
        }
        // UI
    }

    // cleanup
    C2DUIRomList::build();
}

PSNESRomList::~PSNESRomList() {

    doc.Clear();
}
