#include "request_handler.h" 
 
namespace transport_catalog { 
 
void Requests(TransportCatalogue& cata, json::Document& doc,MapRender map_, transport::Router& router) { 
    json::Array output; 
    json::Array bus_and_stop = ((doc.GetRoot().AsDict()).at("stat_requests")).AsArray(); 
    for (auto request : bus_and_stop) { 
        json::Dict map = request.AsDict(); 
        auto type = map.at("type").AsString(); 
        if (type == "Stop") { 
            output.push_back(PrintStop(map,cata).AsDict()); 
        } 
        if (type == "Bus") { 
            output.push_back(PrintBus(map,cata).AsDict()); 
        } 
        if(type == "Map") { 
            output.push_back(PrintMap(map, map_,cata).AsDict()); 
        }
        if (type == "Route") {
            output.push_back(PrintRouting(map, router).AsDict());
        }
    } 
    json::Print(json::Document{output}, std::cout); 
} 
 svg::Document RenderCatalog(TransportCatalogue& cata, MapRender map) { 
    return map.CreateSVG(cata.GetBus()); 
} 
     
    void ReadRequests(TransportCatalogue& cata, std::istream& input) { 
    json::Document doc(json::Load(input)); 
    reader::InputReader(cata, doc); 
    auto dict = (doc.GetRoot().AsDict()).at("render_settings");
    auto rout_settings = (doc.GetRoot().AsDict()).at("routing_settings");
    MapRender map = CreateMapRender(dict.AsDict());
    transport::Router router(rout_settings.AsDict().at("bus_wait_time").AsInt(), rout_settings.AsDict().at("bus_velocity").AsDouble(), cata);
    Requests(cata,doc,map,router);    
} 
}