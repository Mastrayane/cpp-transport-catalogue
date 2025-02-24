#include "transport_router.h"
#include "request_handler.h"

const double KILOMETERS_TO_METERS_PER_HOUR_CONVERSION = 100.0 / 6.0;

namespace transport {

	void Router::AddStopsToGraph(const Catalogue& catalogue, graph::DirectedWeightedGraph<double>& stops_graph) {
		const auto& all_stops = catalogue.GetSortedAllStops();
		std::map<std::string, graph::VertexId> stop_ids;
		graph::VertexId vertex_id = 0;

		for (const auto& [stop_name, stop_info] : all_stops) {
			stop_ids[stop_info->name] = vertex_id;
			stops_graph.AddEdge({
				stop_info->name,
				0,
				vertex_id,
				++vertex_id,
				static_cast<double>(settings_.bus_wait_time)
				});
			++vertex_id;
		}
		stop_ids_ = std::move(stop_ids);
	}

	void Router::AddBusesToGraph(const Catalogue& catalogue, graph::DirectedWeightedGraph<double>& stops_graph) {
		const auto& all_buses = catalogue.GetSortedAllBuses();

		for_each(
			all_buses.begin(),
			all_buses.end(),
			[&stops_graph, this, &catalogue](const auto& item) {
				const auto& bus_info = item.second;
				const auto& stops = bus_info->stops;
				size_t stops_count = stops.size();
				for (size_t i = 0; i < stops_count; ++i) {
					for (size_t j = i + 1; j < stops_count; ++j) {
						const Stop* stop_from = stops[i];
						const Stop* stop_to = stops[j];
						int dist_sum = 0;
						int dist_sum_inverse = 0;
						for (size_t k = i + 1; k <= j; ++k) {
							dist_sum += catalogue.GetDistance(stops[k - 1], stops[k]);
							dist_sum_inverse += catalogue.GetDistance(stops[k], stops[k - 1]);
						}
						stops_graph.AddEdge({ bus_info->number,
											  j - i,
											  stop_ids_.at(stop_from->name) + 1,
											  stop_ids_.at(stop_to->name),
											  static_cast<double>(dist_sum) / (settings_.bus_velocity * KILOMETERS_TO_METERS_PER_HOUR_CONVERSION) });

						if (!bus_info->is_circle) {
							stops_graph.AddEdge({ bus_info->number,
												  j - i,
												  stop_ids_.at(stop_to->name) + 1,
												  stop_ids_.at(stop_from->name),
												  static_cast<double>(dist_sum_inverse) / (settings_.bus_velocity * KILOMETERS_TO_METERS_PER_HOUR_CONVERSION) });
						}
					}
				}
			});
	}

	void Router::BuildGraph(const Catalogue& catalogue)
	{
		graph::DirectedWeightedGraph<double> stops_graph(catalogue.GetSortedAllStops().size() * 2);

		AddStopsToGraph(catalogue, stops_graph);
		AddBusesToGraph(catalogue, stops_graph);

		graph_ = std::move(stops_graph);
		router_ = std::make_unique<graph::Router<double>>(graph_);
	}

	std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
		return router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
	}
	
}