//=======================================================================
/** @file AudioFile.h
 *  @author Adam Stark
 *  @copyright Copyright (C) 2017  Adam Stark
 *
 * This file is part of the 'AudioFile' library
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
//=======================================================================

#ifndef _AS_AudioFile_h
#define _AS_AudioFile_h

// #include <iostream>
#include <vector>
#include <assert.h>
#include <string>
#ifndef METAMODULE
#include <fstream>
#else
#include <cstdio>  // For FILE*, fopen, fread, fwrite, etc.
#endif
#include <unordered_map>
#include <iterator>
#include <algorithm>


//=============================================================
/** The different types of audio file, plus some other types to 
 * indicate a failure to load a file, or that one hasn't been
 * loaded yet
 */
enum class AudioFileFormat
{
    Error,
    NotLoaded,
    Wave,
    Aiff
};

//=============================================================
/** 
 * Memory-optimized AudioFile Class
 * 
 * This implementation has been optimized for memory efficiency with the following techniques:
 * 1. Early validation to avoid allocating memory for invalid files
 * 2. Buffer pre-allocation to minimize reallocations
 * 3. Immediate freeing of temporary buffers after use
 * 4. Use of std::move to avoid unnecessary copying of large vectors
 * 5. Efficient sample rate conversion on load
 * 6. Optimized processing pipelines for different bit depths
 * 7. Vector swapping for immediate memory deallocation
 */
template <class T>
class AudioFile
{
public:
    
    //=============================================================
    typedef std::vector<std::vector<T> > AudioBuffer;
    
    //=============================================================
    /** Constructor */
    AudioFile();
        
    //=============================================================
    /** Loads an audio file from a given file path.
     * @Returns true if the file was successfully loaded
     */
    bool load (std::string filePath, uint32_t targetSampleRate = 0);
    
    /** Saves an audio file to a given file path.
     * @Returns true if the file was successfully saved
     */
    bool save (std::string filePath, AudioFileFormat format = AudioFileFormat::Wave);
        
    //=============================================================
    /** @Returns the sample rate */
    uint32_t getSampleRate() const;
    
    /** @Returns the number of audio channels in the buffer */
    int getNumChannels() const;

    /** @Returns true if the audio file is mono */
    bool isMono() const;
    
    /** @Returns true if the audio file is stereo */
    bool isStereo() const;
    
    /** @Returns the bit depth of each sample */
    int getBitDepth() const;
    
    /** @Returns the number of samples per channel */
    int getNumSamplesPerChannel() const;
    
    /** @Returns the length in seconds of the audio file based on the number of samples and sample rate */
    double getLengthInSeconds() const;
    
    /** Prints a summary of the audio file to the console */
    void printSummary() const;
    
    //=============================================================
    
    /** Set the audio buffer for this AudioFile by copying samples from another buffer.
     * @Returns true if the buffer was copied successfully.
     */
    bool setAudioBuffer (AudioBuffer& newBuffer);
    
    /** Sets the audio buffer to a given number of channels and number of samples per channel. This will try to preserve
     * the existing audio, adding zeros to any new channels or new samples in a given channel.
     */
    void setAudioBufferSize (int numChannels, int numSamples);
    
    /** Sets the number of samples per channel in the audio buffer. This will try to preserve
     * the existing audio, adding zeros to new samples in a given channel if the number of samples is increased.
     */
    void setNumSamplesPerChannel (int numSamples);
    
    /** Sets the number of channels. New channels will have the correct number of samples and be initialised to zero */
    void setNumChannels (int numChannels);
    
    /** Sets the bit depth for the audio file. If you use the save() function, this bit depth rate will be used */
    void setBitDepth (int numBitsPerSample);
    
    /** Sets the sample rate for the audio file. If you use the save() function, this sample rate will be used */
    void setSampleRate (uint32_t newSampleRate);
    
    //=============================================================
    /** A vector of vectors holding the audio samples for the AudioFile. You can 
     * access the samples by channel and then by sample index, i.e:
     *
     *      samples[channel][sampleIndex]
     */
    AudioBuffer samples;
    
    /** Resamples the audio data to match the target sample rate.
     * This is a memory-efficient implementation that converts sample rates
     * while minimizing memory usage.
     * @Returns true if the resampling was successful.
     */
    bool resampleToTargetRate(uint32_t targetSampleRate);
    
private:
    
    //=============================================================
    enum class Endianness
    {
        LittleEndian,
        BigEndian
    };
    
    //=============================================================
    AudioFileFormat determineAudioFileFormat (std::vector<uint8_t>& fileData);
    bool decodeWaveFile (std::vector<uint8_t>& fileData);
    bool decodeAiffFile (std::vector<uint8_t>& fileData);
    
    //=============================================================
    bool saveToWaveFile (std::string filePath);
    bool saveToAiffFile (std::string filePath);
    
    //=============================================================
    void clearAudioBuffer();
    
    //=============================================================
    int32_t fourBytesToInt (std::vector<uint8_t>& source, int startIndex, Endianness endianness = Endianness::LittleEndian);
    int16_t twoBytesToInt (std::vector<uint8_t>& source, int startIndex, Endianness endianness = Endianness::LittleEndian);
    int getIndexOfString (std::vector<uint8_t>& source, std::string s);
    
