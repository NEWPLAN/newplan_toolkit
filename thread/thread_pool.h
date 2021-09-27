#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <queue>
#include <thread>
#include <mutex>
#include <list>
#include <memory>
#include <algorithm>
#include <condition_variable>
#include <pthread.h>
#include <iostream>
#include <cstring>
#include <functional>

#include <unistd.h>
namespace std
{
    //co-existing with boost::thread_group
    //replace boost::thread with std::thread,boost::shared_mutex with std::mutex, respectively
    class thread_group
    {
    private:
        thread_group(thread_group const &);
        thread_group &operator=(thread_group const &);

    public:
        thread_group()
        {
        }
        ~thread_group()
        {
            for (auto it = threads.begin(), end = threads.end(); it != end; ++it)
            {
                delete *it;
            }
        }

        template <typename F>
        thread *create_thread(F threadfunc)
        {
            lock_guard<mutex> guard(m);
            //auto_ptr<thread> new_thread(new thread(threadfunc));
            unique_ptr<thread> new_thread(new thread(threadfunc));

            //DX: bind cpu
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(threads.size() * 2, &cpuset);
            int rc = pthread_setaffinity_np(new_thread->native_handle(), sizeof(cpu_set_t), &cpuset);
            if (rc != 0) std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
            //end bind cpu

            threads.push_back(new_thread.get());
            return new_thread.release();
        }

        void add_thread(thread *thrd)
        {
            if (thrd)
            {
                lock_guard<mutex> guard(m);
                threads.push_back(thrd);
            }
        }

        void remove_thread(thread *thrd)
        {
            lock_guard<mutex> guard(m);
            auto it = std::find(threads.begin(), threads.end(), thrd);
            if (it != threads.end())
            {
                threads.erase(it);
            }
        }

        void join_all()
        {
            lock_guard<mutex> guard(m);
            for (auto it = threads.begin(), end = threads.end(); it != end; ++it)
            {
                (*it)->join();
            }
        }

        size_t size() const
        {
            lock_guard<mutex> guard(m);
            return threads.size();
        }

    private:
        list<thread *> threads;
        mutable mutex m;
    };
} // namespace std
namespace newplan_toolkit
{
    class ThreadPool
    {
    private:
        std::queue<std::function<void()>> tasks_;
        std::thread_group threads_;
        std::mutex mutex_;
        std::condition_variable condition_;
        std::condition_variable completed_;
        bool running_;
        bool complete_;
        std::size_t available_;
        std::size_t total_;

    public:
        /// @brief Constructor.
        explicit ThreadPool(std::size_t pool_size) :
            running_(true), complete_(true),
            available_(pool_size), total_(pool_size)
        {
            for (std::size_t i = 0; i < pool_size; ++i)
            {
                threads_.create_thread(
                    std::bind(&ThreadPool::main_loop, this));
            }
        }

        /// @brief Destructor.
        ~ThreadPool()
        {
            // Set running flag to false then notify all threads.
            {
                std::unique_lock<std::mutex> lock(mutex_);
                running_ = false;
                condition_.notify_all();
            }

            try
            {
                threads_.join_all();
            }
            // Suppress all exceptions.
            catch (const std::exception &)
            {
            }
        }

        /// @brief Add task to the thread pool if a thread is currently available.
        template <typename Task>
        void runTask(Task task)
        {
            std::unique_lock<std::mutex> lock(mutex_);

            // Set task and signal condition variable so that a worker thread will
            // wake up and use the task.
            tasks_.push(std::function<void()>(task));
            complete_ = false;
            condition_.notify_one();
        }

        /// @brief Wait for queue to be empty
        void waitWorkComplete()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (!complete_)
                completed_.wait(lock);
        }

    private:
        /// @brief Entry point for pool threads.
        void main_loop()
        {
            while (running_)
            {
                // Wait on condition variable while the task is empty and
                // the pool is still running.
                std::unique_lock<std::mutex> lock(mutex_);
                while (tasks_.empty() && running_)
                {
                    condition_.wait(lock);
                }
                // If pool is no longer running, break out of loop.
                if (!running_)
                    break;

                // Copy task locally and remove from the queue.  This is
                // done within its own scope so that the task object is
                // destructed immediately after running the task.  This is
                // useful in the event that the function contains
                // shared_ptr arguments bound via bind.
                {
                    std::function<void()> task = tasks_.front();
                    tasks_.pop();
                    // Decrement count, indicating thread is no longer available.
                    --available_;

                    lock.unlock();

                    // Run the task.
                    try
                    {
                        task();
                    }
                    // Suppress all exceptions.
                    catch (const std::exception &)
                    {
                    }

                    // Update status of empty, maybe
                    // Need to recover the lock first
                    lock.lock();

                    // Increment count, indicating thread is available.
                    ++available_;
                    if (tasks_.empty() && available_ == total_)
                    {
                        complete_ = true;
                        completed_.notify_one();
                    }
                }
            } // while running_
        }
    };
} // namespace newplan_toolkit

#endif