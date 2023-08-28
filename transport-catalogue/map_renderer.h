#pragma once
#include "geo.h"
#include "svg.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>
#include "json_reader.h"

inline const double EPSILON = 1e-6;

inline bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};

struct MapRender {
    double width = 0;
    double height = 0;
    double padding = 0;
    double line_width = 0;
    double stop_radius = 0;
    int bus_label_font_size = 0;
    svg::Point bus_label_offset = { 0, 0 };
    int stop_label_font_size = 0;
    svg::Point stop_label_offset = { 0, 0 };
    svg::Color underlayer_color = { svg::NoneColor };
    double underlayer_width = 0;
    std::vector<svg::Color> color_palette {};
    std::vector<svg::Polyline> Route(std::map<std::string_view, transport_catalog::road_objects::Route*> buses, SphereProjector& sp);
    svg::Document CreateSVG(std::map<std::string_view, transport_catalog::road_objects::Route*> buses);
    std::vector<svg::Text> GetBusLabel(std::map<std::string_view, transport_catalog::road_objects::Route*> buses, SphereProjector& sp);
    std::vector<svg::Circle> GetStopsSymbols(std::map<std::string_view, transport_catalog::road_objects::Stop*> stops, SphereProjector& sp);
    std::vector<svg::Text> GetStopsLabels(std::map<std::string_view, transport_catalog::road_objects::Stop*> stops, SphereProjector& sp);
};

inline MapRender CreateMapRender(json::Dict request_map) {
    MapRender maprender;
    maprender.width = request_map.at("width").AsDouble();
    maprender.height = request_map.at("height").AsDouble();
    maprender.padding = request_map.at("padding").AsDouble();
    maprender.line_width = request_map.at("line_width").AsDouble();
    maprender.stop_radius = request_map.at("stop_radius").AsDouble();
    maprender.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
    
json::Array bus_label_offset = request_map.at("bus_label_offset").AsArray();
    
maprender.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    
maprender.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    
json::Array stop_label_offset = request_map.at("stop_label_offset").AsArray();
maprender.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };
    
    if (request_map.at("underlayer_color").IsString() == true) { maprender.underlayer_color = request_map.at("underlayer_color").AsString();
    }
    else if (request_map.at("underlayer_color").IsArray()) {
json::Array underlayer_color = request_map.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            maprender.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            maprender.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        } 
        else {
            throw std::logic_error("wrong underlayer colortype"); }
        } 
    else { 
        throw std::logic_error("wrong underlayer color");
    }    
    
    maprender.underlayer_width = request_map.at("underlayer_width").AsDouble();
    
    json::Array color_palette = request_map.at("color_palette").AsArray();
    for (auto color_element : color_palette) {
        if (color_element.IsString()) {
            maprender.color_palette.push_back(color_element.AsString());
        }    
        else if (color_element.IsArray()) {
            json::Array color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                maprender.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                maprender.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            } 
            else { 
                throw std::logic_error("wrong color_palette type");
            }        
        } 
        else { 
            throw std::logic_error("wrong color_palette");
        }
    }
    
    return maprender;
}
