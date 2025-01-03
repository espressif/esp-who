#include "dl_tool_cache.hpp"

namespace dl {
namespace tool {
namespace cache {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
static uint8_t autoload_trigger = 2; // TODO: typedef enum 提高可读性
static uint8_t autoload_linesize = 0;
static uint8_t autoload_enable = 1;
static uint8_t preload_enable = 0;
#pragma GCC diagnostic pop

int8_t preload_init(uint8_t preload)
{
#if CONFIG_IDF_TARGET_ESP32S3
    preload_enable = preload;
    if (autoload_enable && preload_enable) {
        Cache_Disable_DCache_Autoload();
        autoload_enable = 0;
        printf("preload has been turned on, and autoload has been turned off\n");
        return 0;
    }
    return 1;
#endif
    return -1;
}

void preload_func(uint32_t addr, uint32_t size)
{
#if CONFIG_IDF_TARGET_ESP32S3
    if (preload_enable && (!autoload_enable)) {
        uint8_t enable = (addr < SOC_EXTRAM_DATA_HIGH) ? 1 : 0;
        if (enable) {
            while (!Cache_DCache_Preload_Done()) {
                ;
            }
            Cache_Start_DCache_Preload(addr, size, 0);
        }
    }
#endif
}

int8_t autoload_init(uint8_t autoload, uint8_t trigger, uint8_t line_size)
{
#if CONFIG_IDF_TARGET_ESP32S3
#if CONFIG_TIE728_BOOST
    autoload_trigger = trigger;
    autoload_linesize = line_size;
    autoload_enable = autoload;
    struct autoload_config config = {
        autoload_enable,
        CACHE_AUTOLOAD_POSITIVE,
        autoload_trigger,
        autoload_linesize,
    };
    Cache_Config_DCache_Autoload(&config);
    Cache_Enable_DCache_Autoload();
#else
    autoload_trigger = trigger;
    autoload_linesize = line_size;
    autoload_enable = autoload;
#endif
    if (preload_enable && autoload_enable) {
        while (!Cache_DCache_Preload_Done()) {
            ;
        }
        preload_enable = 0;
        printf("autoload has been turned on, and preload has been turned off\n");
        return 0;
    }
    return 1;
#endif
    return -1;
}

void autoload_func(uint32_t addr1, uint32_t size1, uint32_t addr2, uint32_t size2)
{
#if CONFIG_IDF_TARGET_ESP32S3
#if CONFIG_TIE728_BOOST
    if (autoload_enable && (!preload_enable)) {
        uint8_t input1_enable = (addr1 < SOC_EXTRAM_DATA_HIGH) ? 1 : 0;
        uint8_t input2_enable = (addr2 < SOC_EXTRAM_DATA_HIGH) ? 1 : 0;

        // config first region
        struct autoload_region_config region0 = {
            0,             // region
            input1_enable, // ena
            addr1,         // addr
            size1,         // autoload region size 0-0x03FFFFFF
        };
        Cache_Config_DCache_Region_Autoload(&region0);

        // config second region
        struct autoload_region_config region1 = {
            1,             // region
            input2_enable, // ena
            addr2,         // addr
            size2,         // autoload region size 0-0x03FFFFFF
        };
        Cache_Config_DCache_Region_Autoload(&region1);
    }
#else
    if (autoload_enable && (!preload_enable)) {
        Cache_Disable_DCache_Autoload();
        uint8_t input1_enable = (addr1 < SOC_EXTRAM_DATA_HIGH) ? 1 : 0;
        uint8_t input2_enable = (addr2 < SOC_EXTRAM_DATA_HIGH) ? 1 : 0;
        struct autoload_config config = {
            CACHE_AUTOLOAD_POSITIVE,
            autoload_trigger,
            input1_enable,
            input2_enable,
            addr1,
            size1, // autoload max size 0x03FFFFFF
            addr2,
            size2, // autoload max size 0x03FFFFFF
        };
        Cache_Config_DCache_Autoload(&config);
        REG_SET_FIELD(EXTMEM_DCACHE_AUTOLOAD_CTRL_REG, EXTMEM_DCACHE_AUTOLOAD_SIZE, autoload_linesize); // default 0
        Cache_Enable_DCache_Autoload();
        // printf("autoload_start!\n");
    }
#endif
#endif
}

void autoload_func(uint32_t addr1, uint32_t size1)
{
#if CONFIG_IDF_TARGET_ESP32S3
#if CONFIG_TIE728_BOOST
    if (autoload_enable && (!preload_enable)) {
        uint8_t input1_enable = (addr1 < SOC_EXTRAM_DATA_HIGH) ? 1 : 0;

        // config first region
        struct autoload_region_config region0 = {
            0,             // region
            input1_enable, // ena
            addr1,         // addr
            size1,         // autoload region size 0-0x03FFFFFF
        };
        Cache_Config_DCache_Region_Autoload(&region0);
    }
#else
    if (autoload_enable && (!preload_enable)) {
        Cache_Disable_DCache_Autoload();
        uint8_t input1_enable = (addr1 < SOC_EXTRAM_DATA_HIGH) ? 1 : 0;
        struct autoload_config config = {
            CACHE_AUTOLOAD_POSITIVE,
            autoload_trigger,
            input1_enable,
            0,
            addr1,
            size1, // autoload max size 0x03FFFFFF
            addr1,
            size1, // autoload max size 0x03FFFFFF
        };
        Cache_Config_DCache_Autoload(&config);
        REG_SET_FIELD(EXTMEM_DCACHE_AUTOLOAD_CTRL_REG, EXTMEM_DCACHE_AUTOLOAD_SIZE, autoload_linesize); // default 0
        Cache_Enable_DCache_Autoload();
        // printf("autoload_start!\n");
    }
#endif
#endif
}
} // namespace cache
} // namespace tool
} // namespace dl
