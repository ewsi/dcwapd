#ifndef JSONPARSER_IXX_INCLUDED
#define JSONPARSER_IXX_INCLUDED


#include <fstream>
#include <string>
#include <exception>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

namespace {

typedef rapidjson::GenericDocument<rapidjson::UTF8<>,rapidjson::CrtAllocator> JSONDocument;
typedef rapidjson::GenericValue<rapidjson::UTF8<>,rapidjson::CrtAllocator> JSONValue;
typedef JSONDocument::ValueType JSValue;
typedef JSValue::StringRefType JSStr;

class JSONParserException : public std::exception {
  private:
    const std::string _msg;

  public:
    JSONParserException(const char * const msg) : _msg(msg) { } 
    virtual ~JSONParserException() noexcept { }
    virtual const char* what() const noexcept {
      return _msg.c_str();
    }
};

class JSONParser : public JSONDocument {
public:
  JSONParser(const std::string& filename) {
    std::ifstream ifs(filename.c_str());
    rapidjson::IStreamWrapper isw(ifs);

    if (this->ParseStream(isw).HasParseError()) {
      throw JSONParserException("Failed to parse given JSON");
    }
  }
  virtual ~JSONParser() { }
};

};


#endif //#ifndef JSONPARSER_IXX_INCLUDED
