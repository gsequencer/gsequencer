#include <stdio.h>

#include <dlfcn.h>

int
main(int argc, char **argv)
{
  void *plugin_so;
  void *ladspa_descriptor; // LADSPA_Descriptor_Function
  
  char *plugin_filename;
  
  static char *default_plugin_filename = "gsequencer-macos/build/universal/com.gsequencer.GSequencer.app/Contents/Plugins/lv2/calf.lv2/calf.dylib";
  
  plugin_filename = NULL;

  if(argc > 1){
    plugin_filename = argv[1];
  }else{
    return(-1);
  }
  
  plugin_so = dlopen(plugin_filename,
		     (RTLD_NOW | RTLD_GLOBAL));

  if(plugin_so == NULL){
    fprintf(stdout, "plugin_so = NULL\n");

    fprintf(stdout, "dlerror() = %s\n", dlerror());
    
    return(-1);
  }
  
  ladspa_descriptor = (void *) dlsym((void *) plugin_so,
                                     "ladspa_descriptor");
    
  if(ladspa_descriptor == NULL){
    fprintf(stdout, "ladspa_descriptor = NULL\n");

    fprintf(stdout, "dlerror() = %s\n", dlerror());
    
    return(-1);
  }
  
  return(0);
}
