#include "IOQueue.hpp"

IOQueue::IOQueue() : fRunning(false) {}

IOQueue::~IOQueue() {
}

void IOQueue::Start() {
  InitThreadPool();
  fRunning = true;
}

void IOQueue::Stop() {
  fRunning = false;
  for (auto& aThread : fThreadPool) {
    if (aThread->joinable()) {
      aThread->join();
    }
  }
}

void IOQueue::Push(std::unique_ptr<GenericMessage> inMessage) {
  std::lock_guard<std::mutex> aLock(fReceiveMutex);
  fUnprocessedMessages.push(std::move(inMessage));
}

void IOQueue::ThreadMain() {
  while (fRunning) {
    WaitForMessage();
  }
}

void IOQueue::WaitForMessage() {
  std::unique_lock<std::mutex> aLock(fTransmitMutex);
  while (fUnprocessedMessages.empty() && fRunning) {
    fConditional.wait(aLock);
  }
  ProcessNextMessage();
  aLock.unlock();
}

void IOQueue::ProcessNextMessage() {
  if (fUnprocessedMessages.size() > 0) {
    auto aMessage = std::move(fUnprocessedMessages.front());
    HandleMessage(std::move(aMessage));
    fUnprocessedMessages.pop();
    fConditional.notify_one();
  }
}

void IOQueue::InitThreadPool() {
  const unsigned int aMaxThreads = std::thread::hardware_concurrency();
  for (int i = 0; i < aMaxThreads; ++i) {
    std::unique_ptr<std::thread> aThread =
        std::make_unique<std::thread>(std::bind(&IOQueue::ThreadMain, this));
    fThreadPool.push_back(std::move(aThread));
  }
}

void IOQueue::SetMessageHandler(MessageHandlerCallback& inCallback) {
  fMessageHandlerCallback = inCallback;
}

void IOQueue::HandleMessage(std::unique_ptr<GenericMessage> inMessage) {
  if (fMessageHandlerCallback) {
    fMessageHandlerCallback(std::move(inMessage));
  }
}
