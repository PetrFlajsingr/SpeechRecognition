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
    SafeQueue():keep_running(true){};

    void enqueue(T item){
        std::unique_lock<std::mutex> lock(queueMutex);
        bool wasEmpty = queue.empty();

        queue.push(item);
        lock.unlock();

        if(wasEmpty)
            conditionVariable.notify_one();
    }

    bool dequeue(T& item){
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

    bool isEmpty(){
        return queue.empty();
    }

    void shutdown(){
        keep_running = false;
        conditionVariable.notify_all();
    }
};


#endif //VOICERECOGNITION_SAFEQUEUE_H
