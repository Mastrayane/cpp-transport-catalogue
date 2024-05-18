#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace transport_catalogue {
    namespace detail {
        namespace json {

            class Node;

            using Dict = std::map<std::string, Node>;
            using Array = std::vector<Node>;

            class ParsingError : public std::runtime_error {
            public:
                using runtime_error::runtime_error;
            };

            class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
            public:

                using variant::variant;
                using Value = variant;

                bool IsNull() const;
                bool IsInt() const;
                bool IsDouble() const;
                bool IsPureDouble() const;
                bool IsBool() const;
                bool IsString() const;
                bool IsArray() const;
                bool IsMap() const;

                const Array& AsArray() const;
                const Dict& AsMap() const;
                int AsInt() const;
                double AsDouble() const;
                bool AsBool() const;
                const std::string& AsString() const;

                const Value& GetValue() const;

            };

            bool operator==(const Node& lhs, const Node& rhs);
            bool operator!=(const Node& lhs, const Node& rhs);

            class Document {
            public:
                Document() = default;
                explicit Document(Node root);
                const Node& get_root() const;

            private:
                Node root_;
            };

            inline bool operator==(const Document& lhs, const Document& rhs) {
                return lhs.get_root() == rhs.get_root();
            }
            inline bool operator!=(const Document& lhs, const Document& rhs) {
                return !(lhs == rhs);
            }

            Document load(std::istream& input);
            void print(const Document& document, std::ostream& output);

        }//end namespace json
    }//end namespace detail
}//end namespace transport_catalogue
