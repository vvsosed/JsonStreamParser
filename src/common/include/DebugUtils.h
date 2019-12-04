
#pragma once

#include <stdint.h>
#include <stdio.h>

#define ESP_LOGE( ... ) printf(__VA_ARGS__)
#define ESP_LOGW( ... ) printf(__VA_ARGS__)
#define ESP_LOGI( ... ) printf(__VA_ARGS__)
#define ESP_LOGD( ... ) printf(__VA_ARGS__)
#define ESP_LOGV( ... ) printf(__VA_ARGS__)

#define FUNC_LOGE( ... ) printf(__VA_ARGS__)
#define FUNC_LOGW( ... ) printf(__VA_ARGS__)
#define FUNC_LOGI( ... ) printf(__VA_ARGS__)
#define FUNC_LOGD( ... ) printf(__VA_ARGS__)
#define FUNC_LOGV( ... ) printf(__VA_ARGS__)
