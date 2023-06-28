#pragma once
#include "input_reader.h"
namespace transport_catalog {
    namespace reader {
void stat_reader(transport_catalog::TransportCatalogue& catalog, std::istream& input);
std::ostream& For_bus(transport_catalog::TransportCatalogue& catalog, const std::string& bus, std::ostream& out);
std::ostream& For_stop(transport_catalog::TransportCatalogue& catalog, const std::string& stop, std::ostream& out);        
    }
}