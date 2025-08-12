#pragma once

#include <iostream>

extern bool g_verbose;

#define LOG(msg) do { if (g_verbose) { std::cout << msg << std::endl; } } while(0)