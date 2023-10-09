#include "json_reader.h" 
#include "json_builder.h" 
 
namespace transport_catalog { 
    namespace reader { 
        void InputReader(TransportCatalogue& catalog, json::Document& base_and_stat ) { 
            std::map<std::string,std::vector<std::string>> buses; 
            std::map<std::string,bool> bus_bool; 
            auto bus_stop = (base_and_stat.GetRoot().AsDict().at("base_requests")).AsArray(); 
            for (auto node : bus_stop) { 
                auto bus_or_stop = node.AsDict(); 
                std::string type = bus_or_stop.at("type").AsString(); 
                if (type == "Stop") { 
                    catalog.AddStop(bus_or_stop.at("name").AsString(),{bus_or_stop.at("latitude").AsDouble(),bus_or_stop.at("longitude").AsDouble()}); 
                for(auto [name,node] : bus_or_stop.at("road_distances").AsDict()) {                   catalog.SetDistance(bus_or_stop.at("name").AsString(),name,node.AsInt());    
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
  json::Node result; 
  int id = request_map.at("id").AsInt(); 
  std::ostringstream strm; 
  svg::Document map_s = map.CreateSVG(catalog.GetBus()); 
  map_s.Render(strm); 
  result = json::Builder{} 
                .StartDict() 
                    .Key("request_id").Value(id) 
                    .Key("map").Value(strm.str()) 
                .EndDict() 
            .Build(); 
  return result; 
} 
   
json::Node PrintBus(json::Dict map,TransportCatalogue& catalog) { 
  json::Node result; 
  std::string bus = map.at("name").AsString(); 
  int id = map.at("id").AsInt(); 
  if (catalog.FindBus(bus)->name == "not found") { 
      result = json::Builder{} 
                    .StartDict() 
                        .Key("request_id").Value(id) 
                        .Key("error_message").Value("not found") 
                    .EndDict() 
                .Build(); 
  } 
  else { 
       
      result = json::Builder{} 
                    .StartDict() 
                        .Key("request_id").Value(id) 
                        .Key("curvature").Value(catalog.FindBus(bus)->cur) 
                        .Key("route_length").Value(catalog.FindBus(bus)->dist_fact) 
                        .Key("stop_count").Value(static_cast<int>(catalog.FindBus(bus)->stops.size())) 
                        .Key("unique_stop_count").Value(static_cast<int>(catalog.FindBus(bus)->unique.size())) 
                    .EndDict() 
                .Build(); 
  } 
  return result; 
} 
   
json::Node PrintStop(json::Dict map,TransportCatalogue& catalog) { 
  json::Node result; 
  std::string stop_name = map.at("name").AsString(); 
  int id = map.at("id").AsInt(); 
  if (catalog.FindStop(stop_name)->name == "not found") { 
      result = json::Builder{} 
                    .StartDict() 
                        .Key("request_id").Value(id) 
                        .Key("error_message").Value("not found") 
                    .EndDict() 
                .Build(); 
  } 
   else { 
       json::Array buses; 
       for(auto i : catalog.FindStop(stop_name)->buses) { 
           buses.push_back(i); 
       } 
        result = json::Builder{} 
                    .StartDict() 
                        .Key("request_id").Value(id) 
                        .Key("buses").Value(buses) 
                    .EndDict() 
                .Build(); 
   } 
  return result; 
}
    
json::Node PrintRouting(json::Dict map,transport::Router& router) {
    json::Node result;
int id = map.at("id").AsInt();
std::string stop_from = map.at("from").AsString();
std::string stop_to = map.at("to").AsString();
auto routing = router.FindRoute(stop_from, stop_to);
if (!routing) {
    result = json::Builder{}
        .StartDict()
            .Key("request_id").Value(id)
            .Key("error_message").Value("not found")
        .EndDict()
    .Build();
}
else {
    json::Array items;
    double total_time = 0.0;
    items.reserve(routing.value().edges.size());
    for (auto& edge_id : routing.value().edges) {
        const graph::Edge<double> edge = router.GetGraph().GetEdge(edge_id);
        if (edge.quality == 0) {
            items.emplace_back(json::Node(json::Builder{}
                .StartDict()
                    .Key("stop_name").Value(edge.name)
                    .Key("time").Value(edge.weight)
                    .Key("type").Value("Wait")
                .EndDict()
            .Build()));
            total_time += edge.weight;
        }
        else {
            items.emplace_back(json::Node(json::Builder{}
                .StartDict()
                    .Key("bus").Value(edge.name)
                    .Key("span_count").Value(static_cast<int>(edge.quality))
                    .Key("time").Value(edge.weight)
                    .Key("type").Value("Bus")
                .EndDict()
            .Build()));

            total_time += edge.weight;
        }
    }

    result = json::Builder{}
        .StartDict()
            .Key("request_id").Value(id)
            .Key("total_time").Value(total_time)
            .Key("items").Value(items)
        .EndDict()
    .Build();
}

return result;
}
    
}