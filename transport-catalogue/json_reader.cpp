#include "json_reader.h"

namespace transport_catalog {
    namespace reader {
        void InputReader(TransportCatalogue& catalog, json::Document& base_and_stat ) {
            std::map<std::string,std::vector<std::string>> buses;
            std::map<std::string,bool> bus_bool;
            auto bus_stop = (base_and_stat.GetRoot().AsMap().at("base_requests")).AsArray();
            for (auto node : bus_stop) {
                auto bus_or_stop = node.AsMap();
                std::string type = bus_or_stop.at("type").AsString();
                if (type == "Stop") {
                    catalog.AddStop(bus_or_stop.at("name").AsString(),{bus_or_stop.at("latitude").AsDouble(),bus_or_stop.at("longitude").AsDouble()});
                for(auto [name,node] : bus_or_stop.at("road_distances").AsMap()) {                   catalog.SetDistance(bus_or_stop.at("name").AsString(),name,node.AsInt());   
                }
                }
                if (type == "Bus") {
                    std::string name = bus_or_stop.at("name").AsString();
                    bus_bool[name] = bus_or_stop.at("is_roundtrip").AsBool();
                    for(auto i : bus_or_stop.at("stops").AsArray()) {
                        (buses[name]).push_back(i.AsString());   
                    }
                }
            }
            for(const auto& [name,stops_] : buses) {
                 std::vector<road_objects::Stop*> stops;;
                 for (auto stop : stops_) {
                     stops.push_back(catalog.FindStop(stop));
                     catalog.FindStop(stop)->buses.insert(name);
                 }
                 catalog.AddBus(name, stops, bus_bool.at(name));
            }
         }
    }
json::Node PrintMap(json::Dict request_map, MapRender map,TransportCatalogue& catalog) {
  json::Dict result;
  result["request_id"] = request_map.at("id").AsInt();
  std::ostringstream strm;
  svg::Document map_s = map.CreateSVG(catalog.GetBus());
  map_s.Render(strm);
  result["map"] = strm.str();
  return json::Node{ result };
}
  
json::Node PrintBus(json::Dict map,TransportCatalogue& catalog) {
  json::Dict result;
  std::string bus = map.at("name").AsString();
  result["request_id"] = map.at("id").AsInt();
  if (catalog.FindBus(bus)->name == "not found") {
      result["error_message"] = json::Node{ static_cast<std::string>("not found") };
  }
  else {
      result["curvature"] = catalog.FindBus(bus)->cur;
      result["route_length"] = catalog.FindBus(bus)->dist_fact;
      result["stop_count"] = static_cast<int>(catalog.FindBus(bus)->stops.size());
      result["unique_stop_count"] = static_cast<int>(catalog.FindBus(bus)->unique.size());
  }
  return json::Node{ result };
}
  
json::Node PrintStop(json::Dict map,TransportCatalogue& catalog) {
  json::Dict result;
  std::string stop_name = map.at("name").AsString();
  result["request_id"] = map.at("id").AsInt();
  if (catalog.FindStop(stop_name)->name == "not found") {
      result["error_message"] = json::Node{ static_cast<std::string>("not found") };
  }
   else {
       json::Array buses;
       for(auto i : catalog.FindStop(stop_name)->buses) {
           buses.push_back(i);
       }
       result["buses"] = buses;
   }
  return json::Node{result};
}
}
