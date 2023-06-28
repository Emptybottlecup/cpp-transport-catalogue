#include "transport_catalogue.h"
#include <set>
#include "geo.h"
namespace transport_catalog
{
void TransportCatalogue::AddBus(road_objects::Route&& route_) {
     for(int i = 0; i < route_.stops.size() - 1; ++i) {
            route_.dist_cur += stop_dist.at(std::make_pair(route_.stops[i],route_.stops[i+1]));
            route_.dist_fact += dist_count::ComputeDistance({(*(route_.stops[i])).latitude,(*(route_.stops[i])).longitude},{(*(route_.stops[i+1])).latitude,(*(route_.stops[i+1])).longitude});
        }
    route_.cur = route_.dist_cur/route_.dist_fact;
    routs.push_back(route_);
    routs_map.insert(make_pair(std::string_view((routs.back()).name),&(routs.back())));
}

void TransportCatalogue::AddStop(road_objects::Stop&& stop_) {
        stops.push_back(stop_);
        stops_map.insert(make_pair(std::string_view((stops.back()).name),&(stops.back())));
    stop_bus[(&(stops.back()))];
}


road_objects::Route TransportCatalogue::FindBus(const std::string& name) const {
    if(routs_map.count(name) != 0) {
        return *(routs_map.at(name));
    }
    road_objects::Route not_route;
    not_route.name = "not found";
    return not_route;
}

road_objects::Stop* TransportCatalogue::FindStop(const std::string& name) const {
    if(stops_map.count(name) != 0) {
        return stops_map.at(name);
    }
    road_objects::Stop* not_stop = new road_objects::Stop;
    not_stop->name = "not found";
    return not_stop;    
}

void TransportCatalogue::AddDist(const distance_all_stops& dist) {
      for(auto dist_item : dist) {
          for(auto dist_next_stop : dist_item.second) {
  if(stop_dist.count(std::make_pair(stops_map.at(dist_item.first),stops_map.at(dist_next_stop.first))) != 0) {
   stop_dist.at(std::make_pair(stops_map.at(dist_item.first),stops_map.at(dist_next_stop.first))) = dist_next_stop.second;   
  }
 else {             stop_dist.insert(std::make_pair(std::make_pair(stops_map.at(dist_item.first),stops_map.at(dist_next_stop.first)),dist_next_stop.second));
 if(stop_dist.count(std::make_pair(stops_map.at(dist_next_stop.first),stops_map.at(dist_item.first))) == 0) {
stop_dist.insert(std::make_pair(std::make_pair(stops_map.at(dist_next_stop.first),stops_map.at(dist_item.first)),dist_next_stop.second));
     }
 }    
          }
      }
  }
    
std::map<std::string_view,road_objects::Stop*>& TransportCatalogue::get_stops_map() {
        return stops_map;
    }
}