#include "request_handler.h"

namespace transport_catalog {

void Requests(TransportCatalogue& cata, json::Document& doc,MapRender map_) {
    json::Array output;
    json::Array bus_and_stop = ((doc.GetRoot().AsMap()).at("stat_requests")).AsArray();
    for (auto request : bus_and_stop) {
        json::Dict map = request.AsMap();
        auto type = map.at("type").AsString();
        if (type == "Stop") {
            output.push_back(PrintStop(map,cata).AsMap());
        }
        if (type == "Bus") {
            output.push_back(PrintBus(map,cata).AsMap());
        }
        if(type == "Map") {
            output.push_back(PrintMap(map, map_,cata).AsMap());
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
    auto dict = (doc.GetRoot().AsMap()).at("render_settings");
    MapRender map = CreateMapRender(dict.AsMap());
     Requests(cata,doc,map);   
}
}
