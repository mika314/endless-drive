#include "font.hpp"

Font::Font(const std::string &aPath, class Assets &) : path(aPath) {}
Font::Font(Font &&v) : path(std::move(v.path)) {}
