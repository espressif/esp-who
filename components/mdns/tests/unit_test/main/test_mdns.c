/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "mdns.h"
#include "esp_event.h"
#include "unity.h"
#include "test_utils.h"

#include "unity_fixture.h"
#include "memory_checks.h"

#define MDNS_HOSTNAME "test-hostname"
#define MDNS_DELEGATE_HOSTNAME "delegate-hostname"
#define MDNS_INSTANCE "test-instance"
#define MDNS_SERVICE_NAME  "_http"
#define MDNS_SERVICE_PROTO "_tcp"
#define MDNS_SERVICE_PORT   80

TEST_GROUP(mdns);

TEST_SETUP(mdns)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(mdns)
{
    test_utils_finish_and_evaluate_leaks(32, 64);
}

static void yield_to_all_priorities(void)
{
    // Lower the test-task priority before testing to ensure other tasks got executed on forced context switch
    size_t test_task_prio_before = uxTaskPriorityGet(NULL);
    vTaskPrioritySet(NULL, tskIDLE_PRIORITY);
    taskYIELD(); // Let the RTOS to switch context
    vTaskPrioritySet(NULL, test_task_prio_before);
}


TEST(mdns, api_fails_with_invalid_state)
{
    TEST_ASSERT_NOT_EQUAL(ESP_OK, mdns_init() );
    TEST_ASSERT_NOT_EQUAL(ESP_OK, mdns_hostname_set(MDNS_HOSTNAME) );
    TEST_ASSERT_NOT_EQUAL(ESP_OK, mdns_instance_name_set(MDNS_INSTANCE) );
    TEST_ASSERT_NOT_EQUAL(ESP_OK, mdns_service_add(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_SERVICE_PORT, NULL, 0) );
}

TEST(mdns, init_deinit)
{
    test_case_uses_tcpip();
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());
    TEST_ASSERT_EQUAL(ESP_OK, mdns_init() );
    yield_to_all_priorities(); // Make sure that mdns task has executed to complete initialization
    mdns_free();
    esp_event_loop_delete_default();
}

TEST(mdns, api_fails_with_expected_err)
{
    mdns_txt_item_t serviceTxtData[CONFIG_MDNS_MAX_SERVICES] = { {NULL, NULL},
    };
    mdns_ip_addr_t addr;
    addr.addr.type = ESP_IPADDR_TYPE_V4;
    addr.addr.u_addr.ip4.addr = esp_ip4addr_aton("127.0.0.1");
    addr.next = NULL;
    for (int i = 0; i < CONFIG_MDNS_MAX_SERVICES; ++i) {
        serviceTxtData[i].key = "Key";
        serviceTxtData[i].value = "Value";
    }
    test_case_uses_tcpip();
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());

    TEST_ASSERT_EQUAL(ESP_OK, mdns_init() );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_hostname_set(MDNS_HOSTNAME) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_delegate_hostname_add(MDNS_DELEGATE_HOSTNAME, &addr) );
    yield_to_all_priorities();  // Make sure that mdns task has executed to add the hostname
    TEST_ASSERT_TRUE(mdns_hostname_exists(MDNS_DELEGATE_HOSTNAME) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_instance_name_set(MDNS_INSTANCE) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_add(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_SERVICE_PORT, serviceTxtData, CONFIG_MDNS_MAX_SERVICES) );
    TEST_ASSERT_FALSE(mdns_service_exists(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_DELEGATE_HOSTNAME) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_add_for_host(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_DELEGATE_HOSTNAME,
                      MDNS_SERVICE_PORT, serviceTxtData, CONFIG_MDNS_MAX_SERVICES) );
    TEST_ASSERT_TRUE(mdns_service_exists(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_DELEGATE_HOSTNAME) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_txt_set(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, serviceTxtData, CONFIG_MDNS_MAX_SERVICES) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_txt_item_set(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, "key1", "value1") );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_txt_item_remove(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, "key1") );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_port_set(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 8080) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_remove(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO) );
    yield_to_all_priorities();  // Make sure that mdns task has executed to remove the service

    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_add(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_SERVICE_PORT, NULL, 0) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_delegate_hostname_remove(MDNS_DELEGATE_HOSTNAME) );
    yield_to_all_priorities();  // Make sure that mdns task has executed to remove the hostname
    TEST_ASSERT_FALSE(mdns_service_exists(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_DELEGATE_HOSTNAME) );
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_remove_all() );
    yield_to_all_priorities();  // Make sure that mdns task has executed to remove all services

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, mdns_service_port_set(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 8080) );

    mdns_free();
    esp_event_loop_delete_default();
}

TEST(mdns, query_api_fails_with_expected_err)
{
    mdns_result_t *results = NULL;
    esp_ip6_addr_t addr6;
    esp_ip4_addr_t addr4;
    test_case_uses_tcpip();
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_loop_create_default());

    TEST_ASSERT_EQUAL(ESP_OK, mdns_init() );
    // check it is not possible to register a service or set an instance without configuring the hostname
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, mdns_service_add(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_SERVICE_PORT, NULL, 0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, mdns_instance_name_set(MDNS_INSTANCE));
    TEST_ASSERT_EQUAL(ESP_OK, mdns_hostname_set(MDNS_HOSTNAME));
    // hostname is set, now adding a service and instance should succeed
    TEST_ASSERT_EQUAL(ESP_OK, mdns_service_add(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, MDNS_SERVICE_PORT, NULL, 0));
    TEST_ASSERT_EQUAL(ESP_OK, mdns_instance_name_set(MDNS_INSTANCE));

    TEST_ASSERT_EQUAL(ESP_OK, mdns_query_ptr(MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 10, CONFIG_MDNS_MAX_SERVICES,  &results) );
    mdns_query_results_free(results);

    TEST_ASSERT_EQUAL(ESP_OK, mdns_query_srv(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 10, &results) );
    mdns_query_results_free(results);

    TEST_ASSERT_EQUAL(ESP_OK, mdns_query_txt(MDNS_INSTANCE, MDNS_SERVICE_NAME, MDNS_SERVICE_PROTO, 10, &results) );
    mdns_query_results_free(results);

    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, mdns_query_a(MDNS_HOSTNAME, 10, &addr4) );
    mdns_query_results_free(results);

    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, mdns_query_aaaa(MDNS_HOSTNAME, 10, &addr6) );
    mdns_query_results_free(results);

    mdns_free();
    esp_event_loop_delete_default();
}

TEST_GROUP_RUNNER(mdns)
{
    RUN_TEST_CASE(mdns, api_fails_with_invalid_state)
    RUN_TEST_CASE(mdns, api_fails_with_expected_err)
    RUN_TEST_CASE(mdns, query_api_fails_with_expected_err)
    RUN_TEST_CASE(mdns, init_deinit)
}

void app_main(void)
{
    UNITY_MAIN(mdns);
}
