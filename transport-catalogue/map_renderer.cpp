#include "map_renderer.h" 
 
std::vector<svg::Text> MapRender::GetBusLabel(std::map<std::string_view, transport_catalog::road_objects::Route*> buses, SphereProjector& sp) { 
    std::vector<svg::Text> result; 
    size_t color_num = 0; 
    for (const auto& [bus_number, bus] : buses) { 
        if (bus->stops.empty()) continue; 
        svg::Text text; 
        svg::Text underlayer; 
        text.SetPosition(sp(bus->stops[0]->coord)); 
        text.SetOffset(bus_label_offset); 
        text.SetFontSize(bus_label_font_size); 
        text.SetFontFamily("Verdana"); 
        text.SetFontWeight("bold"); 
        text.SetData(bus->name); 
        text.SetFillColor(color_palette[color_num]); 
        if (color_num < (color_palette.size() - 1)) ++color_num; 
        else color_num = 0; 
         
        underlayer.SetPosition(sp(bus->stops[0]->coord)); 
        underlayer.SetOffset(bus_label_offset); 
        underlayer.SetFontSize(bus_label_font_size); 
        underlayer.SetFontFamily("Verdana"); 
        underlayer.SetFontWeight("bold"); 
        underlayer.SetData(bus->name); 
        underlayer.SetFillColor(underlayer_color); 
        underlayer.SetStrokeColor(underlayer_color); 
        underlayer.SetStrokeWidth(underlayer_width); 
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND); 
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND); 
         
        result.push_back(underlayer); 
        result.push_back(text); 
         
        if (bus->is_roundtrip == false && bus->un_stop[0] != bus->un_stop[bus->un_stop.size() - 1]) { 
            svg::Text text2 {text}; 
            svg::Text underlayer2 {underlayer}; 
            text2.SetPosition(sp(bus->un_stop[bus->un_stop.size() - 1]->coord)); 
            underlayer2.SetPosition(sp(bus->un_stop[bus->un_stop.size() - 1]->coord));    
            result.push_back(underlayer2); 
            result.push_back(text2); 
    }} 
     
    return result; 
} 
 
std::vector<svg::Circle> MapRender::GetStopsSymbols(std::map<std::string_view, transport_catalog::road_objects::Stop*> stops, SphereProjector& sp) { 
    std::vector<svg::Circle> result; 
    for (const auto& [stop_name, stop] : stops) { 
        svg::Circle symbol; 
        symbol.SetCenter(sp(stop->coord)); 
        symbol.SetRadius(stop_radius); 
        symbol.SetFillColor("white");   
        result.push_back(symbol); 
    } 
     
    return result; 
} 
 
std::vector<svg::Text> MapRender::GetStopsLabels(std::map<std::string_view, transport_catalog::road_objects::Stop*> stops, SphereProjector& sp) { 
    std::vector<svg::Text> result; 
    svg::Text text; 
    svg::Text underlayer; 
    for (const auto& [stop_name, stop] : stops) { 
        text.SetPosition(sp(stop->coord)); 
        text.SetOffset(stop_label_offset); 
        text.SetFontSize(stop_label_font_size); 
        text.SetFontFamily("Verdana"); 
        text.SetData(stop->name); 
        text.SetFillColor("black"); 
         
        underlayer.SetPosition(sp(stop->coord)); 
        underlayer.SetOffset(stop_label_offset); 
        underlayer.SetFontSize(stop_label_font_size); 
        underlayer.SetFontFamily("Verdana"); 
        underlayer.SetData(stop->name); 
        underlayer.SetFillColor(underlayer_color); 
        underlayer.SetStrokeColor(underlayer_color); 
        underlayer.SetStrokeWidth(underlayer_width); 
        underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND); 
        underlayer.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND); 
         
        result.push_back(underlayer); 
        result.push_back(text); 
    } 
     
    return result; 
} 
 
std::vector<svg::Polyline> MapRender::Route(std::map<std::string_view, transport_catalog::road_objects::Route*> buses, SphereProjector& sp) { 
    std::vector<svg::Polyline> result; 
    size_t color_num = 0; 
    for (const auto& [bus_number, bus] : buses) { 
        if (bus->stops.empty()) continue; 
        std::vector<transport_catalog::road_objects::Stop*> route_stops{ bus->stops.begin(), bus->stops.end() }; 
        svg::Polyline line; 
        for (const auto& stop : route_stops) { 
            line.AddPoint(sp(stop->coord)); 
        } 
        line.SetStrokeColor(color_palette[color_num]); 
        line.SetFillColor("none"); 
        line.SetStrokeWidth(line_width); 
        line.SetStrokeLineCap(svg::StrokeLineCap::ROUND); 
        line.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND); 
         
        if (color_num < (color_palette.size() - 1)) ++color_num; 
        else color_num = 0; 
         
        result.push_back(line); 
    } 
     
    return result; 
} 
 
svg::Document MapRender::CreateSVG(std::map<std::string_view, transport_catalog::road_objects::Route*> buses) { 
    svg::Document result; 
    std::vector<geo::Coordinates> route_stops_coord; 
    std::map<std::string_view, transport_catalog::road_objects::Stop*> all_stops; 
    for (auto& [bus_number, bus] : buses) { 
        for (const auto& stop : bus->un_stop) { 
            route_stops_coord.push_back(stop->coord); 
            all_stops[stop->name] = stop; 
        } 
    } 
    SphereProjector sp(route_stops_coord.begin(), route_stops_coord.end(), width, height, padding); 
    for (const auto& line : Route(buses, sp)) { 
        result.Add(line); 
    } 
    for (const auto& text : GetBusLabel(buses, sp)) result.Add(text); 
    for (const auto& circle : GetStopsSymbols(all_stops, sp)) result.Add(circle); 
    for (const auto& text : GetStopsLabels(all_stops, sp)) result.Add(text); 
 
    return result; 
}