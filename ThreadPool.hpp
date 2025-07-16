#pragma once

#include <Windows.h>

#include <functional>

#include <vector>

#include <memory>

#include <thread>

#include <future>

#include <iostream>

template <typename T> struct STraits;

template <typename R, typename... Args> struct STraits<R(Args...)> {
    using RetType = R;
};

struct SInfo {
    std::function<void()> Task;
};

class CThreadPool {
public:
    template <typename T, typename... Args>
    std::future<void> Run(T Callback, Args&&... Arguments)
    {
        auto task = std::make_shared<std::packaged_task<void()>>(
            std::bind(std::forward<T>(Callback), std::forward<Args>(Arguments)...));

        auto wrapper = new SInfo{
            [task]() { (*task)(); }
        };

        auto Function = [](PTP_CALLBACK_INSTANCE Instance, PVOID DataInfo, PTP_WORK Work) {
                auto Data = static_cast<SInfo*>(DataInfo);
                try
                {
                    Data->Task();
                }
                catch (const std::exception&) {
                }

                CloseThreadpoolWork(Work);
                delete Data;
            };

        const auto Work = CreateThreadpoolWork(Function, wrapper, nullptr);
        if (!Work)
        {
            throw std::runtime_error("Failed to create threadpool work");
        }

        SubmitThreadpoolWork(Work);
        return task->get_future();
    }
};

inline auto ThreadPool = std::make_unique<CThreadPool>();