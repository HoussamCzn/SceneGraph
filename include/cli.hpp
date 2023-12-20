#pragma once

/**
 * @brief Runs the command line interface.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return The exit code.
 */
auto run_cli(int argc, char* argv[]) noexcept -> int;