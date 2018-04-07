//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_SAFEQUEUE_H
#define VOICERECOGNITION_SAFEQUEUE_H


#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

template<typename T> class SafeQueue {
private:
    std::mutex queueMutex;
    std::condition_variable conditionVariable;
    std::queue<T> queue;
    std::atomic<bool> keep_running;
public:
    SafeQueue();

    void enqueue(T item);

    bool dequeue(T& item);

    bool isEmpty();

    void shutdown();
};


#endif //VOICERECOGNITION_SAFEQUEUE_H
