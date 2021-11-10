#include "esp_log.h"
#include "linenoise/linenoise.h"
#include "esp_vfs_dev.h"
#include "esp_console.h"
#include "esp_vfs_cdcacm.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "driver/uart.h"
#include "driver/usb_serial_jtag.h"
#include <fcntl.h>
#include "esp_vfs_fat.h"
#include "test_logic.h"
#include "who_button.h"

#define CONSOLE_PROMPT_MAX_LEN (32)
#define CONSOLE_PATH_MAX_LEN (ESP_VFS_PATH_MAX)
#define CONFIG_EXAMPLE_STORE_HISTORY 0

static const char *TAG = "ESP32-S3-EYE";
static const char *testmode = "testmode";
static char *console_prompt = NULL;
static QueueHandle_t *queues_tests = NULL;
static QueueHandle_t queue_test_flag = NULL;
static QueueHandle_t queue_test_result = NULL;

typedef struct
{
    esp_console_repl_t repl_core;        // base class
    char prompt[CONSOLE_PROMPT_MAX_LEN]; // Prompt to be printed before each line
    const char *history_save_path;
    TaskHandle_t task_hdl;     // REPL task handle
    size_t max_cmdline_length; // Maximum length of a command line. If 0, default value will be used.
} esp_console_repl_com_t;

typedef struct
{
    esp_console_repl_com_t repl_com; // base class
    int uart_channel;                // uart channel number
} esp_console_repl_universal_t;

#if CONFIG_EXAMPLE_STORE_HISTORY
#define MOUNT_PATH "/data"
#define HISTORY_PATH MOUNT_PATH "/history.txt"

static void initialize_filesystem(void)
{
    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true};
    esp_err_t err = esp_vfs_fat_spiflash_mount(MOUNT_PATH, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
}
#endif

static esp_err_t esp_console_setup_prompt(const char *prompt, esp_console_repl_com_t *repl_com)
{
    /* set command line prompt */
    const char *prompt_temp = "esp>";
    if (prompt)
    {
        prompt_temp = prompt;
    }
    snprintf(repl_com->prompt, CONSOLE_PROMPT_MAX_LEN - 1, LOG_COLOR_I "%s " LOG_RESET_COLOR, prompt_temp);

    /* Figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();
    if (probe_status)
    {
        /* zero indicates success */
        linenoiseSetDumbMode(1);
#if CONFIG_LOG_COLORS
        /* Since the terminal doesn't support escape sequences,
         * don't use color codes in the s_prompt.
         */
        snprintf(repl_com->prompt, CONSOLE_PROMPT_MAX_LEN - 1, "%s ", prompt_temp);
#endif //CONFIG_LOG_COLORS
    }

    return ESP_OK;
}

static esp_err_t esp_console_setup_history(const char *history_path, uint32_t max_history_len, esp_console_repl_com_t *repl_com)
{
    esp_err_t ret = ESP_OK;

    repl_com->history_save_path = history_path;
    if (history_path)
    {
        /* Load command history from filesystem */
        linenoiseHistoryLoad(history_path);
    }

    /* Set command history size */
    if (linenoiseHistorySetMaxLen(max_history_len) != 1)
    {
        ESP_LOGE(TAG, "set max history length to %d failed", max_history_len);
        ret = ESP_FAIL;
        goto _exit;
    }
    return ESP_OK;
_exit:
    return ret;
}

static esp_err_t esp_console_common_init(size_t max_cmdline_length, esp_console_repl_com_t *repl_com)
{
    esp_err_t ret = ESP_OK;
    /* Initialize the console */
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    repl_com->max_cmdline_length = console_config.max_cmdline_length;
    /* Replace the default command line length if passed as a parameter */
    if (max_cmdline_length != 0)
    {
        console_config.max_cmdline_length = max_cmdline_length;
        repl_com->max_cmdline_length = max_cmdline_length;
    }

#if CONFIG_LOG_COLORS
    console_config.hint_color = atoi(LOG_COLOR_CYAN);
#endif
    ret = esp_console_init(&console_config);
    if (ret != ESP_OK)
    {
        goto _exit;
    }

    ret = esp_console_register_help_command();
    if (ret != ESP_OK)
    {
        goto _exit;
    }

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within single line */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback *)&esp_console_get_hint);

    return ESP_OK;
_exit:
    return ret;
}

