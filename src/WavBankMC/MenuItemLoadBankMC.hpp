struct MenuItemLoadBankMC : MenuItem
{
	WavBankMC *module;

	void onAction(const event::Action &e) override
	{
	#if defined(USING_CARDINAL_NOT_RACK) || defined(METAMODULE)
		WavBankMC *module = this->module;
		async_dialog_filebrowser(false, NULL, module->path.c_str(), text.c_str(), [module](char* path) {
			if (path) {
				char *last_slash = strrchr(path, '/');
				if (last_slash) {
					*last_slash = '\0';
				}

				printf("path: %s\n", path);
				pathSelected(module, path);
				free(path);
			}
		});
	#else
	pathSelected(module, module->selectPathVCV());
	#endif
	}

	static void pathSelected(WavBankMC *module, std::string path)
	{
		if (path != "")
		{
			module->load_samples_from_path(path);
			module->path = path;
			module->selected_sample_slot = 0;
			module->setRoot(path);
		}
	}
};
