// Code modified by Bernardo Cohen
// Original code by Antoine Savine
/*
Written by Antoine Savine in 2018

This code is the strict IP of Antoine Savine

License to use and alter this code for personal and commercial applications
is freely granted to any person or company who purchased a copy of the book

Modern Computational Finance: AAD and Parallel Simulations
Antoine Savine
Wiley, 2018

As long as this comment is preserved at the top of the file
*/

#pragma once
#ifndef CONCURRENT_QUEUE_HPP
#define CONCURRENT_QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>
using namespace std;

template <class T> class ConcurrentQueue {

  queue<T> myQueue;
  mutable mutex myMutex;
  condition_variable myCV;
  bool myInterrupt;

public:
  ConcurrentQueue() : myInterrupt(false) {}
  ~ConcurrentQueue() { interrupt(); }

  bool empty() const {
    lock_guard<mutex> lk(myMutex);
    return myQueue.empty();
  }

  bool tryPop(T &t) {
    lock_guard<mutex> lk(myMutex);
    if (myQueue.empty())
      return false;
    t = std::move(myQueue.front());
    myQueue.pop();

    return true;
  }

  void push(T t) {
    {
      lock_guard<mutex> lk(myMutex);
      myQueue.push(std::move(t));
    }

    myCV.notify_one();
  }

  bool pop(T &t) {
    unique_lock<mutex> lk(myMutex);

    while (!myInterrupt && myQueue.empty())
      myCV.wait(lk);

    if (myInterrupt)
      return false;

    t = std::move(myQueue.front());
    myQueue.pop();

    return true;
  }

  void interrupt() {
    {
      lock_guard<mutex> lk(myMutex);
      myInterrupt = true;
    }
    myCV.notify_all();
  }

  void resetInterrupt() { myInterrupt = false; }

  void clear() {
    queue<T> empty;
    swap(myQueue, empty);
  }
};

#endif // CONCURRENT_QUEUE_HPP