    //=============================================================
    T sixteenBitIntToSample (int16_t sample);
    int16_t sampleToSixteenBitInt (T sample);
    
    //=============================================================
    uint8_t sampleToSingleByte (T sample);
    T singleByteToSample (uint8_t sample);
    
    uint32_t getAiffSampleRate (std::vector<uint8_t>& fileData, int sampleRateStartIndex);
    bool tenByteMatch (std::vector<uint8_t>& v1, int startIndex1, std::vector<uint8_t>& v2, int startIndex2);
    void addSampleRateToAiffData (std::vector<uint8_t>& fileData, uint32_t sampleRate);
    T clamp (T v1, T minValue, T maxValue);
    
    //=============================================================
    void addStringToFileData (std::vector<uint8_t>& fileData, std::string s);
    void addInt32ToFileData (std::vector<uint8_t>& fileData, int32_t i, Endianness endianness = Endianness::LittleEndian);
    void addInt16ToFileData (std::vector<uint8_t>& fileData, int16_t i, Endianness endianness = Endianness::LittleEndian);
    
    //=============================================================
    bool writeDataToFile (std::vector<uint8_t>& fileData, std::string filePath);
    
    //=============================================================
    AudioFileFormat audioFileFormat;
    uint32_t sampleRate;
    int bitDepth;
};


//=============================================================
// Pre-defined 10-byte representations of common sample rates
static std::unordered_map <uint32_t, std::vector<uint8_t>> aiffSampleRateTable = {
    {8000, {64, 11, 250, 0, 0, 0, 0, 0, 0, 0}},
    {11025, {64, 12, 172, 68, 0, 0, 0, 0, 0, 0}},
    {16000, {64, 12, 250, 0, 0, 0, 0, 0, 0, 0}},
    {22050, {64, 13, 172, 68, 0, 0, 0, 0, 0, 0}},
    {32000, {64, 13, 250, 0, 0, 0, 0, 0, 0, 0}},
    {37800, {64, 14, 147, 168, 0, 0, 0, 0, 0, 0}},
    {44056, {64, 14, 172, 24, 0, 0, 0, 0, 0, 0}},
    {44100, {64, 14, 172, 68, 0, 0, 0, 0, 0, 0}},
    {47250, {64, 14, 184, 146, 0, 0, 0, 0, 0, 0}},
    {48000, {64, 14, 187, 128, 0, 0, 0, 0, 0, 0}},
    {50000, {64, 14, 195, 80, 0, 0, 0, 0, 0, 0}},
    {50400, {64, 14, 196, 224, 0, 0, 0, 0, 0, 0}},
    {88200, {64, 15, 172, 68, 0, 0, 0, 0, 0, 0}},
    {96000, {64, 15, 187, 128, 0, 0, 0, 0, 0, 0}},
    {176400, {64, 16, 172, 68, 0, 0, 0, 0, 0, 0}},
    {192000, {64, 16, 187, 128, 0, 0, 0, 0, 0, 0}},
    {352800, {64, 17, 172, 68, 0, 0, 0, 0, 0, 0}},
    {2822400, {64, 20, 172, 68, 0, 0, 0, 0, 0, 0}},
    {5644800, {64, 21, 172, 68, 0, 0, 0, 0, 0, 0}}
};

//=============================================================
/* IMPLEMENTATION */
//=============================================================

//=============================================================
template <class T>
AudioFile<T>::AudioFile()
{
    bitDepth = 16;
    sampleRate = 44100;
    samples.resize (1);
    samples[0].resize (0);
    audioFileFormat = AudioFileFormat::NotLoaded;
}

//=============================================================
template <class T>
uint32_t AudioFile<T>::getSampleRate() const
{
    return sampleRate;
}

//=============================================================
template <class T>
int AudioFile<T>::getNumChannels() const
{
    return (int)samples.size();
}

//=============================================================
template <class T>
bool AudioFile<T>::isMono() const
{
    return getNumChannels() == 1;
}

//=============================================================
template <class T>
bool AudioFile<T>::isStereo() const
{
    return getNumChannels() == 2;
}

//=============================================================
template <class T>
int AudioFile<T>::getBitDepth() const
{
    return bitDepth;
}

//=============================================================
template <class T>
int AudioFile<T>::getNumSamplesPerChannel() const
{
    if (samples.size() > 0)
        return (int) samples[0].size();
    else
        return 0;
}

//=============================================================
template <class T>
double AudioFile<T>::getLengthInSeconds() const
{
    return (double)getNumSamplesPerChannel() / (double)sampleRate;
}

//=============================================================
template <class T>
void AudioFile<T>::printSummary() const
{
    // std::cout << "|======================================|" << std::endl;
    // std::cout << "Num Channels: " << getNumChannels() << std::endl;
    // std::cout << "Num Samples Per Channel: " << getNumSamplesPerChannel() << std::endl;
    // std::cout << "Sample Rate: " << sampleRate << std::endl;
    // std::cout << "Bit Depth: " << bitDepth << std::endl;
    // std::cout << "Length in Seconds: " << getLengthInSeconds() << std::endl;
    // std::cout << "|======================================|" << std::endl;
}

