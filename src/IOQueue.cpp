#include "IOQueue.hpp"

IOQueue::IOQueue() : fRunning(false) {}

IOQueue::~IOQueue() {
}

void IOQueue::Start() {
  InitThreadPool();
  fRunning = true;
}

void IOQueue::Stop() {
  for (auto& aThread : fThreadPool) {
    if (aThread->joinable()) {
      aThread->join();
    }
  }
}

void IOQueue::Push(std::unique_ptr<GenericMessage> inData) {
  std::lock_guard<std::mutex> aLock(fReceiveMutex);
  fUnprocessedData.push(std::move(inData));
}

void IOQueue::ThreadMain() {
  while (fRunning) {
    std::unique_lock<std::mutex> aLock(fTransmitMutex);
    while (fUnprocessedData.empty() && fRunning) {
      fConditional.wait(aLock);
    }
    ProcessNextItem();
    aLock.unlock();
  }
}

void IOQueue::ProcessNextItem() {
  if (fUnprocessedData.size() > 0) {
    const auto& aData = std::move(fUnprocessedData.front());
    (void)aData->Get();
    fUnprocessedData.pop();
    // Run callback to publish event
  }
  fConditional.notify_one();
}

void IOQueue::InitThreadPool() {
  const unsigned int aMaxThreads = std::thread::hardware_concurrency();
  for (int i = 0; i < aMaxThreads; ++i) {
    std::unique_ptr<std::thread> aThread =
        std::make_unique<std::thread>(std::bind(&IOQueue::ThreadMain, this));
    fThreadPool.push_back(std::move(aThread));
  }
}
