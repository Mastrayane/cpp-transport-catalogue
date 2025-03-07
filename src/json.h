#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

	class Node;
	using Dict = std::map<std::string, Node>;
	using Array = std::vector<Node>;

	class ParsingError : public std::runtime_error {
	public:
		using runtime_error::runtime_error;
	};

	class Node final
		: private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
	public:
		using variant::variant;
		using Value = variant;

		bool IsInt() const;
		bool IsDouble() const;
		bool IsPureDouble() const;
		bool IsBool() const;
		bool IsString() const;
		bool IsNull() const;
		bool IsArray() const;
		bool IsDict() const;

		int AsInt() const;
		bool AsBool() const;
		double AsDouble() const;
		const std::string& AsString() const;
		const Array& AsArray() const;
		const Dict& AsDict() const;

		bool operator==(const Node& rhs) const;

		const Value& GetValue() const;

		Value& GetValue();
	};

	inline bool operator!=(const Node& lhs, const Node& rhs);

	class Document {
	public:
		explicit Document(Node root)
			: root_(std::move(root)) {
		}

		const Node& GetRoot() const {
			return root_;
		}

	private:
		Node root_;
	};

	inline bool operator==(const Document& lhs, const Document& rhs);
	inline bool operator!=(const Document& lhs, const Document& rhs);

	Document Load(std::istream& input);

	void Print(const Document& doc, std::ostream& output);

} // namespace json