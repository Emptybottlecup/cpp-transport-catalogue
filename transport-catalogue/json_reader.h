#pragma once 
#include "transport_catalogue.h" 
#include "json.h" 
#include "map_renderer.h" 
#include "transport_router.h"

namespace transport_catalog { 
    namespace reader { 
        void InputReader(TransportCatalogue& cata, json::Document& base_and_stat); 
    } 
json::Node PrintBus(json::Dict map,TransportCatalogue& cata); 
     
json::Node PrintStop(json::Dict map,TransportCatalogue& cata); 
 
json::Node PrintMap(json::Dict request_map, MapRender map, TransportCatalogue& cata); 

json::Node PrintRouting(json::Dict map,transport::Router& router);  
}