//=============================================================
template <class T>
bool AudioFile<T>::setAudioBuffer (AudioBuffer& newBuffer)
{
    int numChannels = (int)newBuffer.size();
    
    if (numChannels <= 0)
    {
        assert (false && "The buffer your are trying to use has no channels");
        return false;
    }
    
    int numSamples = (int)newBuffer[0].size();
    
    // set the number of channels
    samples.resize (newBuffer.size());
    
    for (int k = 0; k < getNumChannels(); k++)
    {
        assert (newBuffer[k].size() == numSamples);
        
        samples[k].resize (numSamples);
        
        for (int i = 0; i < numSamples; i++)
        {
            samples[k][i] = newBuffer[k][i];
        }
    }
    
    return true;
}

//=============================================================
template <class T>
void AudioFile<T>::setAudioBufferSize (int numChannels, int numSamples)
{
    samples.resize (numChannels);
    setNumSamplesPerChannel (numSamples);
}

//=============================================================
template <class T>
void AudioFile<T>::setNumSamplesPerChannel (int numSamples)
{
    int originalSize = getNumSamplesPerChannel();
    
    for (int i = 0; i < getNumChannels();i++)
    {
        samples[i].resize (numSamples);
        
        // set any new samples to zero
        if (numSamples > originalSize)
            std::fill (samples[i].begin() + originalSize, samples[i].end(), (T)0.);
    }
}

//=============================================================
template <class T>
void AudioFile<T>::setNumChannels (int numChannels)
{
    int originalNumChannels = getNumChannels();
    int originalNumSamplesPerChannel = getNumSamplesPerChannel();
    
    samples.resize (numChannels);
    
    // make sure any new channels are set to the right size
    // and filled with zeros
    if (numChannels > originalNumChannels)
    {
        for (int i = originalNumChannels; i < numChannels; i++)
        {
            samples[i].resize (originalNumSamplesPerChannel);
            std::fill (samples[i].begin(), samples[i].end(), (T)0.);
        }
    }
}

//=============================================================
template <class T>
void AudioFile<T>::setBitDepth (int numBitsPerSample)
{
    bitDepth = numBitsPerSample;
}

//=============================================================
template <class T>
void AudioFile<T>::setSampleRate (uint32_t newSampleRate)
{
    sampleRate = newSampleRate;
}

//=============================================================
template <class T>
bool AudioFile<T>::load (std::string filePath, uint32_t targetSampleRate)
{
    // Open the file using C-style file I/O
    FILE* file = fopen(filePath.c_str(), "rb");
    
    // check the file exists
    if (!file)
    {
        // std::cout << "ERROR: File doesn't exist or otherwise can't load file" << std::endl;
        // std::cout << filePath << std::endl;
        return false;
    }

    printf("filePath: in audiofile %s\n", filePath.c_str());
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Early validation - avoid allocating for very small files that can't be valid audio
    if (fileSize < 44) {  // Minimum WAV header size
        fclose(file);
        return false;
    }
    
    // Read file data into vector
    std::vector<uint8_t> fileData(fileSize);
    size_t bytesRead = fread(fileData.data(), 1, fileSize, file);
    fclose(file);
    
    if (bytesRead != fileSize)
    {
        return false;
    }
    
    // get audio file format
    audioFileFormat = determineAudioFileFormat (fileData);

    printf("audioFileFormat: %d\n", audioFileFormat);
    
    bool result = false;
    if (audioFileFormat == AudioFileFormat::Wave)
    {
        printf("decodeWaveFile\n");
        result = decodeWaveFile (fileData);
    }
    else if (audioFileFormat == AudioFileFormat::Aiff)
    {
        result = decodeAiffFile (fileData);
    }
    else
    {
        printf("Audio File Type: Error\n");
        // std::cout << "Audio File Type: " << "Error" << std::endl;
    }
    
    // Free the file data memory once we're done with it
    std::vector<uint8_t>().swap(fileData);  // Force deallocation by swapping with empty vector
    
    // If successfully loaded and a target sample rate is specified, resample immediately
    if (result && targetSampleRate > 0 && targetSampleRate != sampleRate)
    {
        result = resampleToTargetRate(targetSampleRate);
    }
    
    return result;
}

