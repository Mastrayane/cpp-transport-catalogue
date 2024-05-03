#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

using namespace std::string_view_literals;

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


	/*

	ПОПРОБОВАЛ НЕСКОЛЬКО ВАРИАНТОВ. К СОЖАЛЕНИЮ ВСЕ НЕ РАБОЧИЕ.
	Не понимаю как это реализовать(((

	Реализовать класс-родитель Value с полем Value value_
	и от него наследовать class Node?
	Только чем это оптимимзирует код?

	Направьте меня пожалуйста. Совет очень интересный,
	хочу научиться его реализовывать.
	_______________________________________

	using  = std::variant<std::nullptr_t, std::string, int, double, bool, Array, Dict>;

	class Node : public Value {
	public:
		using Value::variant; // Inherit constructors

		// Type checking methods
		bool IsInt() const { return this->index() == 2; }
		bool IsDouble() const { return this->index() == 3; }
		bool IsPureDouble() const { return std::holds_alternative<double>(*this); }
		bool IsBool() const { return this->index() == 4; }
		bool IsString() const { return this->index() == 1; }
		bool IsNull() const { return this->index() == 0; }
		bool IsArray() const { return this->index() == 6; }
		bool IsMap() const { return this->index() == 5; }

		// Value extraction methods
		int AsInt() const { return std::get<int>(*this); }
		bool AsBool() const { return std::get<bool>(*this); }
		double AsDouble() const { return std::get<double>(*this); }
		const std::string& AsString() const { return std::get<std::string>(*this); }
		const Array& AsArray() const { return std::get<Array>(*this); }
		const Dict& AsMap() const { return std::get<Dict>(*this); }

		// Access to the variant value
		const Value& GetValue() const { return *this; }

		// Comparison operators
		bool operator==(const Node& rhs) const { return static_cast<const Value&>(*this) == static_cast<const Value&>(rhs); }
		bool operator!=(const Node& rhs) const { return !(*this == rhs); }
	};
	*/




	class Node {
	public:
		using Value = std::variant<std::nullptr_t, std::string, int, double, bool, Array, Dict>;

		template<typename ValueType>
		Node(ValueType value)
			: value_(value)
		{}

		Node()
			: value_(nullptr)
		{}

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

		const Value& GetValue() const;

		bool operator==(const Node& rhs) const;
		bool operator!=(const Node& rhs) const;

	private:
		Value value_;
	};


	class Document {
	public:
		explicit Document(Node root);

		const Node& GetRoot() const;

		bool operator==(const Document& rhs) const;
		bool operator!=(const Document& rhs) const;

	private:
		Node root_;
	};

	Document Load(std::istream& input);

	// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
	struct PrintContext {
		std::ostream& out;
		int indent_step = 4;
		int indent = 0;

		void PrintIndent() const {
			for (int i = 0; i < indent; ++i) {
				out.put(' ');
			}
		}
		// Возвращает новый контекст вывода с увеличенным смещением
		PrintContext Indented() const {
			return { out, indent_step, indent_step + indent };
		}
	};

	void PrintValue(std::nullptr_t, const PrintContext& ctx);
	void PrintValue(std::string value, const PrintContext& ctx);
	void PrintValue(bool value, const PrintContext& ctx);
	void PrintValue(Array array, const PrintContext& ctx);
	void PrintValue(Dict dict, const PrintContext& ctx);
	// Шаблон, подходящий для вывода double и int
	template <typename Value>
	void PrintValue(const Value& value, const PrintContext& ctx) {
		ctx.out << value;
	}
	void PrintNode(const Node& node, const PrintContext& ctx);
	void Print(const Document& doc, std::ostream& output);

}  // namespace json
