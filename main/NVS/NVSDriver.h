#ifndef __NVSDriver__
#define __NVSDriver__

#include "nvs_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t NVSDriverInit(void);
esp_err_t NVSDriverOpen(nvs_open_mode_t OpenMode, const char* KeyNameSpace);
void NVSDriverClose(void);
esp_err_t NVSDriverCommit(void);
esp_err_t NVSDriverReadString(const char* KeyNameSpace, const char* KeyName, char** out);
esp_err_t NVSDriverWriteString(const char* KeyNameSpace, const char* KeyName, char* in);
esp_err_t NVSDriverReadBlob(const char* KeyNameSpace, const char* KeyName, uint8_t** out);
esp_err_t NVSDriverWriteBlob(const char* KeyNameSpace, const char* KeyName, uint8_t* in, size_t in_len);
esp_err_t NVSDriverReadU64(const char* KeyNameSpace, const char* KeyName, uint64_t* out);
esp_err_t NVSDriverWriteU64(const char* KeyNameSpace, const char* KeyName, uint64_t in);
esp_err_t NVSDriverReadU32(const char* KeyNameSpace, const char* KeyName, uint32_t* out);
esp_err_t NVSDriverWriteU32(const char* KeyNameSpace, const char* KeyName, uint32_t in);
esp_err_t NVSDriverReadU16(const char* KeyNameSpace, const char* KeyName, uint16_t* out);
esp_err_t NVSDriverWriteU16(const char* KeyNameSpace, const char* KeyName, uint16_t in);
esp_err_t NVSDriverReadU8(const char* KeyNameSpace, const char* KeyName, uint8_t* out);
esp_err_t NVSDriverWriteU8(const char* KeyNameSpace, const char* KeyName, uint8_t in);
esp_err_t NVSDriverEraseKey(const char* KeyNameSpace, const char* KeyName);
esp_err_t NVSDriverEraseNameSpace(const char* KeyNameSpace);

#ifdef __cplusplus
}
#endif

#endif //__NVSDriver__