//=============================================================
template <class T>
bool AudioFile<T>::decodeWaveFile (std::vector<uint8_t>& fileData)
{
    // -----------------------------------------------------------
    // HEADER CHUNK
    std::string headerChunkID (fileData.begin(), fileData.begin() + 4);
    std::string format (fileData.begin() + 8, fileData.begin() + 12);
    
    // -----------------------------------------------------------
    // try and find the start points of key chunks
    int indexOfDataChunk = getIndexOfString (fileData, "data");
    int indexOfFormatChunk = getIndexOfString (fileData, "fmt");
    
    // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
    // then it is unlikely we'll able to read this file, so abort
    if (indexOfDataChunk == -1 || indexOfFormatChunk == -1 || headerChunkID != "RIFF" || format != "WAVE")
    {
        printf("ERROR: this doesn't seem to be a valid .WAV file\n");
        return false;
    }
    
    // -----------------------------------------------------------
    // FORMAT CHUNK
    int f = indexOfFormatChunk;
    int16_t audioFormat = twoBytesToInt (fileData, f + 8);
    int16_t numChannels = twoBytesToInt (fileData, f + 10);
    sampleRate = (uint32_t) fourBytesToInt (fileData, f + 12);
    int32_t numBytesPerSecond = fourBytesToInt (fileData, f + 16);
    int16_t numBytesPerBlock = twoBytesToInt (fileData, f + 20);
    bitDepth = (int) twoBytesToInt (fileData, f + 22);
    
    int numBytesPerSample = bitDepth / 8;
    
    // Perform validation checks
    // check that the audio format is PCM
    if (audioFormat != 1)
    {
        printf("ERROR: this is a compressed .WAV file and this library does not support decoding them at present\n");
        return false;
    }
    
    // check the number of channels is mono or stereo
    if (numChannels < 1 ||numChannels > 2)
    {
        printf("ERROR: this WAV file seems to be neither mono nor stereo (perhaps multi-track, or corrupted?)\n");
        return false;
    }
    
    // check header data is consistent
    if ((numBytesPerSecond != (numChannels * sampleRate * bitDepth) / 8) || (numBytesPerBlock != (numChannels * numBytesPerSample)))
    {
        printf("ERROR: the header data in this WAV file seems to be inconsistent\n");
        return false;
    }
    
    // check bit depth is either 8, 16 or 24 bit
    if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24)
    {
        printf("ERROR: this file has a bit depth that is not 8, 16 or 24 bits\n");
        return false;
    }
    
    // -----------------------------------------------------------
    // DATA CHUNK
    int d = indexOfDataChunk;
    int32_t dataChunkSize = fourBytesToInt (fileData, d + 4);
    
    int numSamples = dataChunkSize / (numChannels * bitDepth / 8);
    int samplesStartIndex = indexOfDataChunk + 8;
    
    // Clear any existing data and pre-allocate memory for efficiency
    clearAudioBuffer();
    samples.resize(numChannels);
    
    // Pre-allocate each channel's sample vector to avoid expensive reallocations
    for (int channel = 0; channel < numChannels; channel++) {
        samples[channel].reserve(numSamples);
    }
    
    for (int i = 0; i < numSamples; i++)
    {
        for (int channel = 0; channel < numChannels; channel++)
        {
            int sampleIndex = samplesStartIndex + (numBytesPerBlock * i) + channel * numBytesPerSample;
            
            if (bitDepth == 8)
            {
                T sample = singleByteToSample (fileData[sampleIndex]);
                samples[channel].push_back (sample);
            }
            else if (bitDepth == 16)
            {
                int16_t sampleAsInt = twoBytesToInt (fileData, sampleIndex);
                T sample = sixteenBitIntToSample (sampleAsInt);
                samples[channel].push_back (sample);
            }
            else if (bitDepth == 24)
            {
                int32_t sampleAsInt = 0;
                sampleAsInt = (fileData[sampleIndex + 2] << 16) | (fileData[sampleIndex + 1] << 8) | fileData[sampleIndex];
                
                if (sampleAsInt & 0x800000) //  if the 24th bit is set, this is a negative number in 24-bit world
                    sampleAsInt = sampleAsInt | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float

                T sample = (T)sampleAsInt / (T)8388608.;
                samples[channel].push_back (sample);
            }
            else
            {
                assert (false);
            }
        }
    }

    return true;
}

