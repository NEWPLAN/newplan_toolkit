#ifndef __NEWPLAN_BLOCKING_QUEUE_HPP__
#define __NEWPLAN_BLOCKING_QUEUE_HPP__

// usage: BlockingQueue<int> * bq = new BlockingQueue<int>();

#include <queue>
#include <string>
#include <vector>
#include <functional>
#include <condition_variable>
#include <iostream>

namespace newplan_toolkit
{
    template <typename T>
    class BlockingQueue
    {
    public:
        BlockingQueue()
        {
        }
        virtual ~BlockingQueue()
        {
        }

        void push(const T &t)
        {
            {
                std::lock_guard<std::mutex> lg(mutex_);
                queue_.push(t);
            }
            condition_.notify_one();
        }
        // This logs a message if the threads needs to be blocked
        // useful for detecting e.g. when data feeding is too slow
        T pop(const char *log_on_wait)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (queue_.empty())
            {
                if (pop_count++ % 10000)
                    std::cout << log_on_wait << std::endl;
                condition_.wait(lock);
            }
            T t = queue_.front();
            queue_.pop();
            return t;
        }
        T pop()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (queue_.empty())
            {
                condition_.wait(lock);
            }
            T t(queue_.front());
            queue_.pop();
            return t;
        }

        bool try_peek(T *t)
        {
            std::lock_guard<std::mutex> lg(mutex_);
            if (queue_.empty())
            {
                return false;
            }
            *t = queue_.front();
            return true;
        }
        bool try_pop(T *t)
        {
            std::lock_guard<std::mutex> lg(mutex_);
            if (queue_.empty())
            {
                return false;
            }
            *t = queue_.front();
            queue_.pop();
            return true;
        }

        // Return element without removing it
        T peek()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (queue_.empty())
            {
                condition_.wait(lock);
            }
            return queue_.front();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lg(mutex_);
            return queue_.size();
        }

        bool nonblocking_size(size_t *size) const
        {
            if (mutex_.try_lock())
            {
                *size = queue_.size();
                mutex_.unlock();
                return true;
            }
            return false;
        }

    protected:
        std::queue<T> queue_;
        mutable std::mutex mutex_;
        std::condition_variable condition_;
        uint64_t pop_count = 0;
    };
} // namespace newplan_toolkit
#endif