#include "dl_model_base.hpp"
#include "dl_module_add.hpp"
#include "dl_module_creator.hpp"
#include "dl_module_relu.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "unity.h"
#include <type_traits>
static const char *TAG = "TEST DL MODEL";

using namespace dl;

uint8_t key[16] = {0x8a, 0x7f, 0xc9, 0x61, 0xe4, 0xe6, 0xff, 0x0a, 0xd2, 0x64, 0x36, 0x95, 0x28, 0x75, 0xae, 0x4a};

TEST_CASE("Test dl model API: load()", "[api]")
{
    ESP_LOGI(TAG, "Test dl model API: load()");
    int internal_ram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    int psram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    Model *model = new Model("model", 0, fbs::MODEL_LOCATION_IN_FLASH_PARTITION, 0, MEMORY_MANAGER_GREEDY, key);
    delete model;
    module::ModuleCreator::get_instance()->clear();

    int internal_ram_size_second = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    fbs::FbsLoader *fbs_loader = new fbs::FbsLoader("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);
    fbs::FbsModel *fbs_model = fbs_loader->load(0, key);
    Model *model2 = new Model(fbs_model);
    delete model2;
    delete fbs_loader;
    delete fbs_model;
    module::ModuleCreator::get_instance()->clear();

    int internal_ram_size_end = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    int psram_size_end = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);

    ESP_LOGI(TAG,
             "internal ram size before: %d, second: %d, end:%d",
             internal_ram_size_before,
             internal_ram_size_second,
             internal_ram_size_end);
    ESP_LOGI(TAG, "psram size before: %d, end:%d", psram_size_before, psram_size_end);
    TEST_ASSERT_EQUAL(true, internal_ram_size_before - internal_ram_size_second < 1000);
    TEST_ASSERT_EQUAL(true, internal_ram_size_second == internal_ram_size_end);
    TEST_ASSERT_EQUAL(true, psram_size_before == psram_size_end);
}

TEST_CASE("Test dl model API: run()", "[api]")
{
    ESP_LOGI(TAG, "Test dl model API: run()");
    Model *model = new Model("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);
    delete model;

    int total_ram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT);

    dl::tool::Latency latency;
    for (int i = 0; i < 15; i++) {
        model = new Model("model", 0, fbs::MODEL_LOCATION_IN_FLASH_PARTITION, i * 20000);

        latency.start();
        model->run();
        latency.end();
        printf("run:%ld ms\n", latency.get_period() / 1000);
        delete model;
    }

    int total_ram_size_end = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    TEST_ASSERT_EQUAL(true, total_ram_size_before == total_ram_size_end);
}

TEST_CASE("Test dl module API: run()", "[api]")
{
    ESP_LOGI(TAG, "Test dl module API: run()");
    int total_ram_size_before = heap_caps_get_free_size(MALLOC_CAP_8BIT);

    TensorBase *input1 = new TensorBase({1, 3, 64, 64}, nullptr, 0, DATA_TYPE_INT8);
    TensorBase *input2 = new TensorBase({1, 1, 1, 64}, nullptr, 0, DATA_TYPE_INT8);
    TensorBase *output = new TensorBase({1, 3, 64, 64}, nullptr, 0, DATA_TYPE_INT8);

    // single input and single output
    module::Module *relu_op = new module::Relu("relu", MODULE_NON_INPLACE, QUANT_TYPE_SYMM_8BIT);
    relu_op->run(input1, output);
    for (int i = 0; i < output->get_size(); i++) {
        int8_t in = input1->get_element<int8_t>(i);
        int8_t out = output->get_element<int8_t>(i);
        if (in > 0) {
            TEST_ASSERT_EQUAL(true, in == out);
        } else {
            TEST_ASSERT_EQUAL(true, out == 0);
        }
    }

    // multiple inputs and multiple outputs
    module::Module *add_op = new module::Add("add", MODULE_NON_INPLACE, QUANT_TYPE_SYMM_8BIT);
    add_op->run({input1, input2}, {output});

    for (int i = 0; i < output->get_size(); i++) {
        int8_t in1 = input1->get_element<int8_t>(i);
        int8_t in2 = input1->get_element<int8_t>(i % 64);
        int8_t out = output->get_element<int8_t>(i);
        TEST_ASSERT_EQUAL(true, in1 + in2 == out);
    }

    delete input1;
    delete input2;
    delete output;
    delete relu_op;
    delete add_op;

    int total_ram_size_end = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    TEST_ASSERT_EQUAL(true, total_ram_size_before == total_ram_size_end);
}