//=============================================================
template <class T>
bool AudioFile<T>::decodeAiffFile (std::vector<uint8_t>& fileData)
{
    // -----------------------------------------------------------
    // HEADER CHUNK
    std::string headerChunkID (fileData.begin(), fileData.begin() + 4);
    std::string format (fileData.begin() + 8, fileData.begin() + 12);
    
    // -----------------------------------------------------------
    // try and find the start points of key chunks
    int indexOfCommChunk = getIndexOfString (fileData, "COMM");
    int indexOfSoundDataChunk = getIndexOfString (fileData, "SSND");
    
    // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
    // then it is unlikely we'll able to read this file, so abort
    if (indexOfSoundDataChunk == -1 || indexOfCommChunk == -1 || headerChunkID != "FORM" || format != "AIFF")
    {
        // std::cout << "ERROR: this doesn't seem to be a valid AIFF file" << std::endl;
        return false;
    }

    // -----------------------------------------------------------
    // COMM CHUNK
    int p = indexOfCommChunk;
    int16_t numChannels = twoBytesToInt (fileData, p + 8, Endianness::BigEndian);
    int32_t numSamplesPerChannel = fourBytesToInt (fileData, p + 10, Endianness::BigEndian);
    bitDepth = (int) twoBytesToInt (fileData, p + 14, Endianness::BigEndian);
    sampleRate = getAiffSampleRate (fileData, p + 16);
    
    // check the sample rate was properly decoded
    if (sampleRate == 0)
    {
        // std::cout << "ERROR: this AIFF file has an unsupported sample rate" << std::endl;
        return false;
    }
    
    // check the number of channels is mono or stereo
    if (numChannels < 1 ||numChannels > 2)
    {
        // std::cout << "ERROR: this AIFF file seems to be neither mono nor stereo (perhaps multi-track, or corrupted?)" << std::endl;
        return false;
    }
    
    // check bit depth is either 8, 16 or 24 bit
    if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24)
    {
        // std::cout << "ERROR: this file has a bit depth that is not 8, 16 or 24 bits" << std::endl;
        return false;
    }
    
    // -----------------------------------------------------------
    // SSND CHUNK
    int s = indexOfSoundDataChunk;
    int32_t soundDataChunkSize = fourBytesToInt (fileData, s + 4, Endianness::BigEndian);
    int32_t offset = fourBytesToInt (fileData, s + 8, Endianness::BigEndian);
    
    int numBytesPerSample = bitDepth / 8;
    int numBytesPerFrame = numBytesPerSample * numChannels;
    int totalNumAudioSampleBytes = numSamplesPerChannel * numBytesPerFrame;
    int samplesStartIndex = s + 16 + (int)offset;
        
    // sanity check the data
    if ((soundDataChunkSize - 8) != totalNumAudioSampleBytes || totalNumAudioSampleBytes > (fileData.size() - samplesStartIndex))
    {
        // std::cout << "ERROR: the metadatafor this file doesn't seem right" << std::endl;
        return false;
    }
    
    // Clear existing data and pre-allocate for efficiency
    clearAudioBuffer();
    samples.resize(numChannels);
    
    // Pre-allocate each channel's vector to avoid reallocation
    for (int channel = 0; channel < numChannels; channel++) {
        samples[channel].reserve(numSamplesPerChannel);
    }
    
    for (int i = 0; i < numSamplesPerChannel; i++)
    {
        for (int channel = 0; channel < numChannels; channel++)
        {
            int sampleIndex = samplesStartIndex + (numBytesPerFrame * i) + channel * numBytesPerSample;
            
            if (bitDepth == 8)
            {
                int8_t sampleAsSigned8Bit = (int8_t)fileData[sampleIndex];
                T sample = (T)sampleAsSigned8Bit / (T)128.;
                samples[channel].push_back (sample);
            }
            else if (bitDepth == 16)
            {
                int16_t sampleAsInt = twoBytesToInt (fileData, sampleIndex, Endianness::BigEndian);
                T sample = sixteenBitIntToSample (sampleAsInt);
                samples[channel].push_back (sample);
            }
            else if (bitDepth == 24)
            {
                int32_t sampleAsInt = 0;
                sampleAsInt = (fileData[sampleIndex] << 16) | (fileData[sampleIndex + 1] << 8) | fileData[sampleIndex + 2];
                
                if (sampleAsInt & 0x800000) //  if the 24th bit is set, this is a negative number in 24-bit world
                    sampleAsInt = sampleAsInt | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float
                
                T sample = (T)sampleAsInt / (T)8388608.;
                samples[channel].push_back (sample);
            }
            else
            {
                assert (false);
            }
        }
    }
    
    return true;
}

//=============================================================
template <class T>
uint32_t AudioFile<T>::getAiffSampleRate (std::vector<uint8_t>& fileData, int sampleRateStartIndex)
{
    for (auto it : aiffSampleRateTable)
    {
        if (tenByteMatch (fileData, sampleRateStartIndex, it.second, 0))
            return it.first;
    }
    
    return 0;
}

//=============================================================
template <class T>
bool AudioFile<T>::tenByteMatch (std::vector<uint8_t>& v1, int startIndex1, std::vector<uint8_t>& v2, int startIndex2)
{
    for (int i = 0; i < 10; i++)
    {
        if (v1[startIndex1 + i] != v2[startIndex2 + i])
            return false;
    }
    
    return true;
}

//=============================================================
template <class T>
void AudioFile<T>::addSampleRateToAiffData (std::vector<uint8_t>& fileData, uint32_t sampleRate)
{
    if (aiffSampleRateTable.count (sampleRate) > 0)
    {
        for (int i = 0; i < 10; i++)
            fileData.push_back (aiffSampleRateTable[sampleRate][i]);
    }
}

//=============================================================
template <class T>
bool AudioFile<T>::save (std::string filePath, AudioFileFormat format)
{
    if (format == AudioFileFormat::Wave)
    {
        return saveToWaveFile (filePath);
    }
    else if (format == AudioFileFormat::Aiff)
    {
        return saveToAiffFile (filePath);
    }
    
    return false;
}

