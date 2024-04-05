#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>



#include "input_reader.h"

bool operator==(const CommandDescription& lhs, const CommandDescription& rhs) {
	return lhs.command == rhs.command && lhs.id == rhs.id && lhs.description == rhs.description;
}



/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
transport_catalogue::geo::Coordinates ParseCoordinates(std::string_view str) {
	static const double nan = std::nan("");

	auto not_space = str.find_first_not_of(' ');
	auto comma = str.find(',');

	if (comma == str.npos) {
		return { nan, nan };
	}

	auto not_space2 = str.find_first_not_of(' ', comma + 1);

	double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
	double lng = std::stod(std::string(str.substr(not_space2)));

	return { lat, lng };
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
	const auto start = string.find_first_not_of(' ');
	if (start == string.npos) {
		return {};
	}
	return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
	std::vector<std::string_view> result;

	size_t pos = 0;
	while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
		auto delim_pos = string.find(delim, pos);
		if (delim_pos == string.npos) {
			delim_pos = string.size();
		}
		if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
			result.push_back(substr);
		}
		pos = delim_pos + 1;
	}

	return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
	if (route.find('>') != route.npos) {
		return Split(route, '>');
	}

	auto stops = Split(route, '-');
	std::vector<std::string_view> results(stops.begin(), stops.end());
	results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

	return results;
}

transport_catalogue::RouteType ParseRouteType(std::string_view route) {

	if (route.find('>') != route.npos) {
		return transport_catalogue::RouteType::Round;
	}
	else {
		return transport_catalogue::RouteType::Direct;
	}
}

CommandDescription ParseCommandDescription(std::string_view line) {
	auto colon_pos = line.find(':');
	if (colon_pos == line.npos) {
		return {};
	}

	auto space_pos = line.find(' ');
	if (space_pos >= colon_pos) {
		return {};
	}

	auto not_space = line.find_first_not_of(' ', space_pos);
	if (not_space >= colon_pos) {
		return {};
	}

	return { std::string(line.substr(0, space_pos)),
			std::string(line.substr(not_space, colon_pos - not_space)),
			std::string(line.substr(colon_pos + 1)) };
}

void InputReader::ReadingStream(transport_catalogue::TransportCatalogue& catalogue, InputReader& reader, int const& base_request_count)
{
	for (int i = 0; i < base_request_count; ++i) {
		std::string line;
		getline(std::cin, line);
		reader.ParseLine(line);
	}
	reader.ApplyCommands(catalogue);
}


void InputReader::ReadingStream(transport_catalogue::TransportCatalogue& catalogue, InputReader& reader, std::istream& input)
{
	int base_request_count;
	input >> base_request_count >> std::ws;
	for (int i = 0; i < base_request_count; ++i) {
		std::string line;
		getline(input, line);
		reader.ParseLine(line);
	}
	reader.ApplyCommands(catalogue);
}


void InputReader::ParseLine(std::string_view line) {
	auto command_description = ParseCommandDescription(line);

	if (command_description) {
		commands_.push_back(std::move(command_description));
	}
}



std::vector<CommandDescription> InputReader::GetCommands() {
	return commands_;
}

void InputReader::ApplyCommands([[maybe_unused]] transport_catalogue::TransportCatalogue& catalogue) const {

	for (auto& com : commands_) {
		if (com.command == "Stop") {
			transport_catalogue::geo::Coordinates G = ParseCoordinates(com.description);
			catalogue.AddStop(com.id, G.lat, G.lng);
		}
		else {
			std::vector<std::string_view> stops = ParseRoute(com.description);
			catalogue.AddRoute(com.id, stops);
		}
	}
}

