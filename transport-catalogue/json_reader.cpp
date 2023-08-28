#include "json_reader.h"

namespace transport_catalog {
    namespace reader {
        void InputReader(TransportCatalogue& cata, json::Document& base_and_stat ) {
            std::map<std::string,std::vector<std::string>> buses;
            std::map<std::string,bool> bus_bool;
            auto bus_stop = (base_and_stat.GetRoot().AsMap().at("base_requests")).AsArray();
            for (auto node : bus_stop) {
                auto bus_or_stop = node.AsMap();
                std::string type = bus_or_stop.at("type").AsString();
                if (type == "Stop") {
                    cata.AddStop(bus_or_stop.at("name").AsString(),{bus_or_stop.at("latitude").AsDouble(),bus_or_stop.at("longitude").AsDouble()});
                for(auto [name,node] : bus_or_stop.at("road_distances").AsMap()) {                   cata.SetDist(bus_or_stop.at("name").AsString(),name,node.AsInt());   
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
            for(auto [name,stops_] : buses) {
                 std::vector<road_objects::Stop*> stops;;
                 for (auto stop : stops_) {
                     stops.push_back(cata.FindStop(stop));
                     cata.FindStop(stop)->buses.insert(name);
                 }
                 cata.AddBus(name, stops, bus_bool.at(name));
            }
         }
    }
}