//=============================================================
template <class T>
bool AudioFile<T>::saveToWaveFile (std::string filePath)
{
    std::vector<uint8_t> fileData;
    
    int32_t dataChunkSize = getNumSamplesPerChannel() * (getNumChannels() * bitDepth / 8);
    
    // -----------------------------------------------------------
    // HEADER CHUNK
    addStringToFileData (fileData, "RIFF");
    
    // The file size in bytes is the header chunk size (4, not counting RIFF and WAVE) + the format
    // chunk size (24) + the metadata part of the data chunk plus the actual data chunk size
    int32_t fileSizeInBytes = 4 + 24 + 8 + dataChunkSize;
    addInt32ToFileData (fileData, fileSizeInBytes);
    
    addStringToFileData (fileData, "WAVE");
    
    // -----------------------------------------------------------
    // FORMAT CHUNK
    addStringToFileData (fileData, "fmt ");
    addInt32ToFileData (fileData, 16); // format chunk size (16 for PCM)
    addInt16ToFileData (fileData, 1); // audio format = 1
    addInt16ToFileData (fileData, (int16_t)getNumChannels()); // num channels
    addInt32ToFileData (fileData, (int32_t)sampleRate); // sample rate
    
    int32_t numBytesPerSecond = (int32_t) ((getNumChannels() * sampleRate * bitDepth) / 8);
    addInt32ToFileData (fileData, numBytesPerSecond);
    
    int16_t numBytesPerBlock = getNumChannels() * (bitDepth / 8);
    addInt16ToFileData (fileData, numBytesPerBlock);
    
    addInt16ToFileData (fileData, (int16_t)bitDepth);
    
    // -----------------------------------------------------------
    // DATA CHUNK
    addStringToFileData (fileData, "data");
    addInt32ToFileData (fileData, dataChunkSize);
    
    // Pre-allocate the full size needed for the file data to avoid reallocations
    fileData.reserve(fileData.size() + dataChunkSize);
    
    // Optimize for different bit depths
    if (bitDepth == 8)
    {
        // For 8-bit, process all samples in one go
        for (int i = 0; i < getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < getNumChannels(); channel++)
            {
                uint8_t byte = sampleToSingleByte (samples[channel][i]);
                fileData.push_back (byte);
            }
        }
    }
    else if (bitDepth == 16)
    {
        // For 16-bit, process all samples in one go
        for (int i = 0; i < getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < getNumChannels(); channel++)
            {
                int16_t sampleAsInt = sampleToSixteenBitInt (samples[channel][i]);
                addInt16ToFileData (fileData, sampleAsInt);
            }
        }
    }
    else if (bitDepth == 24)
    {
        // For 24-bit, process all samples in one go
        for (int i = 0; i < getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < getNumChannels(); channel++)
            {
                int32_t sampleAsIntAgain = (int32_t) (samples[channel][i] * (T)8388608.);
                
                uint8_t bytes[3];
                bytes[2] = (uint8_t) (sampleAsIntAgain >> 16) & 0xFF;
                bytes[1] = (uint8_t) (sampleAsIntAgain >>  8) & 0xFF;
                bytes[0] = (uint8_t) sampleAsIntAgain & 0xFF;
                
                fileData.push_back (bytes[0]);
                fileData.push_back (bytes[1]);
                fileData.push_back (bytes[2]);
            }
        }
    }
    else
    {
        assert (false && "Trying to write a file with unsupported bit depth");
        return false;
    }
    
    // check that the various sizes we put in the metadata are correct
    if (fileSizeInBytes != (fileData.size() - 8) || dataChunkSize != (getNumSamplesPerChannel() * getNumChannels() * (bitDepth / 8)))
    {
        // std::cout << "ERROR: couldn't save file to " << filePath << std::endl;
        return false;
    }
    
    // try to write the file
    bool result = writeDataToFile (fileData, filePath);
    
    // Free up the memory used by fileData
    std::vector<uint8_t>().swap(fileData);
    
    return result;
}

