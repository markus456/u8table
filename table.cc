#include <iostream>
#include "table.hh"

std::vector<std::string> split_line(std::string_view line, std::string_view delim)
{
    std::vector<std::string> rv;

    auto pos = line.find(delim);

    while (pos != std::string_view::npos)
    {
        rv.emplace_back(line.substr(0, pos));
        line.remove_prefix(pos + delim.size());
        pos = line.find(delim);
    }

    if (!line.empty())
    {
        rv.emplace_back(line);
    }

    return rv;
}

int main(int argc, char** argv)
{
    std::string delim = " ";
    std::string line;
    std::vector<std::vector<std::string>> table;

    if (argc > 1 && *argv[1])
    {
        delim = argv[1];
    }

    while (std::getline(std::cin, line))
    {
        table.push_back(split_line(line, delim));
    }

    std::cout << u8tbl::create(table) << std::endl;
    return 0;
}
