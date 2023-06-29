#pragma once
#include "transport_catalogue.h"

namespace transport_catalog {
    namespace reader {
void InputReader(TransportCatalogue& catalog, std::istream& input);

std::string Split(const std::string& text);

std::string GetObjectType(const std::string& text);

std::vector<std::pair<std::string,double>> SplitDist(std::string text);

transport_catalog::road_objects::Stop SplitStopStr(const std::string& stop);
        
transport_catalog::road_objects::Route SplitRouteStr(const std::string& route,TransportCatalogue& catalog);        
    }
}