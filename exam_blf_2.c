// SPDX-FileCopyrightText: 2013-2021 Tobias Lorenz <tobias.lorenz@gmx.net>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#define _CRT_SECURE_NO_WARNINGS
#include <tchar.h>
#include <stdio.h>
#define STRICT
#include <windows.h>

#include "C:\Users\minhnt29\Downloads\Programming\BLF_Logging\Include\binlog.h"

int main(int argc, char ** argv) {
    /* create file */
    LPCTSTR pFileName = _T("test_CanMessage_2.blf");
    HANDLE hFile = BLCreateFile(pFileName, GENERIC_WRITE);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;

    /* set write options */
    if (!BLSetWriteOptions(hFile, BL_COMPRESSION_NONE, 0))
        return -1;

    /* define object */
    VBLCANMessage2 obj;
    memset(&obj, 0, sizeof(VBLCANMessage2));

    /* VBLCANMessage2 */
    obj.mChannel = 1;
    obj.mFlags = CAN_MSG_FLAGS(0, 0);
    obj.mDLC = 0x08;
    obj.mID = 0x17D;
    obj.mData[0] = 0x55;
    obj.mData[1] = 0x66;
    obj.mData[2] = 0x77;
    obj.mData[3] = 0x88;
    obj.mData[4] = 0x99;
    obj.mData[5] = 0xAA;
    obj.mData[6] = 0xBB;
    obj.mData[7] = 0xCC;
    obj.mFrameLength = 0xDDDD;
    obj.mBitCount = 0xEE;
    obj.mReserved1 = 0xFF;
    obj.mReserved2 = 0x1111;

    /* VBLObjectHeader */
    obj.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_TEN_MICS;
    obj.mHeader.mClientIndex = 0x01;
    obj.mHeader.mObjectVersion = 0;
    obj.mHeader.mObjectTimeStamp = 0;

    /* VBLObjectHeaderBase */
    obj.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
    obj.mHeader.mBase.mHeaderSize = sizeof(obj.mHeader);
    obj.mHeader.mBase.mHeaderVersion = 1;
    obj.mHeader.mBase.mObjectSize = sizeof(VBLCANMessage2);
    obj.mHeader.mBase.mObjectType = BL_OBJ_TYPE_CAN_MESSAGE2;

    /* write object */
    obj.mID = 0x98;
    obj.mData[4] = 0x00;
    obj.mData[5] = 0x00;
    obj.mData[6] = 0x00;
    obj.mData[7] = 0x00;
    obj.mHeader.mClientIndex = 0x02;
    if (!BLWriteObject(hFile, &obj.mHeader.mBase))
    {
        printf("Loi 4");
        return -1;
    }

    obj.mData[4] = 0x00;
    obj.mData[5] = 0x00;
    obj.mData[6] = 0x00;
    obj.mData[7] = 0x00;
    obj.mHeader.mClientIndex = 0x01;
    obj.mHeader.mObjectTimeStamp = 120000;
    if (!BLWriteObject(hFile, &obj.mHeader.mBase))
    {
        printf("Loi 4");
        return -1;
    }

    obj.mData[4] = 0x00;
    obj.mData[5] = 0x00;
    obj.mData[6] = 0x02;
    obj.mData[7] = 0x10;
    obj.mHeader.mClientIndex = 0x02;
    obj.mHeader.mObjectTimeStamp = 130000;
    if (!BLWriteObject(hFile, &obj.mHeader.mBase))
    {
        printf("Loi 4");
        return -1;
    }
    /* close handle */
    if (!BLCloseHandle(hFile))
    {
        printf("Loi 5");
        return -1;
    }
    printf("Done\n");
    
    return 0;
}