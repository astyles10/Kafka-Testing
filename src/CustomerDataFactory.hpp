#ifndef CUSTOMER_DATA_FACTORY_HPP
#define CUSTOMER_DATA_FACTORY_HPP

#include <nlohmann/json.hpp>
#include <queue>

#include "IOQueue.hpp"
// Read file
// Get each user as Json object
// Add to queue
// Start processing thread popping users from queue and publishing

class CustomerDataFactory {
 public:
  CustomerDataFactory(const nlohmann::json& inUserDataBlob);
  ~CustomerDataFactory();
 private:
  void LoadUserDataIntoQueue(const nlohmann::json& inUserData);
  IOQueue<nlohmann::json> fUsers;
};

#endif