#include "test_case_imu.hpp"

#include <math.h>

#include "test_logic.h"
#include "qma7981.h"
#include "define.h"

static const char *TAG = "test/imu";

static QueueHandle_t queue_test_result = NULL;
static QueueHandle_t *queues_tests = NULL;

inline bool check_axis(float t, float o1, float o2)
{
#define IMU_LOW 0.25f
    return (fabs(o1) < IMU_LOW) && (fabs(o2) < IMU_LOW) && (t < (-1.0 + IMU_LOW)) && (t > (-1.0 - IMU_LOW));
}

static bool run_case(const char *name, const uint8_t id)
{
    printf(FORMAT_CASE(name));

    int64_t start = esp_timer_get_time();
    bool pass = false;
    float x, y, z;

    while (!pass)
    {
        qma7981_get_acce(&x, &y, &z);
        if (id == 0)
            pass = check_axis(z, x, y);
        else if (id == 1)
            pass = check_axis(x, y, z);
        else if (id == 2)
            pass = check_axis(y, x, z);

        // printf("timeout: %d, a: %f, x: %f, y: %f, z: %f\n", timeout, sqrt(x*x+y*y+z*z), x, y, z);

        if ((esp_timer_get_time() - start) > 10000000)
            break;

        vTaskDelay(10);
    }

    printf(pass ? FORMAT_PASS : FORMAT_FAIL);

    return pass;
}

bool test_imu()
{
    static bool pass = false;

    if (!pass)
    {

        printf(FORMAT_MENU("IMU Test"));

        // initialize
        static bool not_initial = true;
        if (not_initial)
        {
            if (qma7981_init() == ESP_OK)
                not_initial = false;
            else
                ESP_LOGE(TAG, "Failed to init imu.");
        }

        // run test case
        bool pass_z = run_case("Test Z axis", 0);
        bool pass_x = run_case("Test X axis", 1);
        bool pass_y = run_case("Test Y axis", 2);

        pass = pass_z && pass_x && pass_y;
    }

    return pass;
}