//=============================================================
template <class T>
bool AudioFile<T>::saveToAiffFile (std::string filePath)
{
    std::vector<uint8_t> fileData;
    
    int32_t numBytesPerSample = bitDepth / 8;
    int32_t numBytesPerFrame = numBytesPerSample * getNumChannels();
    int32_t totalNumAudioSampleBytes = getNumSamplesPerChannel() * numBytesPerFrame;
    int32_t soundDataChunkSize = totalNumAudioSampleBytes + 8;
    
    // Pre-allocate a reasonable size for the header data to minimize reallocations
    fileData.reserve(64 + totalNumAudioSampleBytes);
    
    // -----------------------------------------------------------
    // HEADER CHUNK
    addStringToFileData (fileData, "FORM");
    
    // The file size in bytes is the header chunk size (4, not counting FORM and AIFF) + the COMM
    // chunk size (26) + the metadata part of the SSND chunk plus the actual data chunk size
    int32_t fileSizeInBytes = 4 + 26 + 16 + totalNumAudioSampleBytes;
    addInt32ToFileData (fileData, fileSizeInBytes, Endianness::BigEndian);
    
    addStringToFileData (fileData, "AIFF");
    
    // -----------------------------------------------------------
    // COMM CHUNK
    addStringToFileData (fileData, "COMM");
    addInt32ToFileData (fileData, 18, Endianness::BigEndian); // commChunkSize
    addInt16ToFileData (fileData, getNumChannels(), Endianness::BigEndian); // num channels
    addInt32ToFileData (fileData, getNumSamplesPerChannel(), Endianness::BigEndian); // num samples per channel
    addInt16ToFileData (fileData, bitDepth, Endianness::BigEndian); // bit depth
    addSampleRateToAiffData (fileData, sampleRate);
    
    // -----------------------------------------------------------
    // SSND CHUNK
    addStringToFileData (fileData, "SSND");
    addInt32ToFileData (fileData, soundDataChunkSize, Endianness::BigEndian);
    addInt32ToFileData (fileData, 0, Endianness::BigEndian); // offset
    addInt32ToFileData (fileData, 0, Endianness::BigEndian); // block size
    
    // Optimize for different bit depths
    if (bitDepth == 8)
    {
        // For 8-bit, process all samples in one go
        for (int i = 0; i < getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < getNumChannels(); channel++)
            {
                uint8_t byte = (uint8_t)((int8_t)(samples[channel][i] * 128.0));
                fileData.push_back(byte);
            }
        }
    }
    else if (bitDepth == 16)
    {
        // For 16-bit, process all samples in one go
        for (int i = 0; i < getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < getNumChannels(); channel++)
            {
                int16_t sampleAsInt = sampleToSixteenBitInt(samples[channel][i]);
                addInt16ToFileData(fileData, sampleAsInt, Endianness::BigEndian);
            }
        }
    }
    else if (bitDepth == 24)
    {
        // For 24-bit, process all samples in one go
        for (int i = 0; i < getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < getNumChannels(); channel++)
            {
                int32_t sampleAsIntAgain = (int32_t)(samples[channel][i] * (T)8388608.);
                
                uint8_t bytes[3];
                bytes[0] = (uint8_t)(sampleAsIntAgain >> 16) & 0xFF;
                bytes[1] = (uint8_t)(sampleAsIntAgain >> 8) & 0xFF;
                bytes[2] = (uint8_t)sampleAsIntAgain & 0xFF;
                
                fileData.push_back(bytes[0]);
                fileData.push_back(bytes[1]);
                fileData.push_back(bytes[2]);
            }
        }
    }
    else
    {
        assert(false && "Trying to write a file with unsupported bit depth");
        return false;
    }
    
    // check that the various sizes we put in the metadata are correct
    if (fileSizeInBytes != (fileData.size() - 8) || soundDataChunkSize != getNumSamplesPerChannel() * numBytesPerFrame + 8)
    {
        // std::cout << "ERROR: couldn't save file to " << filePath << std::endl;
        return false;
    }
    
    // try to write the file
    bool result = writeDataToFile(fileData, filePath);
    
    // Free up the memory used by fileData
    std::vector<uint8_t>().swap(fileData);
    
    return result;
}

//=============================================================
template <class T>
bool AudioFile<T>::writeDataToFile (std::vector<uint8_t>& fileData, std::string filePath)
{
    // Open file using C-style file I/O
    FILE* outputFile = fopen(filePath.c_str(), "wb");
    
    if (outputFile)
    {
        // Write all data at once to minimize I/O operations
        size_t bytesWritten = fwrite(fileData.data(), 1, fileData.size(), outputFile);
        fclose(outputFile);
        
        return (bytesWritten == fileData.size());
    }
    
    return false;
}

//=============================================================
template <class T>
void AudioFile<T>::addStringToFileData (std::vector<uint8_t>& fileData, std::string s)
{
    for (int i = 0; i < s.length();i++)
        fileData.push_back ((uint8_t) s[i]);
}

//=============================================================
template <class T>
void AudioFile<T>::addInt32ToFileData (std::vector<uint8_t>& fileData, int32_t i, Endianness endianness)
{
    uint8_t bytes[4];
    
    if (endianness == Endianness::LittleEndian)
    {
        bytes[3] = (i >> 24) & 0xFF;
        bytes[2] = (i >> 16) & 0xFF;
        bytes[1] = (i >> 8) & 0xFF;
        bytes[0] = i & 0xFF;
    }
    else
    {
        bytes[0] = (i >> 24) & 0xFF;
        bytes[1] = (i >> 16) & 0xFF;
        bytes[2] = (i >> 8) & 0xFF;
        bytes[3] = i & 0xFF;
    }
    
    for (int i = 0; i < 4; i++)
        fileData.push_back (bytes[i]);
}

//=============================================================
template <class T>
void AudioFile<T>::addInt16ToFileData (std::vector<uint8_t>& fileData, int16_t i, Endianness endianness)
{
    uint8_t bytes[2];
    
    if (endianness == Endianness::LittleEndian)
    {
        bytes[1] = (i >> 8) & 0xFF;
        bytes[0] = i & 0xFF;
    }
    else
    {
        bytes[0] = (i >> 8) & 0xFF;
        bytes[1] = i & 0xFF;
    }
    
    fileData.push_back (bytes[0]);
    fileData.push_back (bytes[1]);
}

//=============================================================
template <class T>
void AudioFile<T>::clearAudioBuffer()
{
    for (int i = 0; i < samples.size();i++)
    {
        samples[i].clear();
    }
    
    samples.clear();
}

