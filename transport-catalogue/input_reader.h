#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <iosfwd>



#include "geo.h"
#include "transport_catalogue.h"

struct CommandDescription {
	// Определяет, задана ли команда (поле command непустое)
	explicit operator bool() const {
		return !command.empty();
	}

	bool operator!() const {
		return !operator bool();
	}

	std::string command;      // Название команды
	std::string id;           // id маршрута или остановки
	std::string description;  // Параметры команды

};

bool operator==(const CommandDescription& lhs, const CommandDescription& rhs);

transport_catalogue::RouteType ParseRouteType(std::string_view route);

class InputReader {
public:

	static void ReadingStream(transport_catalogue::TransportCatalogue& catalogue, InputReader& reader, int const& base_request_count);

	void ReadingStream(transport_catalogue::TransportCatalogue& catalogue, InputReader& reader, std::istream& input);

	/**
	 * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
	 */
	void ParseLine(std::string_view line);

	/**
	 * Наполняет данными транспортный справочник, используя команды из commands_
	 */

	void ApplyCommands(transport_catalogue::TransportCatalogue& catalogue) const;

	std::vector<CommandDescription> GetCommands();

private:
	std::vector<CommandDescription> commands_;
};


