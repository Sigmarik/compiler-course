#pragma once

#include <string>

#include "ast.hpp"

extern std::string s_source_file;

extern void set_input_file(const char* filename);

extern Sequence* root_sequence;
