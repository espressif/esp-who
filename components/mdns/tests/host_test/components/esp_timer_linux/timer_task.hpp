/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <atomic>

typedef void (*cb_t)(void *arg);

class TimerTaskMock {
public:
    TimerTaskMock(cb_t cb): cb(cb), active(false), ms(INT32_MAX) {}
    ~TimerTaskMock(void)
    {
        active = false;
        t.join();
    }

    void SetTimeout(uint32_t m)
    {
        ms = m;
        active = true;
        t = std::thread(run_static, this);
    }

private:

    static void run_static(TimerTaskMock *timer)
    {
        timer->run();
    }

    void run(void)
    {
        while (!active.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        while (active.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            cb(nullptr);
        }
    }

    cb_t cb;
    std::thread t;
    std::atomic<bool> active;
    uint32_t ms;

};
