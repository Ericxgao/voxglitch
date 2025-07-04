struct AutobreakStudioLoadSample : MenuItem
{
	AutobreakStudio *module;
	int sample_number;

	void step() override
	{
		text = std::to_string(sample_number + 1) + ": " + module->loaded_filenames[sample_number];
	}

	void onAction(const event::Action &e) override
	{
#if defined(USING_CARDINAL_NOT_RACK) || defined(METAMODULE)
		AutobreakStudio *module = this->module;
		unsigned int sample_number = this->sample_number;
		async_dialog_filebrowser(false, NULL, NULL, "Load sample", [module, sample_number](char *filename) {
          if(filename)
          {
            fileSelected(module, sample_number, std::string(filename));
            free(filename);
          }
		});
#else
		fileSelected(module, this->sample_number, module->selectFileVCV());
#endif
	}

	static void fileSelected(AutobreakStudio *module, unsigned int sample_number, std::string filename)
	{
		if (filename != "")
		{
			module->samples[sample_number].load(filename);
			module->loaded_filenames[sample_number] = module->samples[sample_number].filename;
			module->setRoot(filename);
		}
	}
};
