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

    if (isEqual(Offsets::ProcessName, "com.tencent.ig") || isEqual(Offsets::ProcessName, "com.pubg.imobile") /*|| isEqual(Offsets::ProcessName, "other_versions")*/){
        Offsets::PubgMobile();
    } else if (isEqual(Offsets::ProcessName, "com.tencent.iglite")){
        Offsets::PubgLite();
    } else if (isEqual(Offsets::ProcessName, "game_package")){
        isDecrypt = true;//Needs Xor Decryption
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
