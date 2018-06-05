//
// Created by cpasjuste on 05/06/18.
//

#include <cstdio>
#include <string>

#include "tinyxml2.h"
#include "psnes_db.h"

using namespace tinyxml2;

//std::string dbPath = "/home/cpasjuste/dev/multi/psnes/psnes/data/Nintendo - Super Nintendo Entertainment System Parent-Clone (20180601-221617) - dat-o-magic.xml";
std::string dbPath = "/home/cpasjuste/dev/multi/psnes/psnes/data/Super Nintendo Entertainment System - hyperspin.xml";
int game_count = 0;

int main(int argc, char *argv[]) {

    if (argc > 1) {
        dbPath = argv[1];
    }

    XMLDocument doc;
    XMLError e = doc.LoadFile(dbPath.c_str());
    if (e != XML_SUCCESS) {
        printf("error: %s\n", tinyxml2::XMLDocument::ErrorIDToName(e));
        return e;
    }

    // try "http://datomatic.no-intro.org/" format
    XMLNode *pRoot = doc.FirstChildElement("datafile");
    if (!pRoot) {
        // try "http://hyperspin-fe.com/" format
        pRoot = doc.FirstChildElement("menu");
        if (!pRoot) {
            printf("error: incorrect db.xml format\n");
            return -1;
        }
    }

    XMLNode *pGame = pRoot->FirstChildElement("game");
    if (!pGame) {
        printf("error: <game> node not found, incorrect format\n");
        return -1;
    }

    while (pGame) {

        // get name
        //printf("%s\n", pGame->ToElement()->Attribute("name"));

        // get clone
        XMLElement *element = pGame->FirstChildElement("cloneof");
        if (element && element->GetText()) {
            //printf("cloneof: %s\n", element->GetText());
        } else {
            const char *clone = pGame->ToElement()->Attribute("cloneof");
            if(clone) {
                //printf("clone: %s\n", pGame->ToElement()->Attribute("cloneof"));
            }
        }

        // get year
        element = pGame->FirstChildElement("year");
        if (element && element->GetText()) {
            printf("year: %s\n", element->GetText());
        }

        game_count++;
        pGame = pGame->NextSibling();
    }

    printf("games found: %i\n", game_count);
}
