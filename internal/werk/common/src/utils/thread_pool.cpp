#include <utils/thread_pool.hpp>

namespace werk::utils {
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

    size_t ThreadPool::GetQueueSize() {
        std::lock_guard<std::mutex> _(tasksQueueMutex);

        return tasksQueue.size();
    }
}