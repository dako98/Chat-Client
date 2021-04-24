#ifndef __THREADSAFEQUEUE_H__
#define __THREADSAFEQUEUE_H__

#include <queue>
template <typename T>
class ThreadSafeQueue
{
public:
    T waitAndPop()
    {
        std::unique_lock<std::mutex> lock(dataMutex);
        dataCondVar.wait(lock, [this]() { return !this->data.empty(); });
        T &&front = std::move(data.front());
        data.pop();
        lock.unlock();

        return front;
    }

    void waitAndPush(const T &item)
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        data.push(item);

        // Assuming we will only have one consumer
        dataCondVar.notify_one();
    }

    // removed, because we're not currently
    // locking the variable
    size_t size() const
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        return data.size();
    }

    bool empty() const
    {
        return this->size() > 0;
    }

private:
    std::queue<T> data;

    // Mutable, so we can lock and unlock in
    // const methods.
    mutable std::mutex dataMutex;
    mutable std::condition_variable dataCondVar;
};

#endif // __THREADSAFEQUEUE_H__
