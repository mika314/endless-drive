#include "sound-wave.hpp"
#include "load-audio.hpp"

SoundWave::SoundWave(const std::string &path, class Assets &) : pcm(loadAudio("data/" + path + ".wav"))
{
}
