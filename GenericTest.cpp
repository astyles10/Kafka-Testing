#include <iostream>
#include <nlohmann/json.hpp>

class Generic {
 public:
  Generic() = default;
  ~Generic() = default;
  virtual std::string Get() = 0;
};

class Specific : public Generic {
 public:
  Specific(int inValue) {
    fData = inValue;
  }
  std::string Get() {
    return std::to_string(fData);
  }
 private:
  int fData;
};

class JsonStream : public Generic {
 public:
  JsonStream(const nlohmann::json& inJson) {
    fData = inJson;
  }
  std::string Get() {
    return fData.dump();
  }
 private:
  nlohmann::json fData;
};

void PrintGeneric(Generic& inGeneric) {
  std::cout << inGeneric.Get() << std::endl;
}

int main(int argc, char** argv) {
  Specific aGeneric(10);
  PrintGeneric(aGeneric);

  JsonStream aJsonStr({{"one", 1}, {"two", 2}});
  PrintGeneric(aJsonStr);

  return 0;
}