//
// Created by Ascarre on 13-06-2022.
//

#ifndef SDKGENERATOR_DUMPER_H
#define SDKGENERATOR_DUMPER_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <ElfFixer/fix.h>

#include "PropertyStructs.h"

void DumpLib(uintptr_t ModuleBase, uintptr_t ModuleEnd) {
    string LibPath = OutPutPathDirectory + Offsets::ProcessName + "/LibTemp.dat";
    ofstream LibDump(LibPath, ofstream::out | ofstream::binary);
    size_t LibSize = (ModuleEnd - ModuleBase);
    if (LibDump.is_open()) {
        char *buffer = new char[1];
        while (LibSize != 0) {
            PVM_Read((void *) (ModuleBase++), buffer, 1);
            LibDump.write(buffer, 1);
            --LibSize;
        }
        LibDump.close();
    }
    sleep(5);
    string LibFixedPath = OutPutPathDirectory + Offsets::ProcessName + "/" + Offsets::ModuleName;
    fix_so(LibPath.c_str(), LibFixedPath.c_str(), ModuleBase);
    remove(LibPath.c_str());//Doesnt work so will think of a workaround for deleteing the temp file later
}

/********** Main Dumping Functions **********/
void DumpBlocks(ofstream &gname, uint32 &count, uintptr_t FNamePool, uint32 blockId, uint32 blockSize) {
    uintptr_t It = Read<uintptr_t>(FNamePool + Offsets::FNamePoolBlocks + (blockId * Offsets::PointerSize));
    uintptr_t End = It + blockSize - Offsets::FNameEntryString;
    uint32 Block = blockId;
    uint16 Offset = 0;
    while (It < End) {
        uintptr_t FNameEntry = It;
        int16 FNameEntryHeader = Read<int16>(FNameEntry);
        int StrLength = FNameEntryHeader >> Offsets::FNameEntryLenBit;
        if (StrLength) {
            bool wide = FNameEntryHeader & 1;
            ///Unicode Dumping Not Supported
            if (StrLength > 0) {
                //String Length Limit
                if (StrLength < 250) {
                    string str;
                    uint32 key = (Block << 16 | Offset);
                    uintptr_t StrPtr = FNameEntry + Offsets::FNameEntryString;

                    if (wide) {
                        str = WideStr::getString(StrPtr, StrLength);
                    } else {
                        str = ReadString2(StrPtr, StrLength);
                    }

                    gname << (wide ? "Wide" : "") << "[" << key << "]: " << str << endl;
                    count++;
                }
            } else {
                StrLength = -StrLength;
            }

            //Next
            Offset += StrLength / Offsets::FNameStride;
            uint16 bytes =
                    Offsets::FNameEntryString + StrLength * (wide ? sizeof(wchar_t) : sizeof(char));
            It += (bytes + Offsets::FNameStride - 1u) & ~(Offsets::FNameStride - 1u);
        } else {// Null-terminator entry found
            break;
        }
    }
}

void DumpStrings(uintptr_t ModuleBase) {
    uint32 count = 0;
    ofstream gname(OutPutPathDirectory + Offsets::ProcessName + "/NamesDump.txt", ofstream::out);
    if (gname.is_open()) {
        gname << "//Dumper Made By Ascarre \n//Join - https://t.me/ascarrehacks" << "\n" << endl;
        gname << "//Libbase was 0x" << hex << ModuleBase << "\n" << endl;
        if (isNew) {
            uintptr_t FNamePool = (ModuleBase + Offsets::GNames) + Offsets::GNamesToFNamePool;

            uint32 BlockSize = Offsets::FNameStride * 65536;
            uint32 CurrentBlock = Read<uint32>(FNamePool + Offsets::FNamePoolCurrentBlock);
            uint32 CurrentByteCursor = Read<uint32>(
                    FNamePool + Offsets::FNamePoolCurrentByteCursor);

            //All Blocks Except Current
            for (uint32 BlockIdx = 0; BlockIdx < CurrentBlock; ++BlockIdx) {
                DumpBlocks(gname, count, FNamePool, BlockIdx, BlockSize);
            }

            //Last Block
            DumpBlocks(gname, count, FNamePool, CurrentBlock, CurrentByteCursor);
        } else {
            for (uint32 i = 0; i < GNameLimit; i++) {
                string s = GetFNameFromID(ModuleBase, i);
                if (!s.empty()) {
                    gname << "[" << i << "]: " << s << endl;
                    count++;
                }
            }
        }
        gname.close();
    }
}

