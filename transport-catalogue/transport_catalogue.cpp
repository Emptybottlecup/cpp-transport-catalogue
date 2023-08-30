#include "transport_catalogue.h"
#include <set>
#include "geo.h"
namespace transport_catalog
{
    
 void TransportCatalogue::AddBus(const std::string& name, std::vector<road_objects::Stop*>& stops, bool is_roundtrip) {
    std::vector<road_objects::Stop*> un;
     for(auto i : stops) {
         un.push_back(i);
     }
        if(is_roundtrip == false) {
                  for(int i = stops.size() - 2; i >= 0; --i) {
                 auto stop = stops[i];
                 stops.push_back(stop);
                   }    
                }
     
        road_objects::Route bus(name,stops,is_roundtrip);
     bus.un_stop = un;
     
        for(auto i : stops) {
            bus.unique.insert(i);
        }
      int route_length = 0;
      double geographic_length = 0.0;

    for (size_t i = 0; i < bus.stops.size() - 1; ++i) {
        auto from = bus.stops[i];
        auto to = bus.stops[i + 1];
        route_length += GetDistance(from->name, to->name);
        geographic_length += geo::ComputeDistance(from->coord,
        to->coord);     
    }
     
     bus.cur = route_length/geographic_length;
     bus.dist_fact = route_length; 
        routs.push_back(bus);
        routs_map.insert(make_pair(std::string_view((routs.back()).name),&(routs.back())));
         for (road_objects::Stop* stop : stops) {
            (stop_bus[stop]).insert(bus.name);
          }
    }

void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates coordinates) {
        road_objects::Stop stop(name, coordinates);
        stops.push_back(stop);
        stops_map.insert(make_pair(std::string_view((stops.back()).name),&(stops.back())));
        stop_bus[(&(stops.back()))];
}


road_objects::Route* TransportCatalogue::FindBus(const std::string name) const {
    if(routs_map.count(name) != 0) {
        return routs_map.at(name);
    }
    road_objects::Route* not_route = new road_objects::Route;
    not_route->name = "not found";
    return not_route;
}

road_objects::Stop* TransportCatalogue::FindStop(const std::string name) const {
    if(stops_map.count(name) != 0) {
        return stops_map.at(name);
    }
    road_objects::Stop* not_stop = new road_objects::Stop;
    not_stop->name = "not found";
    return not_stop;    
}
    
std::map<std::string_view,road_objects::Stop*>& TransportCatalogue::GetStopsMap() {
        return stops_map;
    }
    
void TransportCatalogue::SetDistance(std::string from, std::string to, int dist_) {
        dist[{from,to}] = dist_;
}
    
int TransportCatalogue::GetDistance(std::string from, std::string to) {
    if (dist.count({ from, to }) != 0) {
        return dist.at({ from, to }); 
    }
    else if(dist.count({ to, from }) != 0) {
         return dist.at({ to, from });
    }
    return 0;
}
    
    std::map<std::string_view,road_objects::Route*> TransportCatalogue::GetBus() {
    std::map<std::string_view,road_objects::Route*> result;
    for (auto bus : routs_map) {
        result.emplace(bus);
    }
    return result;
}
}
