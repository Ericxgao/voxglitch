struct GhostsLoadSample : MenuItem
{
	Ghosts *module;

  void onAction(const event::Action &e) override
	{
    #if defined(USING_CARDINAL_NOT_RACK) || defined(METAMODULE)
    	Ghosts *module = this->module;
    	async_dialog_filebrowser(false, NULL, module->samples_root_dir.c_str(), "Load sample", [module](char* filename) {
        if(filename)
        {
			printf("filename: %s\n", filename);
          fileSelected(module, std::string(filename));
          free(filename);
        }
    	});
    #else
      fileSelected(module, module->selectFileVCV());
    #endif
	}

	static void fileSelected(Ghosts *module, std::string filename)
	{
    if (filename != "")
		{
			module->sample.load(filename);
			printf("module->sample.sample_rate: %f\n", module->sample.sample_rate);
      		module->sample_rate_division = module->sample.sample_rate / APP->engine->getSampleRate();
			module->loaded_filename = module->sample.filename;
			module->setRoot(filename);
		}
	}
};
