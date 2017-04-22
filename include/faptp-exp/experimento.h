#pragma once

#include <string_view>

namespace experimento {

void teste_tempo(int timeout_sec = 120);

void ag_cli(std::string_view input, std::string_view file,
            std::string_view servidor, long long timeout);

void sa_ils_cli(std::string_view input, std::string_view file,
                std::string_view servidor, long long timeout);

void wdju_cli(std::string_view input, std::string_view file,
              std::string_view servidor, long long timeout);

void hysst_cli(std::string_view input, std::string_view file,
               std::string_view servidor, long long timeout);

} // namespace experimento