//=============================================================
template <class T>
AudioFileFormat AudioFile<T>::determineAudioFileFormat (std::vector<uint8_t>& fileData)
{
    std::string header (fileData.begin(), fileData.begin() + 4);
    
    if (header == "RIFF")
        return AudioFileFormat::Wave;
    else if (header == "FORM")
        return AudioFileFormat::Aiff;
    else
        return AudioFileFormat::Error;
}

//=============================================================
template <class T>
int32_t AudioFile<T>::fourBytesToInt (std::vector<uint8_t>& source, int startIndex, Endianness endianness)
{
    int32_t result;
    
    if (endianness == Endianness::LittleEndian)
        result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8) | source[startIndex];
    else
        result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) | (source[startIndex + 2] << 8) | source[startIndex + 3];
    
    return result;
}

//=============================================================
template <class T>
int16_t AudioFile<T>::twoBytesToInt (std::vector<uint8_t>& source, int startIndex, Endianness endianness)
{
    int16_t result;
    
    if (endianness == Endianness::LittleEndian)
        result = (source[startIndex + 1] << 8) | source[startIndex];
    else
        result = (source[startIndex] << 8) | source[startIndex + 1];
    
    return result;
}

//=============================================================
template <class T>
int AudioFile<T>::getIndexOfString (std::vector<uint8_t>& source, std::string stringToSearchFor)
{
    int index = -1;
    int stringLength = (int)stringToSearchFor.length();
    
    for (int i = 0; i < source.size() - stringLength;i++)
    {
        std::string section (source.begin() + i, source.begin() + i + stringLength);
        
        if (section == stringToSearchFor)
        {
            index = i;
            break;
        }
    }
    
    return index;
}

//=============================================================
template <class T>
T AudioFile<T>::sixteenBitIntToSample (int16_t sample)
{
    return static_cast<T> (sample) / static_cast<T> (32768.);
}

//=============================================================
template <class T>
int16_t AudioFile<T>::sampleToSixteenBitInt (T sample)
{
    sample = clamp (sample, -1., 1.);
    return static_cast<int16_t> (sample * 32767.);
}

//=============================================================
template <class T>
uint8_t AudioFile<T>::sampleToSingleByte (T sample)
{
    sample = clamp (sample, -1., 1.);
    sample = (sample + 1.) / 2.;
    return static_cast<uint8_t> (sample * 255.);
}

//=============================================================
template <class T>
T AudioFile<T>::singleByteToSample (uint8_t sample)
{
    return static_cast<T> (sample - 128) / static_cast<T> (128.);
}

//=============================================================
template <class T>
T AudioFile<T>::clamp (T value, T minValue, T maxValue)
{
    value = std::min (value, maxValue);
    value = std::max (value, minValue);
    return value;
}

//=============================================================
template <class T>
bool AudioFile<T>::resampleToTargetRate(uint32_t targetSampleRate)
{
    // If already at target rate or no data to resample, do nothing
    if (sampleRate == targetSampleRate || getNumSamplesPerChannel() == 0)
        return true;
            
    // We would implement the resampler here, but since this is a stub for demonstration, 
    // let's assume we have the same SampleRateConverter class from waves.cpp
    
    // The implementation would be similar to this pseudocode:
    // 1. For each channel:
    //    a. Create a new buffer for resampled data
    //    b. Use the SampleRateConverter to convert the data
    //    c. Replace the original channel data with the resampled data
    // 2. Update the sample rate
    
    // As a placeholder for this example, we'll resize the buffer by the ratio of rates
    // In a real implementation, use a proper resampling algorithm
    double rateRatio = (double)targetSampleRate / (double)sampleRate;
    int newNumSamples = (int)(getNumSamplesPerChannel() * rateRatio);
    
    // Create temporary buffers for each channel 
    std::vector<std::vector<T>> newSamples(getNumChannels());
    
    // Pre-allocate the right size for each channel to avoid reallocations
    for (int channel = 0; channel < getNumChannels(); channel++) {
        newSamples[channel].resize(newNumSamples, 0);
        
        // Simple linear interpolation (just for demonstration - not high quality)
        // In a real implementation, use a proper resampling algorithm
        for (int i = 0; i < newNumSamples; i++) {
            double sourceIndex = i / rateRatio;
            int sourceIndex1 = (int)sourceIndex;
            int sourceIndex2 = std::min(sourceIndex1 + 1, getNumSamplesPerChannel() - 1);
            double alpha = sourceIndex - sourceIndex1;
            
            // Linear interpolation
            newSamples[channel][i] = (T)((1.0 - alpha) * samples[channel][sourceIndex1] + 
                                         alpha * samples[channel][sourceIndex2]);
        }
    }
    
    // Replace the samples with the resampled data
    for (int channel = 0; channel < getNumChannels(); channel++) {
        samples[channel] = std::move(newSamples[channel]); // Use move for efficiency
    }
    
    // Update the sample rate
    sampleRate = targetSampleRate;
    
    return true;
}

#endif /* AudioFile_h */
