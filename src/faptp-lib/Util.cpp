#include <sstream>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <faptp-lib/Util.h>
#include <faptp-lib/Aleatorio.h>

#ifdef _WIN32
#include <windows.h>
#endif

int Util::getPosition(int y, int x, int z, int Y, int Z)
{
    return (x + (y * Y) + (z * Y * Z));
}

void Util::get3DMatrix(std::size_t pLinear, int triDimensional[3], int X, int Y, int Z)
{
    auto tamanho = X * Y * Z;

    auto dia = ((pLinear % tamanho) / Y) % X;
    auto bloco = ((pLinear % tamanho) % Y);
    auto camada = pLinear / (X * Y);

    triDimensional[0] = gsl::narrow_cast<int>(bloco); 
    triDimensional[1] = gsl::narrow_cast<int>(dia);
    triDimensional[2] = gsl::narrow_cast<int>(camada);
    

    /*auto dia = pLinear % X;
    pLinear /= X;
    auto bloco = pLinear % Y;
    pLinear /= Y;
    auto camada = pLinear;

    triDimensional[]*/
}

std::vector<std::string>& Util::strSplit(const std::string& s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> Util::strSplit(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    strSplit(s, delim, elems);
    return elems;
}

double Util::timeDiff(clock_t tf, clock_t t0)
{
    return (tf - t0) / 1000000.0 * 1000;
}

long long Util::chronoDiff(std::chrono::time_point<std::chrono::high_resolution_clock> t_end,
                           std::chrono::time_point<std::chrono::high_resolution_clock> t_begin)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>
            (t_end - t_begin).count();
}

std::chrono::time_point<std::chrono::high_resolution_clock> Util::now()
{
    return std::chrono::high_resolution_clock::now();
}

int Util::randomBetween(int min, int max)
{
    if (max < min) {
        throw std::logic_error{"Minimo maior que maximo em randomBetween"};
    }

    if (max == min) {
        return max;
    }

    return aleatorio::randomInt() % (max - min) + min;
}

double Util::randomDouble()
{
    constexpr auto max_random = 32767;
    return static_cast<double>(randomBetween(0, max_random)) / max_random;
}

int Util::warpIntervalo(int i, int tamIntervalo, int comecoIntervalo)
{
    return (i - comecoIntervalo) % tamIntervalo + comecoIntervalo;
}


std::string Util::join_path(const std::vector<std::string>& folders, const std::string& file)
{
    auto oss = std::ostringstream {};
#if defined(_WIN32)
    auto sep = "\\";
#else
    auto sep = "/";
#endif
    for (const auto& f : folders) {
        oss << f << sep;
    }

    oss << file;

    return oss.str();
}

void Util::create_folder(const std::string& path)
{
#if defined(_WIN32)
    std::string command {"mkdir \"\"" + path + "\"\" 2> NUL"};
#else
    std::string command {"mkdir -p \"\"" + path + "\"\" > /dev/null 2>&1"};
#endif
    system(command.c_str());
}

std::size_t Util::hash_string(const std::string& str)
{
    static std::hash<std::string> hasher;
    return hasher(str);
}

std::string Util::date_time()
{
    auto facet = new boost::posix_time::time_facet("%d-%m-%Y %H:%M:%S");
    std::ostringstream oss;
    oss.imbue(std::locale(oss.getloc(), facet));
    oss << boost::posix_time::second_clock::local_time();
    return oss.str();
}

std::string Util::get_computer_name()
{
#ifdef _WIN32
    char buf[MAX_COMPUTERNAME_LENGTH + 1];
    unsigned long len;
    GetComputerName(buf, &len);

    return buf;
#else
    return "0";
#endif
}
