#pragma once
#include "transport_catalogue.h"

namespace transport_catalog {
    namespace reader {
void input_reader(TransportCatalogue& catalog, std::istream& input);

std::string Split(const std::string& text);

std::string Check(const std::string& text);

std::vector<std::pair<std::string,double>> Split_dist(std::string text);

transport_catalog::road_objects::Stop Split_stop_str(const std::string& stop);
        
transport_catalog::road_objects::Route Split_route_str(const std::string& route,TransportCatalogue& catalog);        
    }
}