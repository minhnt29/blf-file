
#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#define STRICT /* WIN32 */
#include <windows.h>
#endif

#include "C:\Users\Administrator\Downloads\Programming\BLF_Logging\Include\binlog.h" /* BL    */

/******************************************************************************
 *
 * Util
 *
 *******************************************************************************/


SYSTEMTIME GetUtcTime() {
  SYSTEMTIME t;
  GetSystemTime(&t);
  return t;
}


/******************************************************************************
 *                                                                             *
 * write BL file                                                               *
 *                                                                             *
 ******************************************************************************/

#define ENV_NAME1 "EnvString"
#define ENV_NAME2 "EnvInt"
#define ENV_DATA1 "01234567"
#define ENV_DATA2 "76543210"

int write_test(const char *pFileName, uint32_t *pWritten) {
  BLHANDLE hFile;
  SYSTEMTIME systemTime;
  int32_t bSuccess;
  uint64_t i;
  VBLAppTrigger appTrigger;
  VBLCANMessage message;

  VBLAppText appText;
  uint64_t time;

  if (NULL == pWritten) {
    return -1;
  }

  *pWritten = 0;

  /* open file */
  hFile = BLCreateFile(pFileName, GENERIC_WRITE);

  if (BLINVALID_HANDLE_VALUE == hFile) {
    return -1;
  }

  /* set applicaton information */
  /* bSuccess = BLSetApplication( hFile, BL_APPID_UNKNOWN, 1, 0, 0); */
  bSuccess = BLSetApplication(hFile, BL_APPID_CANCASEXLLOG, 1, 0, 1);

  systemTime = GetUtcTime();

  bSuccess = bSuccess && BLSetMeasurementStartTime(hFile, &systemTime);

  /* set write options */
  bSuccess = bSuccess && BLSetWriteOptions(hFile, 6, 0);

  if (bSuccess) {
    // initialize all members to zero (including reserved space)
    memset(&appTrigger, 0, sizeof(VBLAppTrigger));
    memset(&message, 0, sizeof(VBLCANMessage));

    /* setup object headers */
    appTrigger.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
    appTrigger.mHeader.mBase.mHeaderSize = sizeof(appTrigger.mHeader);
    appTrigger.mHeader.mBase.mHeaderVersion = 1;
    appTrigger.mHeader.mBase.mObjectSize = sizeof(VBLAppTrigger);
    appTrigger.mHeader.mBase.mObjectType = BL_OBJ_TYPE_APP_TRIGGER;
    appTrigger.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;

    message.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
    message.mHeader.mBase.mHeaderSize = sizeof(message.mHeader);
    message.mHeader.mBase.mHeaderVersion = 1;
    message.mHeader.mBase.mObjectSize = sizeof(VBLCANMessage);
    message.mHeader.mBase.mObjectType = BL_OBJ_TYPE_CAN_MESSAGE;
    message.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;

    appText.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
    appText.mHeader.mBase.mHeaderSize = sizeof(appText.mHeader);
    appText.mHeader.mBase.mHeaderVersion = 1;
    appText.mHeader.mBase.mObjectType = BL_OBJ_TYPE_APP_TEXT;
    appText.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;


    for (i = 0; i < 1000 && bSuccess; ++i) {
      /* increment in milliseconds */
      time = i * 10000000;

      /* setup app trigger object header */
      appTrigger.mHeader.mObjectTimeStamp = time;

      /* write app trigger object */
      bSuccess = BLWriteObject(hFile, &appTrigger.mHeader.mBase);
      *pWritten += bSuccess ? 1 : 0;

      /* setup CAN object header */
      message.mHeader.mObjectTimeStamp = time;

      /* setup CAN message */
      message.mChannel = 1;
      message.mFlags = CAN_MSG_FLAGS(0, 0);
      message.mDLC = 8;
      message.mID = 0x100;
      memcpy(message.mData, (i % 2) ? "01234567" : "76543210", message.mDLC);

      /* write CAN message */
      bSuccess = BLWriteObject(hFile, &message.mHeader.mBase);
      *pWritten += bSuccess ? 1 : 0;

      if (0 == (i % 3) && bSuccess) {
        /* write text */
        if ((i % 100) == 0) {
          char text[128];

          sprintf(text, "%d objects written...", *pWritten);

          appText.mText = text;
          appText.mTextLength = (uint32_t)(strlen(appText.mText));
          appText.mHeader.mBase.mObjectSize =
              sizeof(VBLAppText) + appText.mTextLength;
          appText.mHeader.mObjectTimeStamp = time;

          bSuccess = bSuccess && BLWriteObject(hFile, &appText.mHeader.mBase);
          *pWritten += bSuccess ? 1 : 0;
        }
      }
    }
    appText.mText = "All objects written...";
    appText.mTextLength = (uint32_t)(strlen(appText.mText));
    appText.mHeader.mBase.mObjectSize =
        sizeof(VBLAppText) + appText.mTextLength;

    bSuccess = bSuccess && BLWriteObject(hFile, &appText.mHeader.mBase);
    *pWritten += bSuccess ? 1 : 0;
  }

  /* close file */
  if (!BLCloseHandle(hFile)) {
    return -1;
  }

  return bSuccess ? 0 : -1;
}

/******************************************************************************
 *                                                                             *
 * skip all elements of an BL file                                             *
 *                                                                             *
 ******************************************************************************/
int skip_test(const char *pFileName, uint32_t *pRead) {
  BLHANDLE hFile;
  VBLObjectHeaderBase base;
  VBLFileStatisticsEx statistics = {sizeof(statistics)};
  int32_t bSuccess;

  if (NULL == pRead) {
    return -1;
  }

  *pRead = 0;

  /* open file */
  hFile = BLCreateFile(pFileName, GENERIC_READ);

  if (BLINVALID_HANDLE_VALUE == hFile) {
    return -1;
  }

  BLGetFileStatisticsEx(hFile, &statistics);

  bSuccess = 1;

  /* read base object header from file */
  while (bSuccess && BLPeekObject(hFile, &base)) {
    bSuccess = BLSkipObject(hFile, &base);

    if (bSuccess) {
      *pRead += 1;
    }
  }

  /* close file */
  if (!BLCloseHandle(hFile)) {
    return -1;
  }

  printf("The skip test was: %s\n", bSuccess ? "successful" : "not successful");

  return bSuccess ? 0 : -1;
}

/******************************************************************************
 *                                                                             *
 *                                                                             *
 *                                                                             *
 ******************************************************************************/
int main(int argc, char **argv) {
  const char *pFileName = "test.blf";
  uint32_t dwWritten;
  uint32_t dwRead;
  uint32_t dwSkip;

  int result = 0;

  result = write_test(pFileName, &dwWritten);

  if (0 != result) {
    return result;
  }

  result = skip_test(pFileName, &dwSkip);

  if (0 != result) {
    return result;
  }

  printf("Done");
  return ((dwWritten + 1) == dwRead && dwRead == dwSkip) ? 0 : -1;
}