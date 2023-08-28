#include "request_handler.h"

namespace transport_catalog {
    
  json::Node PrintMap(json::Dict request_map, MapRender map,TransportCatalogue& cata) {
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map_s = map.CreateSVG(cata.GetBus());
    map_s.Render(strm);
    result["map"] = strm.str();
    return json::Node{ result };
}
    
  json::Node PrintBus(json::Dict map,TransportCatalogue& cata) {
    json::Dict result;
    std::string bus = map.at("name").AsString();
    result["request_id"] = map.at("id").AsInt();
    if (cata.FindBus(bus)->name == "not found") {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = cata.FindBus(bus)->cur;
        result["route_length"] = cata.FindBus(bus)->dist_fact;
        result["stop_count"] = static_cast<int>(cata.FindBus(bus)->stops.size());
        result["unique_stop_count"] = static_cast<int>(cata.FindBus(bus)->unique.size());
    }
    return json::Node{ result };
}
    
 json::Node PrintStop(json::Dict map,TransportCatalogue& cata) {
    json::Dict result;
    std::string stop_name = map.at("name").AsString();
    result["request_id"] = map.at("id").AsInt();
    if (cata.FindStop(stop_name)->name == "not found") {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
     else {
         json::Array buses;
         for(auto i : cata.FindStop(stop_name)->buses) {
             buses.push_back(i);
         }
         result["buses"] = buses;
     }
    return json::Node{result};
}
    
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
    
    void ReadandRequests(TransportCatalogue& cata, std::istream& input) {
    json::Document doc(json::Load(input));
    reader::InputReader(cata, doc);
    auto dict = (doc.GetRoot().AsMap()).at("render_settings");
    MapRender map = CreateMapRender(dict.AsMap());
     Requests(cata,doc,map);   
}
}
