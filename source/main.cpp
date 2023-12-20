#include "cli.hpp"

#include <cstdlib>   // EXIT_FAILURE
#include <exception> // std::exception
#include <iostream>  // std::cerr

auto main(int argc, char** argv) -> int
{
    try
    {
        return run_cli(argc, argv);
    }
    catch (std::exception const& exc)
    {
        std::cerr << exc.what() << '\n';
    }

    return EXIT_FAILURE;
}
