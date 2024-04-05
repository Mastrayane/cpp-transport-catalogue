#pragma once

#include <iosfwd>
#include <string_view>
#include <vector>

#include "transport_catalogue.h"

namespace stats {
	struct CommandStat {
		// Определяет, задана ли команда (поле command непустое)
		explicit operator bool() const {
			return !command.empty();
		}

		bool operator!() const {
			return !operator bool();
		}

		std::string command;      // Название команды
		std::string id;           // id маршрута или остановки

	};

	class OutputDataReader {
	public:
		// Парсит строку в структуру CommandDescription и сохраняет результат в commands_
		void ParseLine(std::string_view line);

		std::vector<CommandStat> GetCommands() const;

	private:
		std::vector<CommandStat> commands_;
	};

} // конец namespace stat

void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
	std::ostream& output);

void RequestStatistics(transport_catalogue::TransportCatalogue& catalogue, std::istream& input);

void RequestStatistics(transport_catalogue::TransportCatalogue& catalogue, std::istream& input, int const& stat_request_count);
