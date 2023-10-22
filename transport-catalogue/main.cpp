#include <fstream>
#include <iostream>
#include <string_view>
#include "transport_catalogue.h"
#include "json_reader.h"
#include "serialization.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        transport_catalog::TransportCatalogue catalog;
        json::Document doc(json::Load(std::cin)); 
        transport_catalog::reader::InputReader(catalog, doc);
        auto& serialization_settings = doc.GetRoot().AsDict().at("serialization_settings"s);
        auto& render_settings = (doc.GetRoot().AsDict()).at("render_settings");
        MapRender map = CreateMapRender(render_settings.AsDict());
        auto rout_settings = (doc.GetRoot().AsDict()).at("routing_settings");
        Parametrs param;
        param.wait_time_ = rout_settings.AsDict().at("bus_wait_time").AsInt();
        param.velocity_ = rout_settings.AsDict().at("bus_velocity").AsDouble(); 
        TransportRouter router(param);
        router.CreateGraph(catalog);
        std::ofstream file(serialization_settings.AsDict().at("file"s).AsString(), std::ios::binary);
        if (file.is_open()) {
            Serialize(catalog,map,router,file);
        }
}
    else if (mode == "process_requests"sv) {
    json::Document doc(json::Load(std::cin)); 
    std::ifstream proto_file((doc.GetRoot().AsDict().at("serialization_settings"s)).AsDict().at("file"s).AsString(), std::ios::binary);
    json::Array output; 
    if (proto_file) {
        auto [catalog,map_render,router,graph,stop_ids] = Deserialize(proto_file);
        router.SetGraph(graph,stop_ids);
        json::Array bus_and_stop = ((doc.GetRoot().AsDict()).at("stat_requests")).AsArray(); 
        for (auto request : bus_and_stop) { 
            json::Dict map = request.AsDict(); 
            auto type = map.at("type").AsString(); 
            if (type == "Stop") {                
                output.push_back(PrintStop(map,catalog).AsDict()); 
            } 
            if (type == "Bus") {              
                output.push_back(PrintBus(map,catalog).AsDict()); 
            }
            if(type == "Map") { 
            output.push_back(PrintMap(map, map_render,catalog).AsDict()); 
            }
            if(type == "Route") {
            output.push_back(PrintRouting(map, router ,catalog).AsDict());   
            }
        } 
        json::Print(json::Document{output}, std::cout); 
    }
}
    else {
        PrintUsage();
        return 1;
    }
}