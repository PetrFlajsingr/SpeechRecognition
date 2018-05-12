//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_SAFEQUEUE_H
#define VOICERECOGNITION_SAFEQUEUE_H


#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <constants.h>
#include <android/log.h>

/**
* @brief A template queue to pass data between threads - thread safe
*
* @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
*/
template<typename T> class SafeQueue {
private:
    std::mutex queueMutex;
    std::condition_variable conditionVariable;
    std::queue<T> queue;
    std::atomic<bool> keep_running;

public:
    SafeQueue():keep_running(true){};

    /**
     * Add an item to the end of the queue
     * @param item item to be added
     */
    void enqueue(T item){
        std::unique_lock<std::mutex> lock(queueMutex);
        bool wasEmpty = queue.empty();

        queue.push(item);
        lock.unlock();

        if(wasEmpty)
            conditionVariable.notify_one();
    }

    /**
     * Get a first item from the queue and remove it
     * @param item returned item
     * @return true if queue is still active, else false
     */
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

    int size(){
        return queue.size();
    }
};


#endif //VOICERECOGNITION_SAFEQUEUE_H
