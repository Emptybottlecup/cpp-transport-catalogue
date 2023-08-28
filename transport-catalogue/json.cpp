#include "json.h"

using namespace std;

namespace json {
 bool Node::operator == (const Node& node) const {
    if(visit(IsAnyValue{},GetValue()) == visit(IsAnyValue{},node.GetValue())) {
        return value_ == node.GetValue();    
    }
    return false;
}
    
bool Node::operator != (const Node& node) const {
    if(visit(IsAnyValue{},GetValue()) == visit(IsAnyValue{},node.GetValue())) {
        return !(value_ == node.GetValue());    
    }
    return true;
}    
    
bool Node::IsInt() const {
     if (visit(IsAnyValue{},GetValue()) == "INT") {
        return true;
    } else {
        return false;
    }          
}
    
bool Node::IsDouble() const {
     if (visit(IsAnyValue{},GetValue()) == "INT" || visit(IsAnyValue{},value_) == "DOUBLE") {
        return true;
    } else {
        return false;
    }          
}
    
 bool Node::IsPureDouble() const {
     if (visit(IsAnyValue{},GetValue()) == "DOUBLE") {
        return true;
    } else {
        return false;
    }          
}
    
 bool Node::IsBool() const {
     if (visit(IsAnyValue{},GetValue()) == "BOOL") {
        return true;
    } else {
        return false;
    }          
}   

bool Node::IsString() const {
     if (visit(IsAnyValue{},GetValue()) == "STRING") {
        return true;
    } else {
        return false;
    }          
}   
    
bool Node::IsNull() const {
    if (visit(IsAnyValue{},GetValue()) == "NULL") {
        return true;
    } else {
        return false;
    }        
}
    
bool Node::IsArray() const {
        if (visit(IsAnyValue{},GetValue()) == "ARRAY") {
            return true;
        }
        
        else {
            return false;
         }            
    }
    
    bool Node::IsMap() const {
        if (visit(IsAnyValue{},GetValue()) == "DICT") {
           return true;
        } 
        else {
           return false;
        }              
    }
    
    int Node::AsInt() const {
        if(visit(IsAnyValue{},GetValue()) != "INT") {
           throw std::logic_error("Other Type");
       }
       return get<int>(value_);
   }
    
    bool Node::AsBool() const {
        if(visit(IsAnyValue{},GetValue()) != "BOOL") {
           throw std::logic_error("Other Type");
       }
       return get<bool>(value_);
    }
    
    double Node::AsDouble() const {
        if(visit(IsAnyValue{},GetValue()) != "DOUBLE" && visit(IsAnyValue{},GetValue()) != "INT") {
           throw std::logic_error("Other Type");
       }
        if(visit(IsAnyValue{},GetValue()) == "INT") {
            return static_cast<double>(get<int>(value_));
        }
       return get<double>(value_);
    }
    
    const std::string& Node::AsString() const {
        if(visit(IsAnyValue{},GetValue()) != "STRING") {
           throw std::logic_error("Other Type");
       }
       return get<std::string>(value_);    
    }
    const Array& Node::AsArray() const {
                if(visit(IsAnyValue{},GetValue()) != "ARRAY") {
           throw std::logic_error("Other Type");
       }
       return get<Array>(value_);    
    }
    const Dict& Node::AsMap() const {
              if(visit(IsAnyValue{},GetValue()) != "DICT") {
           throw std::logic_error("Other Type");
       }
       return get<Dict>(value_);  
    }
    
Node LoadNode(istream& input);

Node LoadArray(istream& input) {
   Array arr;
    for (char c; input >> c && c != ']';) {
        
        if (c != ',') {
            input.putback(c);
        }
        arr.push_back(LoadNode(input));
    }
    if (!input) {
        throw ParsingError("Error value"s);
    }
    return Node(std::move(arr));
}

    
    using Number = std::variant<int, double>;
    
