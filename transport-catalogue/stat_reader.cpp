#include <string_view>
#include <iostream>
#include <algorithm>

#include "stat_reader.h"
#include "input_reader.h"


stats::CommandStat ParseCommandDesStat(std::string_view line) {

	stats::CommandStat commandStat;

	auto space_pos = line.find(' ');
	if (space_pos == line.npos) {
		return commandStat;
	}

	commandStat.command = std::string(line.substr(0, space_pos));
	commandStat.id = std::string(line.substr(space_pos + 1));

	return commandStat;

}

void stats::OutputDataReader::ParseLine(std::string_view line) {
	auto command_description = ParseCommandDesStat(line);
	if (command_description) {
		commands_.push_back(std::move(command_description));
	}
}

std::vector<stats::CommandStat> stats::OutputDataReader::GetCommands() const {
	return commands_;
};


void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
	std::ostream& output) {
	stats::OutputDataReader reader;
	reader.ParseLine(request);
	std::vector<stats::CommandStat> commands = reader.GetCommands();


	for (auto const& com : commands) {

		if (com.command == "Bus") {
			transport_catalogue::BusPtr busptr = tansport_catalogue.GetRoute(com.id);

			if (busptr == nullptr) {
				output << "Bus " << com.id << ": not found" << std::endl;
			}
			else {
				transport_catalogue::BusStat bus = tansport_catalogue.GetStatistics(busptr);
				output << "Bus " << com.id << ": " << bus.number_of_stops << " stops on route, " <<
					bus.unique_stops << " unique stops, " <<
					bus.distance << " route length" << std::endl;
			}
		}

		else if (com.command == "Stop") {
			transport_catalogue::StopPtr stopptr = tansport_catalogue.GetStop(com.id);

			if (stopptr == nullptr) {
				output << "Stop " << com.id << ": not found" << std::endl;
			}
			else {

				std::set<std::string_view> buses = tansport_catalogue.SetBusByStop(stopptr);

				if (buses.empty()) {
					output << "Stop " << com.id << ": no buses" << std::endl;
				}
				else {

					output << "Stop " << com.id << ": buses";
					for (auto const& bus : buses) {
						output << " " << bus;
					}
					output << std::endl;

				}
			}
		}
	}
}

void RequestStatistics(transport_catalogue::TransportCatalogue& catalogue, std::istream& input) {
	int stat_request_count;
	input >> stat_request_count >> std::ws;
	for (int i = 0; i < stat_request_count; ++i) {
		std::string line;
		getline(input, line);
		ParseAndPrintStat(catalogue, line, std::cout);
	}
}

void RequestStatistics(transport_catalogue::TransportCatalogue& catalogue, std::istream& input, int const& stat_request_count)
{
	for (int i = 0; i < stat_request_count; ++i) {
		std::string line;
		getline(input, line);
		ParseAndPrintStat(catalogue, line, std::cout);
	}
}
