struct GrainFxCore
{
    Grain grain_array[MAX_GRAINS + 1];
    Grain grain_array_tmp[MAX_GRAINS + 1];
    unsigned int grain_array_length = 0;
    Common *common;

    GrainFxCore()
    {
    }

    virtual ~GrainFxCore() {
    }

    // Return number of active grains
    virtual int size()
    {
        return(grain_array_length);
    }

    virtual bool isEmpty()
    {
        return(grain_array_length == 0);
    }

    virtual void add(double start_position, unsigned int lifespan, double pan, AudioBuffer *buffer_ptr, unsigned int max_grains, double pitch)
    {
        if(grain_array_length >= max_grains) return;
        if(lifespan == 0) return;

        Grain grain;

        // Configure grain for playback
        grain.start_position = start_position;
        grain.buffer_ptr = buffer_ptr;
        grain.lifespan = lifespan;
        grain.age = lifespan;
        grain.pan = pan;
        grain.pitch = pitch;
        grain.common = common;

        grain_array[grain_array_length] = grain;
        grain_array_length++;
    }

    virtual std::pair<float, float> process(float smooth_rate, unsigned int contour_selection)
    {
        float left_mix_output = 0;
        float right_mix_output = 0;
        unsigned int active_grains = 0;

        //
        // Process grains
        // ---------------------------------------------------------------------

        for (unsigned int i=0; i < grain_array_length; i++)
        {
            if(grain_array[i].erase_me == false)
            {
                std::pair<float, float> stereo_output = grain_array[i].getStereoOutput(contour_selection);
                left_mix_output  += stereo_output.first;
                right_mix_output += stereo_output.second;

                grain_array[i].step();
                
                // If grain is still active after stepping, keep it in the array
                if(grain_array[i].erase_me == false)
                {
                    // Only move the grain if it needs to be moved
                    if(active_grains != i)
                    {
                        grain_array[active_grains] = grain_array[i];
                    }
                    active_grains++;
                }
            }
        }

        grain_array_length = active_grains;

        return {left_mix_output, right_mix_output};
    }

};
