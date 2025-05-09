#pragma once

#include "AudioFile.h"

struct SampleAudioBuffer
{
  std::vector<float> left_buffer;
  std::vector<float> right_buffer;
  unsigned int interpolation = 1;
  unsigned int virtual_size = 0;

  void clear()
  {
    // Trick to free up memory in buffers
    std::vector<float>().swap(left_buffer);
    std::vector<float>().swap(right_buffer);
  }

  void push_back(float audio_left, float audio_right)
  {
    left_buffer.push_back(audio_left);
    right_buffer.push_back(audio_right);
  }

  unsigned int size()
  {
    return(left_buffer.size());
  }

  void read(unsigned int index, float *left_audio_ptr, float *right_audio_ptr)
  {
    size_t buf_size = left_buffer.size();
    
    if(index >= buf_size)
    {
      *left_audio_ptr = 0;
      *right_audio_ptr = 0;
    }
    else
    {
      *left_audio_ptr = left_buffer[index];
      *right_audio_ptr = right_buffer[index];
    }
  }

  // Read sample using linear interpolation
  void readLI(double position, float *left_audio_ptr, float *right_audio_ptr)
  {
    unsigned int index = std::floor(position); // convert float to int
    size_t buf_size = left_buffer.size();

    // If out of bounds, return zeros
    if(index >= (buf_size - 1))
    {
      *left_audio_ptr = 0;
      *right_audio_ptr = 0;
    }
    // Else, compute and return the interpolated values
    else
    {
      float distance = position - (float) index;
      *left_audio_ptr = left_buffer[index] + ((left_buffer[index + 1] - left_buffer[index]) * distance);
      *right_audio_ptr = right_buffer[index] + ((right_buffer[index + 1] - right_buffer[index]) * distance);
    }
  }
};

struct Sample
{
  std::string path = "";
  std::string filename;
  std::string display_name;
  bool loading = false;
  bool loaded = false;
  bool queued_for_loading = false;
  std::string queued_path = "";
  unsigned int sample_length = 0;
  SampleAudioBuffer sample_audio_buffer;
  float sample_rate = 44100.0;                // This is the sample rate in which the sample was recorded
  unsigned int channels = 0;
  AudioFile<float> audioFile;                 // For loading samples and saving samples

  Sample()
  {
    // No need to clear already-empty buffer
    // sample_audio_buffer.clear();
    loading = false;
    filename = "[ empty ]";
    display_name = "[ empty ]";
    path = "";
    sample_rate = 0;
    channels = 0;

    audioFile.setNumChannels(2);
    audioFile.setSampleRate(44100);
  }

  ~Sample()
  {
    sample_audio_buffer.clear();
  }

  bool load(const std::string& path)
  {
    // Set loading flags
    this->loading = true;
    this->loaded = false;

    // initialize some transient variables
    float left = 0;
    float right = 0;

    printf("path: %s\n", path.c_str());

    // Load the audio file
    if(! audioFile.load(path))
    {
      
      printf("audioFile.load(path) failed\n");
      this->loading = false;
      this->loaded = false;
      return(false);
    }

    // Read details about the loaded sample
    uint32_t sampleRate = audioFile.getSampleRate();
    int numSamples = audioFile.getNumSamplesPerChannel();
    int numChannels = audioFile.getNumChannels();

    this->channels = numChannels;
    this->sample_rate = sampleRate;
    sample_audio_buffer.clear();

    //
    // Copy the sample data from the AudioFile object to floating point vectors
    //
    sample_audio_buffer.left_buffer.reserve(numSamples);
    sample_audio_buffer.right_buffer.reserve(numSamples);
    
    for(int i = 0; i < numSamples; i++)
    {
      if(numChannels == 2)
      {
        left = audioFile.samples[0][i];  // [channel][sample_index]
        right = audioFile.samples[1][i];
      }
      else if(numChannels == 1)
      {
        left = audioFile.samples[0][i];
        right = left;
      }

      sample_audio_buffer.push_back(left, right);
    }
    
    // Now that the audioFile has been read into memory, clear it out
    std::vector<float>().swap(audioFile.samples[0]);
    std::vector<float>().swap(audioFile.samples[1]);

    // Store sample length and file information to this object for the rest
    // of the patch to reference.
    this->sample_length = sample_audio_buffer.size();
    this->filename = system::getFilename(path);
    this->display_name = filename;
    this->display_name.erase(this->display_name.length()-4); // remove the .wav extension
    this->path = path;

    this->loading = false;
    this->loaded = true;

    printf("sample.loaded: %d\n", this->loaded);

    return(true);
  };


  bool isLoaded()
  {
    return(this->loaded);
  }

  // Where to put recording code and how to save it?
  void initialize_recording()
  {
    // Clear out audioFile data. audioFile represents the .wav file information
    // that can be loaded or saved. In this case, we're going to be saving incoming audio pretty soon.
    std::vector<float>().swap(audioFile.samples[0]);
    std::vector<float>().swap(audioFile.samples[1]);

    // Also clear out the sample audio information
    sample_audio_buffer.clear();
    sample_length = 0;
  }

  void record_audio(float left, float right)
  {
    // Reserve more space if needed to reduce reallocations
    if (audioFile.samples[0].size() == audioFile.samples[0].capacity()) {
      size_t new_capacity = audioFile.samples[0].empty() ? 44100 : audioFile.samples[0].capacity() * 2;
      audioFile.samples[0].reserve(new_capacity);
      audioFile.samples[1].reserve(new_capacity);
      sample_audio_buffer.left_buffer.reserve(new_capacity);
      sample_audio_buffer.right_buffer.reserve(new_capacity);
    }
    
    // Store incoming audio both in the audioFile for saving and in the sample for playback
    audioFile.samples[0].push_back(left);
    audioFile.samples[1].push_back(right);

    sample_audio_buffer.push_back(left, right);
    sample_length = sample_audio_buffer.size();
  }

  void save_recorded_audio(const std::string& path)
  {
    audioFile.save(path);
  }

  // Read stereo audio from the buffer at position _index_
  void read(unsigned int index, float *left_audio_ptr, float *right_audio_ptr)
  {
    sample_audio_buffer.read(index, left_audio_ptr, right_audio_ptr);
  }

  // Read sample, applying Linear Interpolation
  void readLI(double position, float *left_audio_ptr, float *right_audio_ptr)
  {
    sample_audio_buffer.readLI(position, left_audio_ptr, right_audio_ptr);
  }

  unsigned int size()
  {
    return(sample_length);
  }

  void setSize(unsigned int sample_length)
  {
    this->sample_length = sample_length;
  }

  float getSampleRate()
  {
    return(this->sample_rate);
  }

  void unload()
  {
    this->sample_audio_buffer.clear();
    this->sample_length = 0;
    this->filename = "";
    this->display_name = "";
    this->loaded = false;
  }

  std::string getFilename()
  {
    return(this->filename);
  }

  std::string getPath()
  {
    return(this->path);
  }

};
