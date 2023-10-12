#include "transport_router.h"

	TransportRouter::TransportRouter(Parametrs& param) : param_(param) {}

	void TransportRouter::CreateGraph(transport_catalog::TransportCatalogue& cata) {
        size_t stop_id = 0;
		auto stops = cata.GetStopsMap();
		for (auto stop : stops) {
			stop_id_.insert({ stop.first, stop_id });
			id_stop_.insert({ stop_id, stop.first });
            ++stop_id;
		}
        graph_ = std::move(graph::DirectedWeightedGraph<double>{stop_id_.size()});
		for (auto [bus_name, route_stats] : cata.GetBus()) {
			auto stops_in_bus = route_stats->stops;
			for (int i = 0; i < stops_in_bus.size() - 1; ++i) {
			    double route_time = param_.wait_time_;
			    auto from = stops_in_bus[i];
			    int span_count_ = 1;
			    for (int j = i + 1; j < stops_in_bus.size(); ++j) {
				    auto to = stops_in_bus[j];
                    auto distance = cata.GetDistance(stops_in_bus[j - 1]->name,to->name);
				    route_time += (distance/(param_.velocity_ * 1000)) * 60;
				    graph_.AddEdge({ stop_id_[from->name], stop_id_[to->name], route_time,cata.GetBus().at(bus_name)->name,span_count_});
                    ++span_count_;
			    }
		    }
		}
		router_ = std::make_unique<graph::Router<double>>(graph::Router(graph_));
	}

	std::optional <graph::Router<double>::RouteInfo> TransportRouter::FindRoute(const std::string_view stop_name_from, const std::string_view stop_name_to) {
        if(!router_) {
	        return std::nullopt;
		}
		return router_->BuildRoute(stop_id_.at(stop_name_from), stop_id_.at(stop_name_to));
	}

	graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() {
		return graph_;
	}

	Parametrs& TransportRouter::GetParametrs() {
		return param_;
	}

	 std::string_view TransportRouter::GetStopId(size_t id) {
		return id_stop_.at(id);
	}