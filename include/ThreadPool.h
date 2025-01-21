#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <future>   // For std::future and std::packaged_task
#include <vector>   // For std::vector
#include <queue>    // For std::queue
#include <mutex>    // For std::mutex and std::unique_lock
#include <condition_variable> // For std::condition_variable
#include <functional> // For std::function and std::bind
#include <thread>   // For std::thread
#include <stdexcept> // For std::runtime_error
#include <memory>   // For std::shared_ptr

class ThreadPool {
public:
    // Constructor: Creates a thread pool with the specified number of threads.
    ThreadPool(size_t threadCount) : running_(true) {
        for (size_t i = 0; i < threadCount; ++i) {
            // Create and start worker threads. Each thread executes workerThread().
            workers.emplace_back([this]() { workerThread(); });
        }
    }

    // Destructor: Stops the thread pool and joins all worker threads.
    ~ThreadPool() {
        {
            // Acquire lock to safely modify running_
            std::unique_lock<std::mutex> lock(queueMutex_);
            running_ = false; // Signal worker threads to stop
        } // Lock is released here! Crucial to prevent deadlock.
        condition_.notify_all(); // Wake up all waiting threads
        for (std::thread& worker : workers) {
            worker.join(); // Wait for all worker threads to finish
        }
    }

    // Enqueue: Adds a task to the thread pool.
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using return_type = std::invoke_result_t<F, Args...>;

        // Create a packaged task to manage the task and its result.
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...) // Bind function and arguments
        );

        std::future<return_type> res = task->get_future(); // Get the future for the result

        {
            // Acquire lock to safely access the task queue.
            std::unique_lock<std::mutex> lock(queueMutex_);
            if (!running_) {
                throw std::runtime_error("enqueue on stopped ThreadPool"); // Throw if enqueue is called after destructor
            }
            // Push the task onto the queue. Lambda captures the shared_ptr to the task.
            taskQueue_.push([task]() { (*task)(); });
        } // Lock is released here

        condition_.notify_one(); // Notify one waiting worker thread
        return res; // Return the future
    }

private:
    // workerThread: Function executed by each worker thread.
    void workerThread() {
        while (true) {
            std::function<void()> task;
            {
                // Acquire lock to access the task queue and running_ flag.
                std::unique_lock<std::mutex> lock(queueMutex_);
                // Wait for a task or for the thread pool to be stopped. Predicate prevents spurious wakeups.
                condition_.wait(lock, [this]() { return !taskQueue_.empty() || !running_; });
                if (!running_ && taskQueue_.empty()) {
                    return; // Exit if the thread pool is stopped and there are no more tasks.
                }
                // Retrieve the task from the queue using move semantics.
                task = std::move(taskQueue_.front());
                taskQueue_.pop();
            } // Lock is released here! Before executing the task.
            task(); // Execute the task (outside the lock).
        }
    }

    std::vector<std::thread> workers; // Vector of worker threads
    std::queue<std::function<void()>> taskQueue_; // Queue of tasks
    std::mutex queueMutex_; // Mutex to protect shared data
    std::condition_variable condition_; // Condition variable for thread synchronization
    bool running_; // Flag to indicate if the thread pool is running
};

#endif