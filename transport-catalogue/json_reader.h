#pragma once
#include "transport_catalogue.h"
#include "json.h"
namespace transport_catalog {
    namespace reader {
        void InputReader(TransportCatalogue& cata, json::Document& base_and_stat);
    }
}
