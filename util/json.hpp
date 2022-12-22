#pragma once

#include <sstream>

#include "json/json.h"

inline void jsonToString(const Json::Value &json, std::string *res) {
  std::stringstream ss;
  std::string dataStr;
  Json::StreamWriterBuilder builder;
  std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
  writer->write(json, &ss);
  *res = ss.str();
}

inline bool stringToJson(const char *str, int len, Json::Value *json,
                         std::string *error) {
  Json::CharReaderBuilder rbuilder;
  std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
  return reader->parse(str, str + len, json, error);
}