
#pragma once

#include <stdint.h>
#include <stdio.h>

#define ESP_LOGE( ... )
#define ESP_LOGW( ... )
#define ESP_LOGI( ... )
#define ESP_LOGD( ... )
#define ESP_LOGV( ... )

#define FUNC_LOGE( ... ) ESP_LOGE( __FUNCTION__, __VA_ARGS__ )
#define FUNC_LOGW( ... ) ESP_LOGW( __FUNCTION__, __VA_ARGS__ )
#define FUNC_LOGI( ... ) ESP_LOGI( __FUNCTION__, __VA_ARGS__ )
#define FUNC_LOGD( ... ) ESP_LOGD( __FUNCTION__, __VA_ARGS__ )
#define FUNC_LOGV( ... ) ESP_LOGV( __FUNCTION__, __VA_ARGS__ )