    Node LoadString(std::istream& input) {
    using namespace std::literals;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("Error value");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("Error value");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Error value");
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Error value"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return Node(move(s));
}
    
    
    
    

Node LoadDict(istream& input) {
 Dict dict;
      
    for (char c; input >> c && c != '}';) {
        if (c == '"') { 
            std::string key = LoadString(input).AsString();
            if (input >> c && c == ':') {
                if (dict.count(key) != 0) {
                    throw ParsingError("Duplicate");
                }
                Node value = LoadNode(input);
                dict.emplace(std::move(key), value);
            } else {
                throw ParsingError("Error value");
            }
        } else if (c != ',') {
            throw ParsingError("Error key");
        }
    }
    if (!input) {
        throw ParsingError("Nothing in input");
    }
    return Node(std::move(dict));   
}
    
Node LoadNull([[maybe_unused]] istream& input) {
    std::string s;
    while (std::isalpha(input.peek())) {
        s.push_back(static_cast<char>(input.get()));
    }
    if (s != "null"sv) {
    throw ParsingError("Error value"s);
    }
    return Node();
}
    
Node LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Error value"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("Error value"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return Node(std::stoi(parsed_num));
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return Node(std::stod(parsed_num));
    } catch (...) {
        throw ParsingError("Error value"s);
    }
}    

Node LoadBool (istream& input) {
std::string s;
    while (std::isalpha(input.peek())) {
        s.push_back(static_cast<char>(input.get()));
    }
    if (s != "true"sv && s != "false"sv) {
        throw ParsingError("Error value");
    } 
    else if (s == "true"sv) {
        return Node(true);
    }
    return Node(false);
}
    
    
Node LoadNode(istream& input) {
    char c;
    input >> c;
    if (!input) {
        throw ParsingError("Error nothing input"s);
    }
    if(c == '{') {
        return LoadDict(input);
    }
    if (c == '[') {
        return LoadArray(input);
    } else if (c == '"') {
        return LoadString(input);
    }
    else if(c == 'n') {
        input.putback(c);
        return LoadNull(input);
    }
    else if(c == 't' || c =='f') {
         input.putback(c);
        return LoadBool(input);
    }
    input.putback(c);
    return LoadNumber(input);
} 

Node::Node() {}

Node::Node(std::nullptr_t) : value_(nullptr){
}
    
Node::Node(Array array)
    : value_(move(array)) {
}

Node::Node(Dict map)
    : value_(move(map)) {
}

Node::Node(int value)
    : value_(value) {
}

Node::Node(std::string value)
    : value_(move(value)) {
}
    
 Node::Node(bool value)
    : value_(move(value)) {
}
    
Node::Node(double value)
    : value_(move(value)) {
}    

Document::Document(Node root)
    : root_(move(root)) {
}
    
const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

struct VisitorPrint {
    ostream& out;
        void operator()(std::nullptr_t) const {
        out << "null";    
    }
    
    void operator()([[maybe_unused]] Array arr_) const {
    out << "[\n"sv;
    bool first = true;
    for (const Node& val : arr_) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        visit(VisitorPrint{out},Node(val).GetValue());
    }
    out.put('\n');
    out.put(']');
    }
    
    void operator()([[maybe_unused]] Dict dict_) const {
    out << "{\n"sv;
    bool first = true;
    for (const auto& [key, node] : dict_) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        visit(VisitorPrint{out}, Node(key).GetValue());
        out << ": "sv;
        visit(VisitorPrint{out}, node.GetValue());
    }
    out.put('\n');
    out.put('}');
    }
    
    void operator()([[maybe_unused]] bool bool_) const {
        if(bool_ == true) {
        out << "true";
        }
        else {
            out << "false";
        }
    }
    
    void operator()([[maybe_unused]] int int_) const {
        out << int_;   
    }
    
    void operator()([[maybe_unused]] double double_) const {
        out << double_;    
    }
     void operator()([[maybe_unused]] std::string string_) const {
    out << '"';
    for (const auto lit : string_) {
         switch (lit) {
            case '\r':
                out << "\\r"sv;
                break;
            case '\n':
                out << "\\n"sv;
                break;
            case '"':
                [[fallthrough]];
            case '\\':
                out.put('\\');
                [[fallthrough]];
            default:
                out.put(lit);
                break;
        }  
    }
    out << '"';
    }
};
    
void Print(const Document& doc, std::ostream& output) {
    visit(VisitorPrint{output}, (doc.GetRoot()).GetValue());
}
    
bool Document::operator == (const Document& doc) const {
    return GetRoot() == doc.GetRoot();
}
bool Document::operator != (const Document& doc) const {
    return GetRoot() != doc.GetRoot();
}    
}
