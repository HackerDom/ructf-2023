#pragma once

#include <thread>
#include <functional>
#include <atomic>
#include <future>
#include <condition_variable>
#include <vector>
#include <queue>
#include <mutex>

namespace werk::utils {
    class ThreadPool {
    public:
        explicit ThreadPool(size_t workersCount);

        ~ThreadPool();

        ThreadPool(const ThreadPool &other) = delete;

        ThreadPool(ThreadPool &&other) = delete;

        ThreadPool &operator=(const ThreadPool &other) = delete;

        ThreadPool &operator=(ThreadPool &&other) = delete;

        template<class F, class ...Args>
        auto EnqueueUserTask(F &&action, Args &&... args);

        const size_t workersCount;

    private:
        using TaskT = std::function<void()>;

        std::queue<TaskT> tasksQueue;
        std::mutex tasksQueueMutex;
        std::vector<std::thread> workers;
        std::condition_variable tasksQueueNotifier;
        std::atomic_bool cancellationRequested;

        void DequeueTasksCycle([[maybe_unused]] size_t workerId);

        bool TryDequeueTask(TaskT &dequeuedTask);
    };


    ThreadPool::ThreadPool(size_t workersCount) : workersCount(workersCount) {
        cancellationRequested = false;

        workers.reserve(workersCount);

        for (size_t i = 0; i < workersCount; i++) {
            workers.emplace_back([this, i] { this->DequeueTasksCycle(i); });
        }
    }

    ThreadPool::~ThreadPool() {
        cancellationRequested = true;

        tasksQueueNotifier.notify_all();

        for (auto &worker: workers) {
            worker.join();
        }
    }

    void ThreadPool::DequeueTasksCycle([[maybe_unused]] size_t workerId) {
        while (true) {
            TaskT dequeuedTask;

            if (!TryDequeueTask(dequeuedTask)) {
                break;
            }

            std::invoke(dequeuedTask);
        }
    }

    bool ThreadPool::TryDequeueTask(TaskT &dequeuedTask) {
        std::unique_lock<std::mutex> lock(tasksQueueMutex);
        tasksQueueNotifier.wait(lock, [this] { return cancellationRequested || !tasksQueue.empty(); });

        if (cancellationRequested) {
            return false;
        }

        dequeuedTask = tasksQueue.front();

        tasksQueue.pop();

        return true;
    }

    template<class F, class ... Args>
    auto ThreadPool::EnqueueUserTask(F &&action, Args &&... args) {
        using TaskReturnType = decltype(std::invoke(action, args...));

        // bind instead of lambda because of arguments passing
        auto bind = std::bind(std::forward<F>(action), std::forward<Args>(args)...);
        auto task = std::make_shared<std::packaged_task<TaskReturnType()>>(bind);

        {
            std::lock_guard<std::mutex> lock(tasksQueueMutex);

            tasksQueue.emplace([task] { (*task)(); });
        }

        tasksQueueNotifier.notify_one();

        return task->get_future();
    }
}
