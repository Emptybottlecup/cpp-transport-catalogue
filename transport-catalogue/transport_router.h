#pragma once

#include "transport_catalogue.h"
#include "router.h"
#include <memory>


	struct Parametrs {
		double wait_time_ = 0;
		double velocity_ = 0;
	};

	class TransportRouter {
	public:

		TransportRouter() = default;

		TransportRouter(Parametrs& param);
        
        void CreateGraph(transport_catalog::TransportCatalogue& catalogue_);

		std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_name_from, const std::string_view stop_name_to);

	    const graph::Edge<double>& GetEdgeFromGraph(size_t id) const;
        
		std::unique_ptr<graph::Router<double>>& GetRouter();

		Parametrs& GetParametrs();

		std::string_view GetStopId(size_t id);
        
        void CreateMaps(transport_catalog::TransportCatalogue& catalog);
        
        void FillGraph(transport_catalog::TransportCatalogue& catalog);
	private:
		Parametrs param_;
		std::map<std::string_view, uint32_t> stop_id_;
		std::unordered_map<uint32_t, std::string_view> id_stop_;
		std::unique_ptr<graph::Router<double>> router_ = nullptr;
		graph::DirectedWeightedGraph<double> graph_;
	};