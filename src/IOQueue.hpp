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

// class MessageHandlerCallback {
//  public:
//   MessageHandlerCallback(std::function<void (std::unique_ptr<GenericMessage>)>& inCallback);
//   virtual ~MessageHandlerCallback();
//   void operator()(std::unique_ptr<GenericMessage>);

//  private:
//   std::function<void (std::unique_ptr<GenericMessage>)> fCallback;
// };

typedef std::function<void (std::unique_ptr<GenericMessage>)> MessageHandlerCallback;

class IOQueue {
 public:
  IOQueue();
  ~IOQueue();

  void Start();
  void Stop();
  void Push(std::unique_ptr<GenericMessage> inMessage);
  void SetMessageHandler(MessageHandlerCallback& inCallback);

 private:
  void ThreadMain();
  void WaitForMessage();
  void InitThreadPool();
  void ProcessNextMessage();
  void HandleMessage(std::unique_ptr<GenericMessage> inMessage);

  std::queue<std::unique_ptr<GenericMessage>> fUnprocessedMessages;
  std::vector<std::unique_ptr<std::thread>> fThreadPool;
  MessageHandlerCallback fMessageHandlerCallback;

  std::atomic_bool fRunning;
  mutable std::mutex fReceiveMutex;
  mutable std::mutex fTransmitMutex;
  std::condition_variable fConditional;
};

#endif