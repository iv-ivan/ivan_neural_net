//
//  pool.h
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 23/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#ifndef pool_h
#define pool_h

#include "queue.h"
#include <atomic>
#include <vector>
#include <thread>
#include <future>

class TJoinThreads {
public:
    explicit TJoinThreads(std::vector<std::thread>& threads)
        : Threads(threads)
    {}
    
    ~TJoinThreads() {
        for (size_t i = 0; i < Threads.size(); ++i) {
            if (Threads[i].joinable()) {
                Threads[i].join();
            }
        }
    }
private:
    std::vector<std::thread>& Threads;
};

class TFunctionWrapper {
public:
    template<typename TFunction>
    TFunctionWrapper(TFunction&& f)
        : Impl(std::make_unique<TImplType<TFunction>>(std::move(f)))
    {}
    
    void operator() () {
        Impl->Call();
    }
    
    TFunctionWrapper() = default;
    
    TFunctionWrapper(TFunctionWrapper&& other)
        : Impl(std::move(other.Impl))
    {}

    TFunctionWrapper& operator =(TFunctionWrapper&& other) {
        Impl = std::move(other.Impl);
        return *this;
    }

private:
    struct IImplBase {
        virtual void Call() = 0;
        virtual ~IImplBase() = default;
    };
    std::unique_ptr<IImplBase> Impl;
    
    template<typename TFunction>
    struct TImplType : public IImplBase {
        TFunction F;

        TImplType(TFunction&& f)
            : F(std::move(f))
        {}

        void Call() override {
            F();
        }
    };
private:
    TFunctionWrapper(TFunctionWrapper& other) = delete;
    TFunctionWrapper(const TFunctionWrapper& other) = delete;
    TFunctionWrapper& operator =(const TFunctionWrapper& other) = delete;
};

class TThreadPool {
public:
    TThreadPool()
        : Done(false)
        , Joiner(Threads)
    {
        const size_t threadsCount = std::thread::hardware_concurrency();
        try {
            for (size_t i = 0; i < threadsCount; ++i) {
                Threads.push_back(std::thread(&TThreadPool::WorkerThread, this));
            }
        } catch(...) {
            Done = true;
            throw;
        }
    }
    
    ~TThreadPool() {
        Done = true;
    }
    
    template<typename TFunctionType>
    std::future<typename std::result_of<TFunctionType()>::type> Submit(TFunctionType f) {
        using TResultType = typename std::result_of<TFunctionType()>::type;
        std::packaged_task<TResultType()> task(std::move(f));
        std::future<TResultType> result = task.get_future();
        WorkQueue.Push(std::move(task));
        return result;
    }
private:
    std::atomic_bool Done;
    TThreadSafeQueue<TFunctionWrapper> WorkQueue;
    std::vector<std::thread> Threads;
    TJoinThreads Joiner;
private:
    void WorkerThread() {
        while (!Done) {
            TFunctionWrapper task;
            if (WorkQueue.TryPop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }
};


#endif /* pool_h */