static esp_err_t initialize_console(const esp_console_repl_config_t *repl_config)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_universal_t *usb_serial_jtag_repl = NULL;

    usb_serial_jtag_repl = (esp_console_repl_universal_t *)calloc(1, sizeof(esp_console_repl_universal_t));
    if (!usb_serial_jtag_repl)
    {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_usb_serial_jtag_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_usb_serial_jtag_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

    /* Enable non-blocking mode on stdin and stdout */
    fcntl(fileno(stdout), F_SETFL, 0);
    fcntl(fileno(stdin), F_SETFL, 0);

    usb_serial_jtag_driver_config_t usb_serial_jtag_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();

    /* Install USB-SERIAL-JTAG driver for interrupt-driven reads and writes */
    ret = usb_serial_jtag_driver_install(&usb_serial_jtag_config);
    if (ret != ESP_OK)
    {
        goto _exit;
    }

    // initialize console, common part
    ret = esp_console_common_init(repl_config->max_cmdline_length, &usb_serial_jtag_repl->repl_com);
    if (ret != ESP_OK)
    {
        goto _exit;
    }

    /* Tell vfs to use usb-serial-jtag driver */
    esp_vfs_usb_serial_jtag_use_driver();

    // setup history
    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len, &usb_serial_jtag_repl->repl_com);
    if (ret != ESP_OK)
    {
        goto _exit;
    }

    // setup prompt
    esp_console_setup_prompt(repl_config->prompt, &usb_serial_jtag_repl->repl_com);
    console_prompt = usb_serial_jtag_repl->repl_com.prompt;
    return ret;

_exit:
    if (usb_serial_jtag_repl)
    {
        esp_console_deinit();
        free(usb_serial_jtag_repl);
    }
    return ret;
}

static void task_console_handler(void *arg)
{
    bool testmode_flag = false;
    en_test_state g_state_test = TEST_IDLE;
    int test_times[TEST_NUM] = {0};
    bool test_reuslts[TEST_NUM] = {false};
    while (true)
    {
        g_state_test = TEST_IDLE;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        char *line = linenoise(console_prompt);
        if (line == NULL)
        {
            printf("empty line\n");
            /* Ignore empty lines */
            continue;
        }
        else
        {
            if (!testmode_flag)
            {
                if (strcmp(line, testmode) == 0)
                {
                    testmode_flag = true;
                    xQueueSend(queue_test_flag, &testmode_flag, portMAX_DELAY);
                    vTaskDelay(1600 / portTICK_PERIOD_MS);

                    ESP_LOGI(TAG, "--------------- Enter Test Mode ---------------\n");
                    ets_printf("ESP32-S3-EYE V2.1 Firmware V0.1.0\nFACTORY Test Mode\n\n");
                    ets_printf("auto: \tauto test\n");
                    ets_printf("all: \ttest all\n");
                    ets_printf("id0: \tSD Card test\n");
                    ets_printf("id1: \tButton test\n");
                    ets_printf("id2: \tLED test\n");
                    ets_printf("id3: \tLCD test\n");
                    ets_printf("id4: \tCAMERA test\n");
                    ets_printf("id5: \tMIC test\n\n");
                }
            }
            else
            {
                if (strcmp(line, "help") == 0)
                {
                    ets_printf("ESP32-S3-EYE V2.0 Firmware V0.1.0\nFACTORY Test Mode\n\n");
                    ets_printf("auto: \tauto test\n");
                    ets_printf("all: \ttest all\n");
                    ets_printf("id0: \tSD Card test\n");
                    ets_printf("id1: \tButton test\n");
                    ets_printf("id2: \tLED test\n");
                    ets_printf("id3: \tLCD test\n");
                    ets_printf("id4: \tCAMERA test\n");
                    ets_printf("id5: \tMIC test\n\n");

                    continue;
                }
                else if (strcmp(line, "all") == 0)
                {
                    for (int i = 0; i < TEST_NUM; ++i)
                    {
                        g_state_test = i;
                        xQueueSend(queues_tests[i], &g_state_test, portMAX_DELAY);
                        xQueueReceive(queue_test_result, &test_reuslts[i], portMAX_DELAY);
                        test_times[i] += 1;
                    }
                }
                else if (strcmp(line, "auto") == 0)
                {
                    for (int i = 0; i < TEST_NUM; ++i)
                    {
                        if (test_reuslts[i])
                            continue;

                        g_state_test = i;
                        xQueueSend(queues_tests[i], &g_state_test, portMAX_DELAY);
                        xQueueReceive(queue_test_result, &test_reuslts[i], portMAX_DELAY);
                        test_times[i] += 1;
                    }
                }
                else if (strcmp(line, "id0") == 0)
                {
                    g_state_test = 0;
                    xQueueSend(queues_tests[0], &g_state_test, portMAX_DELAY);
                    xQueueReceive(queue_test_result, &test_reuslts[0], portMAX_DELAY);
                    test_times[0] += 1;
                }
                else if (strcmp(line, "id1") == 0)
                {
                    g_state_test = 1;
                    xQueueSend(queues_tests[1], &g_state_test, portMAX_DELAY);
                    xQueueReceive(queue_test_result, &test_reuslts[1], portMAX_DELAY);
                    test_times[1] += 1;
                }
                else if (strcmp(line, "id2") == 0)
                {
                    g_state_test = 2;
                    xQueueSend(queues_tests[2], &g_state_test, portMAX_DELAY);
                    xQueueReceive(queue_test_result, &test_reuslts[2], portMAX_DELAY);
                    test_times[2] += 1;
                }
                else if (strcmp(line, "id3") == 0)
                {
                    g_state_test = 3;
                    xQueueSend(queues_tests[3], &g_state_test, portMAX_DELAY);
                    xQueueReceive(queue_test_result, &test_reuslts[3], portMAX_DELAY);
                    test_times[3] += 1;
                }
                else if (strcmp(line, "id4") == 0)
                {
                    g_state_test = 4;
                    xQueueSend(queues_tests[4], &g_state_test, portMAX_DELAY);
                    xQueueReceive(queue_test_result, &test_reuslts[4], portMAX_DELAY);
                    test_times[4] += 1;
                }
                else if (strcmp(line, "id5") == 0)
                {
                    g_state_test = 5;
                    xQueueSend(queues_tests[5], &g_state_test, portMAX_DELAY);
                    xQueueReceive(queue_test_result, &test_reuslts[5], portMAX_DELAY);
                    test_times[5] += 1;
                }
                // else
                // {
                //     xSemaphoreGive(semaphore_tests);
                // }
                bool test_all_done = true;
                for (int i = 0; i < TEST_NUM; ++i)
                {
                    test_all_done = test_all_done && (test_times[i] > 0);
                }
                if (test_all_done)
                {
                    bool test_all_pass = true;
                    for (int i = 0; i < TEST_NUM; ++i)
                    {
                        test_all_pass = test_all_pass && test_reuslts[i];
                    }
                    if (test_all_pass)
                    {
                        ESP_LOGI(TAG, "*************** Test PASS ***************\n");
                    }
                    else
                    {
                        ESP_LOGE(TAG, "*************** Test FAIL ***************\n");
                    }
                }
            }
        }
        linenoiseFree(line);
    }
    ESP_LOGD(TAG, "The End");
    vTaskDelete(NULL);
}

