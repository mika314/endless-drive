#pragma once
#include <filesystem>
#include <vector>

auto loadAudio(std::filesystem::path) -> std::vector<float>;
