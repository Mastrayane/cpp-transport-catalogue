#include "stat_reader.h"

#include <iomanip>
#include <iostream>
#include <set>

using namespace std;

namespace transport_catalogue {

	namespace output {

		void OutputRouteAbout(TransportCatalogue& tc, std::string_view route) {
			if (tc.GetRoute(route) == nullptr) {
				cout << "Bus "s << route << ": not found"s << endl;
			}
			else {
				BusStat stat = tc.GetStatistics(tc.GetRoute(route));

				cout << "Bus "s << route << ": "s << stat.number_of_stops
					<< " stops on route, "s << stat.unique_stops
					<< " unique stops, "s << std::setprecision(6)
					<< stat.real_distance << " route length, "s
					<< std::setprecision(6) << stat.curvature
					<< " curvature"s << endl;
			}
		}

		void OutputStopAbout(TransportCatalogue& tc, string_view name) {
			bool flag = tc.GetStop(name) != nullptr;
			set<string_view> buses = tc.GetBuses(name);

			if (flag) {
				if (buses.size() == 0) {
					cout << "Stop "s << name << ": no buses"s << endl;
				}
				else {
					cout << "Stop "s << name << ": buses "s;
					for (auto it = buses.begin(); it != buses.end(); ++it) {
						if (next(it) != buses.end()) {
							cout << (*it) << " "s;
						}
						else {
							cout << (*it);
						}
					}
					cout << endl;
				}
			}
			else {
				cout << "Stop "s << name << ": not found"s << endl;
			}
		}

		void OutputAbout(TransportCatalogue& tc, query::Command com) {
			if (com.type == query::QueryType::StopX) {
				OutputStopAbout(tc, com.name);
			}

			if (com.type == query::QueryType::BusX) {
				OutputRouteAbout(tc, com.name);
			}
		}

	}//namespace output
}//namespace transport_catalogue
