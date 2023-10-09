#include "transport_router.h"

namespace transport {

graph::DirectedWeightedGraph<double>& Router::BuildGraph(transport_catalog::TransportCatalogue& cata) {
	const auto& all_stops = cata.GetStopsMap();
	const auto& all_buses = cata.GetBus();
	graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
    std::map<std::string, graph::VertexId> stop_ids;
    graph::VertexId vertex_id = 0;

    for (const auto& [stop_name, stop_info] : all_stops) {
        stop_ids[stop_info->name] = vertex_id;
        stops_graph.AddEdge({
                stop_info->name,
                0,
                vertex_id,
                ++vertex_id,
                static_cast<double>(bus_wait_time_)
            });
        ++vertex_id;
    }
    stop_ids_ = std::move(stop_ids);

    for_each(
        all_buses.begin(),
        all_buses.end(),
        [&stops_graph, this, &cata](const auto& item) {
            const auto& bus_info = item.second;
            const auto& stops = bus_info->stops;
            size_t stops_count = stops.size();
            for (size_t i = 0; i < stops_count; ++i) {
                for (size_t j = i + 1; j < stops_count; ++j) {
                    transport_catalog::road_objects::Stop* stop_from = stops[i];
                    transport_catalog::road_objects::Stop* stop_to = stops[j];
                    int dist_sum = 0;
                    int dist_sum_inverse = 0;
                    for (size_t k = i + 1; k <= j; ++k) {
                        dist_sum += cata.GetDistance((stops[k - 1])->name, (stops[k])->name);
                        dist_sum_inverse += cata.GetDistance((stops[k - 1])->name, (stops[k])->name);
                    }
                    stops_graph.AddEdge({ bus_info->name,
                                          j - i,
                                          stop_ids_.at(stop_from->name) + 1,
                                          stop_ids_.at(stop_to->name),
                                          static_cast<double>(dist_sum) / (bus_velocity_ * (100.0 / 6.0))});
                }
            }
        });

    graph_ = std::move(stops_graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);

    return graph_;
}

std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(std::string& stop_from,std::string& stop_to) {
	return router_->BuildRoute(stop_ids_.at(std::string(stop_from)),stop_ids_.at(std::string(stop_to)));
}

graph::DirectedWeightedGraph<double>& Router::GetGraph() {
	return graph_;
}

}