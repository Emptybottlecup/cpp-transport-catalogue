#include "json_builder.h"

namespace json {

Builder::Builder() {
    Node* root_ptr = &root_;
    nodes_stack_.emplace_back(root_ptr);
}

DictKeyContext Builder::Key(std::string key) {
    if (nodes_stack_.empty()) {
    throw std::logic_error("wrong action");
  }
    auto &top_node = nodes_stack_.back()->GetValue();

  if (!nodes_stack_.back()->IsDict()) {
    throw std::logic_error("wrong");
  }

  nodes_stack_.emplace_back(&(std::get<Dict>(top_node)[key]));

    return *this;
}

Builder& Builder::Value(Node::Value value) {
     if (nodes_stack_.empty()) {
    throw std::logic_error("wrong action");
  }
    auto &top_node = nodes_stack_.back()->GetValue();

  if (std::holds_alternative<Array>(top_node)) {
    auto &node = std::get<Array>(top_node).emplace_back(GetNode(value));

    if (std::holds_alternative<Array>(value)
        || std::holds_alternative<Dict>(value)) {
      nodes_stack_.emplace_back(&node);
    }
  } else {
    top_node = value;
    nodes_stack_.pop_back();
  }

    return *this;
}

DictItemContext Builder::StartDict() {
    if (nodes_stack_.empty()) {
    throw std::logic_error("wrong action");
  }
    auto &top_node = nodes_stack_.back()->GetValue();

  if (std::holds_alternative<Array>(top_node)) {
    auto &node = std::get<Array>(top_node).emplace_back(Dict());
      nodes_stack_.emplace_back(&node);
  } else {
    top_node = Dict();
  }

    return *this;
}

Builder& Builder::EndDict() {
    if (nodes_stack_.empty()) {
    throw std::logic_error("wrong action");
  }
  if (!nodes_stack_.back()->IsDict()) {
    throw std::logic_error("Error with dict");
  }

  nodes_stack_.pop_back();

  return *this;
}

ArrayItemContext Builder::StartArray() {
    if(nodes_stack_.empty()) {
    throw std::logic_error("wrong action");
  }
    auto &top_node = nodes_stack_.back()->GetValue();

  if (std::holds_alternative<Array>(top_node)) {
    auto &node = std::get<Array>(top_node).emplace_back(Array());
      nodes_stack_.emplace_back(&node);
  } else {
    top_node = Array();
  }

    return *this;
}

Builder& Builder::EndArray() {
    if (nodes_stack_.empty()) {
    throw std::logic_error("wrong action");
  }
  if (!nodes_stack_.back()->IsDict()) {
    throw std::logic_error("Error with array");
  }

  nodes_stack_.pop_back();

  return *this;
}

Node Builder::Build() {
    if (root_.IsNull() || nodes_stack_.size() > 1) throw std::logic_error("Wrong Build()");
    return root_;
}

Node Builder::GetNode(Node::Value value) {
    Node val;
    val.GetValue() = value;
    return val;
}

DictItemContext::DictItemContext(Builder& builder)
    : builder_(builder)
{}

DictKeyContext DictItemContext::Key(std::string key) {
    return builder_.Key(key);
}

Builder& DictItemContext::EndDict() {
    return builder_.EndDict();
}

ArrayItemContext::ArrayItemContext(Builder& builder)
    : builder_(builder)
{}

ArrayItemContext ArrayItemContext::Value(Node::Value value) {
    return ArrayItemContext(builder_.Value(value));
}

DictItemContext ArrayItemContext::StartDict() {
    return builder_.StartDict();
}

ArrayItemContext ArrayItemContext::StartArray() {
    return builder_.StartArray();
}

Builder& ArrayItemContext::EndArray() {
    return builder_.EndArray();
}

DictKeyContext::DictKeyContext(Builder& builder)
    : builder_(builder)
{}

DictItemContext DictKeyContext::Value(Node::Value value) {
    return DictItemContext(builder_.Value(value));
}

ArrayItemContext DictKeyContext::StartArray() {
    return builder_.StartArray();
}

DictItemContext DictKeyContext::StartDict() {
    return builder_.StartDict();
}

} // namespace json
