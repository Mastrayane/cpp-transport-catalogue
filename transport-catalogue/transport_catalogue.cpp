#include "transport_catalogue.h"

#include <algorithm>

using namespace std;

namespace transport_catalogue {

	void TransportCatalogue::AddStop(string_view name, transport_catalogue::geo::Coordinates&& coordinates) {
		stops_.push_back({ { name.begin(), name.end() }, coordinates.lat, coordinates.lng });
		stop_by_name_[stops_.back().name] = &stops_.back();
	}

	void TransportCatalogue::AddRoute(string_view number, RouteType type, const std::vector<std::string_view>& stops) {
		Bus result;
		result.number = { number.begin(), number.end() };

		for (auto& stop : stops) {
			auto found_stop = GetStop(stop);

			if (found_stop != nullptr) {
				result.stops.push_back(found_stop);
			}
		}
		if (type == RouteType::Direct) {
			auto temp = result.stops;

			for (int i = temp.size() - 2; i >= 0; --i) {
				result.stops.push_back(result.stops[i]);
			}
		}

		buses_.push_back(move(result));
		bus_by_name_[buses_.back().number] = &buses_.back();

		for (auto& stop : stops) {
			auto found_stop = GetStop(stop);

			if (found_stop != nullptr) {
				bus_by_stop_[found_stop].insert(buses_.back().number);
			}
		}
	}

	BusPtr TransportCatalogue::GetRoute(string_view name) {
		if (bus_by_name_.count(name)) {
			return bus_by_name_.at(name);
		}
		else {
			return nullptr;
		}
	}

	StopPtr TransportCatalogue::GetStop(string_view name) {
		if (stop_by_name_.count(name)) {
			return stop_by_name_.at(name);
		}
		else {
			return nullptr;
		}
	}

	set<string_view> TransportCatalogue::GetBuses(string_view stop) {
		if (auto found_stop = GetStop(stop); found_stop != nullptr) {
			if (bus_by_stop_.count(found_stop)) {
				return bus_by_stop_.at(found_stop);
			}
		}
		return {};
	}

	void TransportCatalogue::SetStopDistance(std::string_view stop_from, uint64_t dist, std::string_view stop_to) {
		auto p_stop_from = GetStop(stop_from);
		auto p_stop_to = GetStop(stop_to);

		if (p_stop_from != nullptr && p_stop_to != nullptr) {
			di_to_stop[{p_stop_from, p_stop_to}] = dist;
		}
	}

	int TransportCatalogue::GetStopDistance(StopPtr p_stop_from, StopPtr p_stop_to) {
		if (p_stop_from != nullptr && p_stop_to != nullptr) {
			if (di_to_stop.count({ p_stop_from, p_stop_to })) {
				return di_to_stop.at({ p_stop_from, p_stop_to });
			}
			else
			{
				if (di_to_stop.count({ p_stop_to, p_stop_from })) {
					return di_to_stop.at({ p_stop_to, p_stop_from });
				}
			}
		}
		return 0;
	}

	BusStat TransportCatalogue::GetStatistics(BusPtr bus) {
		BusStat statistics;

		statistics.number_of_stops = bus->stops.size();

		auto temp = bus->stops;
		sort(temp.begin(), temp.end());
		auto it = unique(temp.begin(), temp.end());
		temp.resize(distance(temp.begin(), it));
		statistics.unique_stops = temp.size();

		for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
			statistics.distance += ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
			statistics.real_distance += GetStopDistance(bus->stops[i], bus->stops[i + 1]);
		}
		statistics.curvature = statistics.real_distance / statistics.distance;

		return statistics;
	}

}//namespace transport_catalogue