void DumpObjects(uintptr_t ModuleBase) {
    uint32 count = 0;
    ofstream obj(OutPutPathDirectory + Offsets::ProcessName + "/Objects.txt", ofstream::out);
    if (obj.is_open()) {
        obj << "//Dumper Made By Ascarre \n//Join - https://t.me/ascarrehacks" << "\n" << endl;
        obj << "//Libbase was 0x" << hex << ModuleBase << "\n" << endl;

        int32 ocount = GetObjectCount(ModuleBase);
        if (ocount < 10 || ocount > 999999) {
            ocount = 300000;
        }
        for (int32 i = 0; i < ocount; i++) {
            uintptr_t UObjectArray = GetUObjectFromID(ModuleBase, i);
            if (UObject::isValid(UObjectArray)) {
                obj << setbase(16) << "[0x" << i << "]:" << endl;
                obj << "Name: " << UObject::getName(ModuleBase, UObjectArray).c_str() << endl;
                obj << "Class: " << UObject::getClassName(ModuleBase, UObjectArray).c_str() << endl;
                obj << "ObjectPtr: 0x" << setbase(16) << UObjectArray << endl;
                obj << "ClassPtr: 0x" << setbase(16) << UObject::getClass(UObjectArray) << endl;
                obj << endl;
                count++;
            }
        }
        obj.close();
    }
}

void DumpSDK(uintptr_t ModuleBase) {
    ofstream sdk(OutPutPathDirectory + Offsets::ProcessName + "/SDK.txt", ofstream::out);
    if (sdk.is_open()) {
        sdk << "//Dumper Made By Ascarre \n Join - https://t.me/ascarrehacks" << "\n" << endl;
        sdk << "//Libbase was 0x" << hex << ModuleBase << "\n" << endl;
        for (int32 i = 0; i < GetObjectCount(ModuleBase); i++) {
            uintptr_t uobj = GetUObjectFromID(ModuleBase, i);
            if (UObject::isValid(uobj)) {
                writeStruct(sdk, ModuleBase, UObject::getClass(uobj));
            }
        }
        sdk.close();
    }
}

void DumpSDKW(uintptr_t ModuleBase) {
    ofstream sdk(OutPutPathDirectory + Offsets::ProcessName + "/SDKW.txt", ofstream::out);
    if (sdk.is_open()) {
        sdk << "//Dumper Made By Ascarre \n Join - https://t.me/ascarrehacks" << "\n" << endl;
        sdk << "//Libbase was 0x" << hex << ModuleBase << "\n" << endl;

        uintptr_t GWorld;
        if (isPubgM) {
            GWorld = Read<uintptr_t>(Read<uintptr_t>(ModuleBase + Offsets::GWorld) + Offsets::GWorldPointer);
        } else {
            GWorld = Read<uintptr_t>(ModuleBase + Offsets::GWorld);
        }
        if (UObject::isValid(GWorld)) {
            //Iterate World
            writeStruct(sdk, ModuleBase, UObject::getClass(GWorld));
            //Iterate Entities
            uintptr_t level = Read<uintptr_t>(GWorld + Offsets::PersistentLevel);
            uintptr_t actorList = Read<uintptr_t>(level + Offsets::AActors);
            int actorsCount = Read<int>(level + Offsets::AActorsCount);
            for (int i = 0; i < actorsCount; i++) {
                uintptr_t actor = Read<uintptr_t>(actorList + (i * Offsets::PointerSize));
                if (UObject::isValid(actor)) {
                    writeStruct(sdk, ModuleBase, UObject::getClass(actor));
                }
            }
        }
        sdk.close();
    }
}


#endif //SDKGENERATOR_DUMPER_H
