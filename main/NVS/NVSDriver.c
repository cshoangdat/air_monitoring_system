#include "NVSDriver.h"
#include "nvs.h"
#include "esp_log.h"
#include "string.h"
#define NVS_PART_NAME                   "nvs"

nvs_handle_t NVSHandle;

static const char* TAG = "NVS Driver";

esp_err_t NVSDriverInit(void){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        err = nvs_flash_init();
    }
    return err;
}

esp_err_t NVSDriverOpen(nvs_open_mode_t OpenMode, const char* KeyNameSpace)
{
    return nvs_open_from_partition(NVS_PART_NAME, KeyNameSpace, OpenMode, &NVSHandle);
}

void NVSDriverClose(void)
{
    nvs_close(NVSHandle);
}

esp_err_t NVSDriverCommit(void)
{
    return nvs_commit(NVSHandle);
}

esp_err_t NVSDriverReadString(const char* KeyNameSpace, const char* KeyName, char** out)
{
    size_t required_size;
    esp_err_t err = NVSDriverOpen(NVS_READONLY, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_get_str(NVSHandle, KeyName, NULL, &required_size);
        if(err == ESP_OK){
            char* tmp_buf = malloc(required_size);
            err = nvs_get_str(NVSHandle, KeyName, tmp_buf, &required_size);
            if(err == ESP_OK){
                *out = tmp_buf;
                ESP_LOGI(TAG, "Read: %s/%s success", KeyNameSpace, KeyName);
                NVSDriverClose();
                return ESP_OK;
            }
        }
        *out = NULL;
        ESP_LOGE(TAG, "Read: %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    else
    {
        *out = NULL;
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverWriteString(const char* KeyNameSpace, const char* KeyName, char* in)
{
    esp_err_t err = NVSDriverOpen(NVS_READWRITE, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_set_str(NVSHandle, KeyName, in);
        if(err == ESP_OK)
        {
            ESP_LOGI(TAG, "Save: %s/%s = %s success", KeyNameSpace, KeyName, in);
            NVSDriverCommit();
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Save: %s/%s = %s fail=[%s]", KeyNameSpace, KeyName, in, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverReadU64(const char* KeyNameSpace, const char* KeyName, uint64_t* out)
{
    esp_err_t err = NVSDriverOpen(NVS_READONLY, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_get_u64(NVSHandle, KeyName, out);
        if(err == ESP_OK){
            ESP_LOGI(TAG, "Read: %s/%s success", KeyNameSpace, KeyName);
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Read: %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err; 
}
esp_err_t NVSDriverWriteU64(const char* KeyNameSpace, const char* KeyName, uint64_t in)
{
    esp_err_t err = NVSDriverOpen(NVS_READWRITE, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_set_u64(NVSHandle, KeyName, in);
        if(err == ESP_OK)
        {
            ESP_LOGI(TAG, "Save: %s/%s = %d success", KeyNameSpace, KeyName, in);
            NVSDriverCommit();
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Save: %s/%s = %d fail=[%s]", KeyNameSpace, KeyName, in, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverReadU32(const char* KeyNameSpace, const char* KeyName, uint32_t* out)
{
    esp_err_t err = NVSDriverOpen(NVS_READONLY, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_get_u32(NVSHandle, KeyName, out);
        if(err == ESP_OK){
            ESP_LOGI(TAG, "Read: %s/%s success", KeyNameSpace, KeyName);
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Read: %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err; 
}

esp_err_t NVSDriverWriteU32(const char* KeyNameSpace, const char* KeyName, uint32_t in)
{
    esp_err_t err = NVSDriverOpen(NVS_READWRITE, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_set_u32(NVSHandle, KeyName, in);
        if(err == ESP_OK)
        {
            ESP_LOGI(TAG, "Save: %s/%s = %d success", KeyNameSpace, KeyName, in);
            NVSDriverCommit();
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Save: %s/%s = %d fail=[%s]", KeyNameSpace, KeyName, in, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverReadU16(const char* KeyNameSpace, const char* KeyName, uint16_t* out)
{
    esp_err_t err = NVSDriverOpen(NVS_READONLY, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_get_u16(NVSHandle, KeyName, out);
        if(err == ESP_OK){
            ESP_LOGI(TAG, "Read: %s/%s success", KeyNameSpace, KeyName);
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Read: %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err; 
}

esp_err_t NVSDriverWriteU16(const char* KeyNameSpace, const char* KeyName, uint16_t in)
{
    esp_err_t err = NVSDriverOpen(NVS_READWRITE, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_set_u16(NVSHandle, KeyName, in);
        if(err == ESP_OK)
        {
            ESP_LOGI(TAG, "Save: %s/%s = %d success", KeyNameSpace, KeyName, in);
            NVSDriverCommit();
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Save: %s/%s = %d fail=[%s]", KeyNameSpace, KeyName, in, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverReadU8(const char* KeyNameSpace, const char* KeyName, uint8_t* out)
{
    esp_err_t err = NVSDriverOpen(NVS_READONLY, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_get_u8(NVSHandle, KeyName, out);
        if(err == ESP_OK){
            ESP_LOGI(TAG, "Read: %s/%s success", KeyNameSpace, KeyName);
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Read: %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err; 
}

esp_err_t NVSDriverWriteU8(const char* KeyNameSpace, const char* KeyName, uint8_t in)
{
    esp_err_t err = NVSDriverOpen(NVS_READWRITE, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_set_u8(NVSHandle, KeyName, in);
        if(err == ESP_OK)
        {
            ESP_LOGI(TAG, "Save: %s/%s = %d success", KeyNameSpace, KeyName, in);
            NVSDriverCommit();
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Save: %s/%s = %d fail=[%s]", KeyNameSpace, KeyName, in, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverReadBlob(const char* KeyNameSpace, const char* KeyName, uint8_t** out)
{
    size_t required_size;
    esp_err_t err = NVSDriverOpen(NVS_READONLY, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_get_blob(NVSHandle, KeyName, NULL, &required_size);
        if(err == ESP_OK){
            uint8_t* tmp_buf = malloc(required_size);
            err = nvs_get_blob(NVSHandle, KeyName, tmp_buf, &required_size);
            if(err == ESP_OK){
                *out = tmp_buf;
                ESP_LOGI(TAG, "Read: %s/%s success", KeyNameSpace, KeyName);
                NVSDriverClose();
                return ESP_OK;
            }
        }
        *out = NULL;
        ESP_LOGE(TAG, "Read: %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    else
    {
        *out = NULL;
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverWriteBlob(const char* KeyNameSpace, const char* KeyName, uint8_t* in, size_t in_len)
{
    esp_err_t err = NVSDriverOpen(NVS_READWRITE, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_set_blob(NVSHandle, KeyName, in, in_len);
        if(err == ESP_OK)
        {
            ESP_LOGI(TAG, "Save: %s/%s = (blob length %u) success", KeyNameSpace, KeyName, in_len);
            NVSDriverCommit();
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Save: %s/%s = (blob length %u) fail=[%s]", KeyNameSpace, KeyName, in_len, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverEraseKey(const char* KeyNameSpace, const char* KeyName)
{
    esp_err_t err = NVSDriverOpen(NVS_READWRITE, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_erase_key(NVSHandle, KeyName);
        if(err == ESP_OK)
        {
            ESP_LOGI(TAG, "Erase: %s/%s success", KeyNameSpace, KeyName);
            NVSDriverCommit();
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Erase: %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s/%s fail=[%s]", KeyNameSpace, KeyName, esp_err_to_name(err));
    }
    return err;
}

esp_err_t NVSDriverEraseNameSpace(const char* KeyNameSpace)
{
    esp_err_t err = NVSDriverOpen(NVS_READWRITE, KeyNameSpace);
    if(err == ESP_OK)
    {
        err = nvs_erase_all(NVSHandle);
        if(err == ESP_OK)
        {
            ESP_LOGI(TAG, "Erase: %s success", KeyNameSpace);
            NVSDriverCommit();
            NVSDriverClose();
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Erase: %s fail=[%s]", KeyNameSpace, esp_err_to_name(err));
    } 
    else
    {
        ESP_LOGE(TAG, "Open %s fail=[%s]", KeyNameSpace, esp_err_to_name(err));
    }
    return err;
}