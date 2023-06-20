#include "input_reader.h"
#include <cassert>
#include <sstream>
#include "stat_reader.h"

int main() {
    transport_catalog::TransportCatalogue cata;
    transport_catalog::reader::input_reader(cata,std::cin);
    transport_catalog::reader::stat_reader(cata, std::cin);
}