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

template <class T>
class QueueMock {
public:
    QueueMock(void): q(), m(), c() {}
    ~QueueMock(void) {}

    void send(std::unique_ptr<T> t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(std::move(t));
        c.notify_one();
    }

    std::unique_ptr<T> receive(uint32_t ms)
    {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty()) {
            if (c.wait_for(lock, std::chrono::milliseconds(ms)) == std::cv_status::timeout) {
                return nullptr;
            }
        }
        std::unique_ptr<T> val = std::move(q.front());
        q.pop();
        return val;
    }

private:
    std::queue<std::unique_ptr<T>> q;
    mutable std::mutex m;
    std::condition_variable c;
};
