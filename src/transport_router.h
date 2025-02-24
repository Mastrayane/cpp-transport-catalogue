#pragma once


#include "router.h"
#include "transport_catalogue.h"

#include <memory>
#include "graph.h"

namespace transport {

	struct RouterSettings {
		int bus_wait_time = 0;
		double bus_velocity = 0.0;
	};

	class Router {
	public:

		explicit Router(const RouterSettings& settings, const Catalogue& catalogue) {
			settings_ = settings;
			BuildGraph(catalogue);
		}

		std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;

	private:
		RouterSettings settings_;

		graph::DirectedWeightedGraph<double> graph_;
		std::map<std::string, graph::VertexId> stop_ids_;
		std::unique_ptr<graph::Router<double>> router_;

		void AddStopsToGraph(const Catalogue& catalogue, graph::DirectedWeightedGraph<double>& stops_graph);
		void AddBusesToGraph(const Catalogue& catalogue, graph::DirectedWeightedGraph<double>& stops_graph);

		void BuildGraph(const Catalogue& catalogue);

	};

}