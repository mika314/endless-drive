#include "consts.hpp"
#include <filesystem>
#include <log/log.hpp>
#include <vector>

#define MA_NO_DEVICE_IO
#include "miniaudio.h"

auto loadAudio(std::filesystem::path filePath) -> std::vector<float>
{
  const ma_format TARGET_FORMAT = ma_format_f32;
  const ma_uint32 TARGET_CHANNELS = ChN;
  const ma_uint32 TARGET_SAMPLE_RATE = SampleRate;

  ma_decoder_config config = ma_decoder_config_init(TARGET_FORMAT, TARGET_CHANNELS, TARGET_SAMPLE_RATE);
  ma_decoder decoder;
  ma_result result = ma_decoder_init_file(filePath.string().c_str(), &config, &decoder);
  if (result != MA_SUCCESS)
  {
    ma_decoder_config nativeConfig =
      ma_decoder_config_init(TARGET_FORMAT, 0, 0); // 0 = use source channels/sample rate
    result = ma_decoder_init_file(filePath.string().c_str(), &nativeConfig, &decoder);
    if (result != MA_SUCCESS)
    {
      LOG("Failed to load audio file");
      return {};
    }
  }

  // Determine total frame count (length) if possible
  ma_uint64 totalFrames = 0;
  ma_decoder_get_length_in_pcm_frames(
    &decoder, &totalFrames); // Retrieve total PCM frame count:contentReference[oaicite:12]{index=12}

  std::vector<float> samples;
  samples.reserve((size_t)totalFrames * decoder.outputChannels);

  if (totalFrames == 0)
  {
    // Unknown length (e.g., streaming format) – read in chunks until end
    std::vector<float> chunkBuffer(4096 * decoder.outputChannels);
    ma_uint64 framesRead = 0;
    do
    {
      ma_uint64 framesThisRead = 0;
      result = ma_decoder_read_pcm_frames(&decoder, chunkBuffer.data(), 4096, &framesThisRead);
      if (framesThisRead > 0)
      {
        // Append read samples to output vector
        samples.insert(samples.end(),
                       chunkBuffer.begin(),
                       chunkBuffer.begin() + framesThisRead * decoder.outputChannels);
      }
      framesRead = framesThisRead;
    } while (result == MA_SUCCESS && framesRead > 0);
    totalFrames = samples.size() / decoder.outputChannels;
  }
  else
  {
    // Known length: read all frames at once
    samples.resize((size_t)totalFrames * decoder.outputChannels);
    ma_uint64 framesRead = 0;
    ma_result readRes = ma_decoder_read_pcm_frames(&decoder, samples.data(), totalFrames, &framesRead);
    if (readRes != MA_SUCCESS && readRes != MA_AT_END)
    {
      ma_decoder_uninit(&decoder);
      LOG("Error reading audio frames");
      return {};
    }
    // If we read fewer frames than expected (end of stream reached early), resize the vector down
    samples.resize((size_t)framesRead * decoder.outputChannels);
    totalFrames = framesRead;
  }

  // Save output format info before uninitializing (for fallback processing)
  ma_uint32 srcChannels = decoder.outputChannels;
  ma_uint32 srcSampleRate = decoder.outputSampleRate;
  ma_decoder_uninit(&decoder);

  // If the decoder did not already convert to 44100 Hz stereo, perform conversions now
  if (srcChannels != TARGET_CHANNELS || srcSampleRate != TARGET_SAMPLE_RATE)
  {
    // **Channel conversion**: Up-mix mono to stereo (duplicate channel).
    if (srcChannels == 1 && TARGET_CHANNELS == ChN)
    {
      std::vector<float> stereoSamples;
      stereoSamples.resize(samples.size() * ChN);
      size_t frameCount = samples.size(); // frameCount equals sample count since srcChannels==1
      for (size_t i = 0; i < frameCount; ++i)
      {
        float monoSample = samples[i];
        for (auto j = 0; j < ChN; ++j)
          stereoSamples[2 * i + j] = monoSample;
      }
      samples.swap(stereoSamples);
      srcChannels = ChN;
    }
    // (If source had more than 2 channels, a downmix to stereo could be implemented here as needed.)

    // **Resampling**: Nearest-neighbor resample if sample rate mismatch
    if (srcSampleRate != TARGET_SAMPLE_RATE)
    {
      size_t inFrameCount = samples.size() / srcChannels;
      double resampleRatio = double(TARGET_SAMPLE_RATE) / double(srcSampleRate);
      size_t outFrameCount = (size_t)(inFrameCount * resampleRatio + 0.5); // target number of frames
      std::vector<float> resampled;
      resampled.resize(outFrameCount * srcChannels);
      for (size_t outFrame = 0; outFrame < outFrameCount; ++outFrame)
      {
        // Find nearest input frame index
        size_t inFrame = (size_t)(outFrame / resampleRatio);
        if (inFrame >= inFrameCount)
        {
          inFrame = inFrameCount > 0 ? inFrameCount - 1 : 0;
        }
        // Copy all channels (here srcChannels should be 2 after up-mix)
        for (ma_uint32 ch = 0; ch < srcChannels; ++ch)
        {
          resampled[outFrame * srcChannels + ch] = samples[inFrame * srcChannels + ch];
        }
      }
      samples.swap(resampled);
      // Now samples vector contains stereo data at the new sample rate.
    }
  }

  // At this point, 'samples' holds interleaved stereo PCM data at 44100 Hz in float format.

  // Normalize samples
  float maxAmplitude = 0.0f;
  for (float sample : samples)
  {
    maxAmplitude = std::max(maxAmplitude, std::abs(sample));
  }

  if (maxAmplitude > 0.0f)
  {
    for (float &sample : samples)
    {
      sample /= maxAmplitude;
    }
  }
  return samples;
}
