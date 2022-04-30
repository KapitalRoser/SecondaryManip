#include<iostream>
#include <math.h>
#include "../../processCore.h"

uint32_t XDRNG(uint32_t targetSeed) {
    return 0x000343FD * targetSeed + 0x00269EC3;
}

uint32_t XDRNGR(uint32_t targetSeed) {
    return 0xB9B33155 * targetSeed + 0xA170F641;
}

uint16_t getHighSeed(uint32_t targetSeed) {
    return targetSeed >> 16;
}

int main() {
    uint32_t startingSeed, finalStartingSeed;
    unsigned long startingFrame = 0, frame = 0, hittedFrame = 0, testedFrame = 0;
    int counter, catchRate = 0;
    bool found = false;



    const int pokeBall = 47661;
    const int greatBall = 52428;
    const int QUIL_CATCH_ODDS = greatBall;
    catchRate = QUIL_CATCH_ODDS;

    std::cout << "Insert the Starting Seed: ";
    scanf("%X", &startingSeed); //kinda ok honestly, helps save some of the str -> hex headache. what happens when user enters not a hex number?

    std::cout << "Insert the Starting Frame: ";
    std::cin >> startingFrame;


    startingFrame += 150; //frames as in calls to roll.

    LCGn(startingSeed,startingFrame);

    frame = startingFrame;

    for (unsigned long int j = 0; j < 4294967295 && found == false; j++) {
        counter = 0;

        while (getHighSeed(startingSeed) < catchRate && counter < 10) {
            counter++;
            startingSeed = XDRNG(startingSeed);
            frame++;
        }

        if (counter == 10 && frame - 178 > startingFrame) {
            finalStartingSeed = startingSeed;
            for (int i = 0; i < 6; i++) {
                startingSeed = XDRNGR(startingSeed);
            }
            printf("Seed: %08X - %08X | Frame: %lu - %lu\nTry Frame: %lu\n\n", startingSeed, finalStartingSeed, frame - 6, frame - 1, frame - 178);
            found = true;
        }
        else {
            startingSeed = XDRNG(startingSeed);
            frame++;
        }
    }

    while (true) {
        std::cout << "Tested frame: ";
        std::cin >> testedFrame;
        std::cout << "Hitted frame: ";
        std::cin >> hittedFrame;

        std::cout << "Try Frame: ";

        if (hittedFrame > frame) {
            std::cout << int(testedFrame - ((hittedFrame - frame) * 0.875)) <<"\n\n";
        }
        else {
            std::cout << int(testedFrame + ((frame - hittedFrame) * 0.875)) <<"\n\n";
        }
    }

    return 0;
}