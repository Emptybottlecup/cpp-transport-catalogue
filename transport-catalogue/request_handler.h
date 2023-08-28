#pragma once
#include "json_reader.h"
#include "map_renderer.h"
namespace transport_catalog {
    
json::Node PrintBus(json::Dict map,TransportCatalogue& cata);
    
json::Node PrintStop(json::Dict map,TransportCatalogue& cata);
    
void Requests(TransportCatalogue& cata, json::Document& doc,MapRender map_);

void ReadandRequests(TransportCatalogue& cata, std::istream& input);

json::Node PrintMap(json::Dict request_map, MapRender map, TransportCatalogue& cata);
 
}
