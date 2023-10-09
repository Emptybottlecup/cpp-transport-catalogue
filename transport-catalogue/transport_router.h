#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport {

class Router {
public:
	Router(int wait_time, double bus_velocity,transport_catalog::TransportCatalogue& cata) {
		bus_wait_time_ = wait_time;
		bus_velocity_ = bus_velocity;
		BuildGraph(cata);
	}

	graph::DirectedWeightedGraph<double>& BuildGraph(transport_catalog::TransportCatalogue& cata);
	std::optional<graph::Router<double>::RouteInfo> FindRoute(std::string& stop_from,std::string& stop_to);
	graph::DirectedWeightedGraph<double>& GetGraph();

private:
	int bus_wait_time_ = 0;
	double bus_velocity_ = 0.0;
	graph::DirectedWeightedGraph<double> graph_;
	std::map<std::string, graph::VertexId> stop_ids_;
	std::unique_ptr<graph::Router<double>> router_;
};

}