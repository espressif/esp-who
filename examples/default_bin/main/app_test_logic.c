#include "esp_log.h"
#include "linenoise/linenoise.h"
#include "esp_vfs_dev.h"
#include "esp_console.h"
#include "esp_vfs_cdcacm.h"
#include "esp_vfs_usb_serial_jtag.h"
#include "driver/uart.h"
#include "driver/usb_serial_jtag.h"
#include <fcntl.h>
#include "app_test_logic.h"

#define CONSOLE_PROMPT_MAX_LEN (32)
#define CONSOLE_PATH_MAX_LEN   (ESP_VFS_PATH_MAX)

static const char *TAG = "ESPS3-EYE";
char *console_prompt = NULL;
SemaphoreHandle_t *queues_tests = NULL;
SemaphoreHandle_t semaphore_tests = NULL;


typedef struct {
    esp_console_repl_t repl_core;        // base class
    char prompt[CONSOLE_PROMPT_MAX_LEN]; // Prompt to be printed before each line
    const char *history_save_path;
    TaskHandle_t task_hdl;              // REPL task handle
    size_t max_cmdline_length;          // Maximum length of a command line. If 0, default value will be used.
} esp_console_repl_com_t;

typedef struct {
    esp_console_repl_com_t repl_com; // base class
    int uart_channel;                // uart channel number
} esp_console_repl_universal_t;


static esp_err_t esp_console_setup_prompt(const char *prompt, esp_console_repl_com_t *repl_com)
{
    /* set command line prompt */
    const char *prompt_temp = "esp>";
    if (prompt) {
        prompt_temp = prompt;
    }
    snprintf(repl_com->prompt, CONSOLE_PROMPT_MAX_LEN - 1, LOG_COLOR_I "%s " LOG_RESET_COLOR, prompt_temp);

    /* Figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();
    if (probe_status) {
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
    if (history_path) {
        /* Load command history from filesystem */
        linenoiseHistoryLoad(history_path);
    }

    /* Set command history size */
    if (linenoiseHistorySetMaxLen(max_history_len) != 1) {
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
    if (max_cmdline_length != 0) {
        console_config.max_cmdline_length = max_cmdline_length;
        repl_com->max_cmdline_length = max_cmdline_length;
    }

#if CONFIG_LOG_COLORS
    console_config.hint_color = atoi(LOG_COLOR_CYAN);
#endif
    ret = esp_console_init(&console_config);
    if (ret != ESP_OK) {
        goto _exit;
    }

    ret = esp_console_register_help_command();
    if (ret != ESP_OK) {
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

    usb_serial_jtag_repl = (esp_console_repl_universal_t*)calloc(1, sizeof(esp_console_repl_universal_t));
    if (!usb_serial_jtag_repl) {
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
    while (true)
    {
        xSemaphoreTake(semaphore_tests, portMAX_Delay);
        vTaskDelay(100);
        char *line = linenoise(console_prompt);
        if (line == NULL)
        {
            printf("empty line\n");
            /* Ignore empty lines */
            continue;
        }
        else
        {
            switch (line)
            {
            case "auto":

                auto_test = true;
                x
                break;

            case "id0":

                break;
            
            default:
                break;
            }
            printf("%s\n", line);
        }
        linenoiseFree(line);
    }
    ESP_LOGD(TAG, "The End");
    vTaskDelete(NULL);
}


void register_console(QueueHandle_t *console_queues, SemaphoreHandle_t console_semaphore)
{
    queues_tests = console_queues;
    semaphore_tests = console_semaphore;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "ESPS3-EYE >";
    esp_err_t ret = initialize_console(&repl_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Initialize console failed !\n");
    }
    else
    {
        xTaskCreatePinnedToCore(task_console_handler, "test_console_process", 2 * 1024, NULL, 5, NULL, 1);
    }
}
