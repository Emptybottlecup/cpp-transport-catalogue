#pragma once
#include "transport_catalogue.h"

namespace transport_catalog {
    namespace reader {
void input_reader(TransportCatalogue& catalog, std::istream& input);

std::string Split(const std::string& text);

std::string Check(const std::string& text);

std::vector<std::pair<std::string,double>> Split_dist(std::string text); 
    }
}