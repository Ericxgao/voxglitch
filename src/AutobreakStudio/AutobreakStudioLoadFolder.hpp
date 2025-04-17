struct AutobreakStudioLoadFolder : MenuItem
{
    AutobreakStudio *module;
    std::string root_dir;

    void onAction(const event::Action &e) override
    {
        const std::string dir = root_dir.empty() ? "" : root_dir;
#if defined(USING_CARDINAL_NOT_RACK) || defined(METAMODULE)
        AutobreakStudio *module = this->module;
        async_dialog_filebrowser(false, NULL, module->samples_root_dir.c_str(), "Load folder", [module](char *path) {
          if (path) {
            // Remove the last chunk after the last slash but keep the slash
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
        char *path = osdialog_file(OSDIALOG_OPEN_DIR, dir.c_str(), NULL, NULL);
        pathSelected(module, path);
#endif
    }

    static void pathSelected(AutobreakStudio *module, char *path)
    {
        if (path)
        {
            std::vector<std::string> dirList = system::getEntries(path);
            unsigned int i = 0;

            for (auto filename : dirList)
            {
                if (
                    // Something happened in Rack 2 where the extension started to include
                    // the ".", so I decided to check for both versions, just in case.
                    (rack::string::lowercase(system::getExtension(filename)) == "wav") ||
                    (rack::string::lowercase(system::getExtension(filename)) == ".wav"))
                {
                    if (i < 8)
                    {
                        module->samples[i].load(filename);
                        module->loaded_filenames[i] = module->samples[i].filename;
                        module->setRoot(filename);
                        i++;
                    }
                }
            }

            free(path);
        }
    }
};
