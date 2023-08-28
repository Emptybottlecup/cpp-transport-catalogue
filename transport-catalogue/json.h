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

    
struct IsAnyValue {
    std::string operator()(std::nullptr_t) const {
        return "NULL";    
    }
    std::string operator()([[maybe_unused]] Array arr_) const {
        return "ARRAY";    
    }
    std::string operator()([[maybe_unused]] Dict dict_) const {
        return "DICT";    
    }
    std::string operator()([[maybe_unused]] bool bool_) const {
        return "BOOL";    
    }
    std::string operator()([[maybe_unused]] int int_) const {
        return "INT";    
    }
    
    std::string operator()([[maybe_unused]] double double_) const {
        return "DOUBLE";    
    }
     std::string operator()([[maybe_unused]] std::string string_) const {
        return "STRING";    
    }
};
    
class Node {
public:
   /* Реализуйте Node, используя std::variant */
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    Node();
    Node(std::nullptr_t);
    Node(Array array);
    Node(Dict map);
    Node(int value);
    Node(std::string value);
    Node(bool value);
    Node(double value);
    const Value& GetValue() const { return value_; }
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
private:
    Value value_;
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
