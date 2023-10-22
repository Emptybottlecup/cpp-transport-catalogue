#include "transport_router.h"

TransportRouter::TransportRouter(Parametrs& param) : param_(param) {}

void TransportRouter::CreateGraph(transport_catalog::TransportCatalogue& catalog) {
    CreateMaps(catalog);
    graph_ = graph::DirectedWeightedGraph<double>{stop_id_.size()};
    FillGraph(catalog);
    router_ = std::make_unique<graph::Router<double>>(graph::Router(graph_));
}

std::optional <graph::Router<double>::RouteInfo> TransportRouter::FindRoute(const std::string_view stop_name_from, const std::string_view stop_name_to) {
    if(!router_) {
	    return std::nullopt;
    }
	return router_->BuildRoute(stop_id_.at(stop_name_from), stop_id_.at(stop_name_to));
}

Parametrs& TransportRouter::GetParametrs() {
    return param_;
}
    
void TransportRouter::CreateMaps(transport_catalog::TransportCatalogue& catalog) {
    size_t stop_id = 0;
	auto stops = catalog.GetStopsMap();
	for (auto stop : stops) {
	    stop_id_.insert({ stop.first, stop_id });
	    id_stop_.insert({ stop_id, stop.first });
        ++stop_id;
	}    
}

void TransportRouter::FillGraph(transport_catalog::TransportCatalogue& catalog) {
    for (auto [bus_name, route_stats] : catalog.GetBus()) {
        auto stops_in_bus = route_stats->stops;
		for (int i = 1; i < stops_in_bus.size(); ++i) {
	        double route_time = param_.wait_time_;
			auto from = stops_in_bus[i-1];
			int span_count_ = 1;
			for (int j = i; j < stops_in_bus.size(); ++j) {
		        auto to = stops_in_bus[j];
                auto distance = catalog.GetDistance(stops_in_bus[j - 1]->name,to->name);
				route_time += (distance/(param_.velocity_ * 1000)) * 60;
				graph_.AddEdge({ stop_id_[from->name], stop_id_[to->name], route_time,catalog.GetBus().at(bus_name)->name,span_count_});
                ++span_count_;
		    }
	    }
    }
}

const graph::Edge<double>& TransportRouter::GetEdgeFromGraph(size_t id) const {
    return graph_.GetEdge(id);
}

std::string_view TransportRouter::GetStopId(size_t id) {
    return id_stop_.at(id);
}

void TransportRouter::SetGraph(graph::DirectedWeightedGraph<double> graph, std::map<std::string, uint32_t> stop_id) {
    graph_ = graph;
    for(auto i : stop_id) {
    names.push_back(i.first);
    stop_id_[names.back()] = static_cast<uint32_t>(i.second);
    id_stop_[i.second] = names.back();
    }
    router_ = std::make_unique<graph::Router<double>>(graph_);
}

std::map<std::string_view, uint32_t> TransportRouter::GetStopIds() {
    return stop_id_;
}

graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() {
    return graph_;
}