#ifndef IO_QUEUE_HPP
#define IO_QUEUE_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

/* 
  TODOs:
  - Ensure Push is thread safe
  - Test that threads will wait for new jobs before executing
  - Add callback to publish to kafka stream

 */

// TODO: Make this template into a generic input data class
template<class T>
class IOQueue {
 public:
  IOQueue() : fRunning(false) {
    InitThreadPool();
  }
  ~IOQueue() {
  }

  void Push(const T& inData) {
    std::lock_guard<std::mutex> aLock(fReceiveMutex);
    fUnprocessedData.push(inData);
  }
 private:
  void ThreadMain() {
    while (fRunning) {
      std::unique_lock<std::mutex> aLock(fMutex);
      fConditional.wait(aLock);
      ProcessNextItem();
      aLock.unlock();
    }
  }

  void ProcessNextItem() {
    const T& aData = fUnprocessedData.front();
    // Run callback to publish event
    fConditional.notify_one();
  }

  void InitThreadPool() {
    const unsigned int aMaxThreads = std::thread::hardware_concurrency();
    for (int i = 0; i < aMaxThreads; ++i) {
      std::unique_ptr<std::thread> aThread = std::make_unique<std::thread>(std::bind(&ThreadMain, this));
      fThreadPool.push_back(aThread);
    }
  }

  std::atomic_bool fRunning;
  std::vector<std::unique_ptr<std::thread>> fThreadPool;
  std::mutex fReceiveMutex;
  std::mutex fMutex;
  std::condition_variable fConditional;
  std::queue<T> fUnprocessedData;
};

#endif