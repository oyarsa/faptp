#pragma once

#include <string>

namespace experimento {

void teste_tempo(int timeout_sec = 120);

void ag_cli(const std::string& input, const std::string& file,
            const std::string& servidor, long long timeout);

void sa_ils_cli(const std::string& input, const std::string& file,
                const std::string& servidor, long long timeout);

void wdju_cli(const std::string& input, const std::string& file,
              const std::string& servidor, long long timeout);

void hysst_cli(const std::string& input, const std::string& file,
               const std::string& servidor, long long timeout);

} // namespace experimento
