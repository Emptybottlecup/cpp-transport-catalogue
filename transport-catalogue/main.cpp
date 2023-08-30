#include "json_reader.h"
#include <cassert>
#include <sstream>
#include "request_handler.h"

int main() {
    transport_catalog::TransportCatalogue cata;
    ReadRequests(cata, std::cin);
}
