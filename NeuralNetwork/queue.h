//
//  queue.h
//  NeuralNetwork
//
//  Created by Ivan Ivashkovskii on 23/06/2019.
//  Copyright © 2019 Ivan Ivashkovskii. All rights reserved.
//

#ifndef queue_h
#define queue_h

#include <memory>
#include <mutex>

template<typename T>
class TThreadSafeQueue {
public:
    TThreadSafeQueue()
        : Head(std::make_unique<TNode>())
        , Tail(Head.get())
    {}

    std::shared_ptr<T> WaitAndPop() {
        const std::unique_ptr<TNode> oldHead = WaitAndPop();
        return oldHead->Data;
    }

    void WaitAndPop(T& value) {
        const std::unique_ptr<TNode> oldHead = WaitAndPop(value);
    }

    std::shared_ptr<T> TryPop() {
        const std::unique_ptr<TNode> oldHead = TryPopHead();
        return oldHead ? oldHead->Data : std::shared_ptr<T>();
        return oldHead->Data;
    }
  
    bool TryPop(T& value) {
        const std::unique_ptr<TNode> oldHead = TryPopHead(value);
        return bool(oldHead);
    }
 
    bool Empty() {
        std::unique_lock<std::mutex> headLock(HeadMutex);
        return Head.get() == GetTail();
    }

    void Push(T newValue) {
        std::shared_ptr<T> newData(std::make_shared<T>(std::move(newValue)));
        std::unique_ptr<TNode> p = std::make_unique<TNode>();
        {
            std::lock_guard<std::mutex> tailLock(TailMutex);
            Tail->Data = newData;
            TNode* const newTail = p.get();
            Tail->Next = std::move(p);
            Tail = newTail;
        }
        DataCondition.notify_one();
    }
private:
    struct TNode {
        std::shared_ptr<T> Data;
        std::unique_ptr<TNode> Next;
    };

    std::mutex HeadMutex;
    std::mutex TailMutex;
    std::unique_ptr<TNode> Head;
    TNode* Tail;
    std::condition_variable DataCondition;
private:
    TThreadSafeQueue(const TThreadSafeQueue& other) = delete;
    TThreadSafeQueue& operator =(const TThreadSafeQueue& other) = delete;
    
    TNode* GetTail() {
        std::lock_guard<std::mutex> tailLock(TailMutex);
        return Tail;
    }
    
    std::unique_ptr<TNode> PopHead() {
        std::unique_ptr<TNode> oldHead = std::move(Head);
        Head = std::move(oldHead->Next);
        return oldHead;
    }
    
    std::unique_lock<std::mutex> WaitForData() {
        std::unique_lock<std::mutex> headLock(HeadMutex);
        DataCondition.wait(headLock, [&] {return Head.Get() != GetTail();});
        return headLock;
    }
    
    std::unique_ptr<TNode> WaitPopHead() {
        std::unique_lock<std::mutex> headLock(WaitForData());
        return PopHead();
    }
    
    std::unique_ptr<TNode> WaitPopHead(T& value) {
        std::unique_lock<std::mutex> headLock(WaitForData());
        value = std::move(*Head->Data);
        return PopHead();
    }
    
    std::unique_ptr<TNode> TryPopHead() {
        std::unique_lock<std::mutex> headLock(HeadMutex);
        if (Head.get() == GetTail()) {
            return std::unique_ptr<TNode>();
        }
        return PopHead();
    }
    
    std::unique_ptr<TNode> TryPopHead(T& value) {
        std::unique_lock<std::mutex> headLock(HeadMutex);
        if (Head.get() == GetTail()) {
            return std::unique_ptr<TNode>();
        }
        value = std::move(*Head->Data);
        return PopHead();
    }
};

#endif /* queue_h */
