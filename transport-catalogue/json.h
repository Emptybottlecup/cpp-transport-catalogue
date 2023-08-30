#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <execution>

namespace json {

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

enum class ValueType {
    NULL_,
    ARRAY,
    DICT,
    BOOL_,
    INT,
    DOUBLE,
    STRING,
};

struct IsAnyValue {
    ValueType operator()(std::nullptr_t) const {
        return ValueType::NULL_;
    }
    ValueType operator()([[maybe_unused]] Array arr_) const {
        return ValueType::ARRAY;
    }
    ValueType operator()([[maybe_unused]] Dict dict_) const {
        return ValueType::DICT;
    }
    ValueType operator()([[maybe_unused]] bool bool_) const {
        return ValueType::BOOL_;
    }
    ValueType operator()([[maybe_unused]] int int_) const {
        return ValueType::INT;
    }
    
    ValueType operator()([[maybe_unused]] double double_) const {
        return ValueType::DOUBLE;
    }
    ValueType operator()([[maybe_unused]] std::string string_) const {
        return ValueType::STRING;
    }
};
    
class Node : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
   /* Реализуйте Node, используя std::variant */
    using variant::variant;
    using Value = variant;
    const Value& GetValue() const { return *this; }
    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;
    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    bool operator == (const Node& node) const;
    bool operator != (const Node& node) const;
};

class Document {
public:
    explicit Document(Node root);
    bool operator != (const Document& doc) const;
    bool operator == (const Document& doc) const;
    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