void register_console(QueueHandle_t *console_queues, const QueueHandle_t test_queue, const QueueHandle_t result_queue, TaskHandle_t *console_task)
{
    queues_tests = console_queues;
    queue_test_result = result_queue;
    queue_test_flag = test_queue;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

#if CONFIG_EXAMPLE_STORE_HISTORY
    initialize_filesystem();
    repl_config.history_save_path = HISTORY_PATH;
#endif

    repl_config.prompt = "ESP32-S3-EYE >";
    esp_err_t ret = initialize_console(&repl_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Initialize console failed !\n");
    }
    else
    {
        xTaskCreatePinnedToCore(task_console_handler, "test_console_process", 4 * 1024, NULL, 5, console_task, 1);
    }
}

static void task_controller_handler(void *arg)
{
    bool testmode_flag = false;
    en_test_state g_state_test = TEST_IDLE;
    int test_times[TEST_NUM] = {0};
    bool test_reuslts[TEST_NUM] = {false};
    int test_flag = KEY_SHORT_PRESS;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    while (true)
    {
        if (test_flag == KEY_SHORT_PRESS)
        {
            ESP_LOGI(TAG, "--------------- Test Start ---------------\n");
            for (int i = 0; i < TEST_NUM; ++i)
            {
                if (test_reuslts[i])
                    continue;

                g_state_test = i;
                xQueueSend(queues_tests[i], &g_state_test, portMAX_DELAY);
                xQueueReceive(queue_test_result, &test_reuslts[i], portMAX_DELAY);
                test_times[i] += 1;
            }

            bool test_all_done = true;
            for (int i = 0; i < TEST_NUM; ++i)
            {
                test_all_done = test_all_done && (test_times[i] > 0);
            }
            if (test_all_done)
            {
                bool test_all_pass = true;
                for (int i = 0; i < TEST_NUM; ++i)
                {
                    test_all_pass = test_all_pass && test_reuslts[i];
                }
                if (test_all_pass)
                {
                    ESP_LOGI(TAG, "*************** Test PASS ***************\n");
                }
                else
                {
                    ESP_LOGE(TAG, "*************** Test FAIL ***************\n");
                }
            }
        }
        xQueueReceive(queue_test_flag, &test_flag, 10 / portTICK_PERIOD_MS);
        xQueueReceive(queue_test_flag, &test_flag, portMAX_DELAY);
    }
    ESP_LOGD(TAG, "The End");
    vTaskDelete(NULL);
}

void register_test_controller(QueueHandle_t *console_queues, const QueueHandle_t test_queue, const QueueHandle_t result_queue)
{
    queues_tests = console_queues;
    queue_test_result = result_queue;
    queue_test_flag = test_queue;
    xTaskCreatePinnedToCore(task_controller_handler, "test_console_process", 4 * 1024, NULL, 5, NULL, 1);
}
