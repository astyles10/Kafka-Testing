#include "CustomerDataFactory.hpp"

// Take in json test data

CustomerDataFactory::CustomerDataFactory(const nlohmann::json& inUserDataBlob) {
  LoadUserDataIntoQueue(inUserDataBlob);
}

CustomerDataFactory::~CustomerDataFactory() {

}

void CustomerDataFactory::LoadUserDataIntoQueue(const nlohmann::json& inUserData) {
  // fUsers.
}
