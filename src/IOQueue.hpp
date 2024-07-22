#ifndef IO_QUEUE_HPP
#define IO_QUEUE_HPP

/* 
  IOQueue is a pool of threads which operate on a shared queue.
 */

#include "Messages/GenericMessage.hpp"
#include "Messages/JsonMessage.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

/* 
  TODOs:
  - Add callback to publish to kafka stream

 */

class IOQueue {
 public:
  IOQueue();
  ~IOQueue();

  void Start();
  void Stop();
  void Push(std::unique_ptr<GenericMessage> inData);
  void SetDataHandler();

 private:
  void ThreadMain();
  void ProcessNextItem();
  void InitThreadPool();

  std::atomic_bool fRunning;
  std::queue<std::unique_ptr<GenericMessage>> fUnprocessedData;
  std::vector<std::unique_ptr<std::thread>> fThreadPool;
  mutable std::mutex fReceiveMutex;
  mutable std::mutex fTransmitMutex;
  std::condition_variable fConditional;
};

#endif