//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <SafeQueue.h>


template<typename T>
SafeQueue<T>::SafeQueue():keep_running(true) {}

/**
 * Adds and item to the queue
 */
template<typename T>
void SafeQueue<T>::enqueue(T item) {
    std::unique_lock<std::mutex> lock(queueMutex);
    bool wasEmpty = queue.empty();

    queue.push(item);
    lock.unlock();

    if(wasEmpty)
        conditionVariable.notify_one();
}

/**
 * Returns the first item in queue via argument
 * @return true on success, false on shutdown
 */
template<typename T>
bool SafeQueue<T>::dequeue(T &item) {
    std::unique_lock<std::mutex> lock(queueMutex);
    while(keep_running && queue.empty()){
        conditionVariable.wait(lock);
    }

    if(keep_running && !queue.empty()){
        item = std::move(queue.front());
        queue.pop();
    }

    return keep_running;
}

template<typename T>
bool SafeQueue<T>::isEmpty() {
    return queue.empty();
}

/**
 * Disable queue
 */
template<typename T>
void SafeQueue<T>::shutdown() {
    keep_running = false;
    conditionVariable.notify_all();
}