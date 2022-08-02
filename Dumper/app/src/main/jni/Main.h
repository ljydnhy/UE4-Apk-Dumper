//
// Created by Ascarre on 03-08-2022.
//

#ifndef PRIVATE_DUMPER_MAIN_H
#define PRIVATE_DUMPER_MAIN_H

#include "Canvas.h"
#include "Dumper.h"

void RenderHacks(Canvas Draw, int SWidth, int SHeight) {
    Draw.DrawText(Color(255, 40, 40), "Made By Ascarre", Vector2(SWidth / 8, SHeight / 12), 25);

    ProcessId = FindProcessId(Offsets::ProcessName);
    ModuleBase = GetModuleBase(ProcessId, Offsets::ModuleName);
    ModuleEnd = GetModuleEnd(ProcessId, Offsets::ModuleName);

    string OutputDirectory = "/sdcard/DumpedGames/";
    OutputDirectory += Offsets::ProcessName;

    if (isUE423) {
        if (isEqual(Offsets::ProcessName, "Package Name of the game")){
            Offsets::NewGame();
            //isDecrypt = true;//If needs Xor Decryption then uncomment this line
        }
    } else {
        if (isPubg){//If Gnames is Fuked UP
            Offsets::PubgMobile();
            if (isEqual(Offsets::ProcessName, "com.tencent.iglite")){
                //Offsets::PubgLite();
            }
        }
    }

    /******************** Dumping Functions ********************/
    //LibDump
    if (isLibDump && !LibDumped) {
        LibDumped = true;
        LibDump(OutputDirectory);
    }

    //Strings Dump
    if (isStringDump && !StringDumped) {
        StringDumped = true;
        DumpStrings(OutputDirectory);
    }

    //Objects Dump
    if (isObjectsDump && !isObjectsDumped){
        isObjectsDumped = true;
        DumpObjects(OutputDirectory);
    }

    //SDKW Dump using GWorld
    if (isSDKWDump && !isSDKWDumped) {
        isSDKWDumped = true;
        DumpSDKW(OutputDirectory);
    }

    //SDK Dump using GUobject
    if (isSDKDump && !isSDKDumped) {
        isSDKDumped = true;
        DumpSDK(OutputDirectory);
    }
}

#endif //PRIVATE_DUMPER_MAIN_H
