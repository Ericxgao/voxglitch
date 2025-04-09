struct MenuItemLoadBank : MenuItem
{
  WavBank *module;

  void onAction(const event::Action &e) override
  {
    #if defined(USING_CARDINAL_NOT_RACK) || defined(METAMODULE)
        WavBank *module = this->module;
        async_dialog_filebrowser(false, NULL, module->samples_root_dir.c_str(), "Load sample", [module](char *path) {
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
        pathSelected(module, module->selectPathVCV());
    #endif
  }

  static void pathSelected(WavBank *module, std::string path)
  {
    if (path != "")
    {
      module->load_samples_from_path(path);
      module->path = path;
      module->setRoot(path);
    }
  }
};
