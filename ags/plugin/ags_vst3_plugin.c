/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/plugin/ags_vst3_plugin.h>

#include <ags/plugin/ags_plugin_port.h>

#include <string.h>
#include <math.h>

#if defined(AGS_W32API)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

void ags_vst3_plugin_class_init(AgsVst3PluginClass *vst3_plugin);
void ags_vst3_plugin_init (AgsVst3Plugin *vst3_plugin);
void ags_vst3_plugin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_vst3_plugin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_vst3_plugin_finalize(GObject *gobject);

gpointer ags_vst3_plugin_instantiate_with_params(AgsBasePlugin *base_plugin,
						 guint *n_params,
						 gchar ***parameter_name,
						 GValue **value);

void ags_vst3_plugin_connect_port(AgsBasePlugin *base_plugin,
				  gpointer plugin_handle,
				  guint port_index,
				  gpointer data_location);
void ags_vst3_plugin_activate(AgsBasePlugin *base_plugin,
			      gpointer plugin_handle);
void ags_vst3_plugin_deactivate(AgsBasePlugin *base_plugin,
				gpointer plugin_handle);
void ags_vst3_plugin_run(AgsBasePlugin *base_plugin,
			 gpointer plugin_handle,
			 snd_seq_event_t *seq_event,
			 guint frame_count);
void ags_vst3_plugin_load_plugin(AgsBasePlugin *base_plugin);

/**
 * SECTION:ags_vst3_plugin
 * @short_description: The vst3 plugin class
 * @title: AgsVst3Plugin
 * @section_id:
 * @include: ags/plugin/ags_vst3_plugin.h
 *
 * The #AgsVst3Plugin loads/unloads a Vst3 plugin.
 */

enum{
  PROP_0,
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_vst3_plugin_parent_class = NULL;
static guint vst3_plugin_signals[LAST_SIGNAL];

GHashTable *ags_vst3_plugin_process_data = NULL;

GType
ags_vst3_plugin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vst3_plugin = 0;

    static const GTypeInfo ags_vst3_plugin_info = {
      sizeof (AgsVst3PluginClass),
      NULL, /* vst3_init */
      NULL, /* vst3_finalize */
      (GClassInitFunc) ags_vst3_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVst3Plugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vst3_plugin_init,
    };

    ags_type_vst3_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
						  "AgsVst3Plugin",
						  &ags_vst3_plugin_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vst3_plugin);
  }

  return g_define_type_id__volatile;
}

void
ags_vst3_plugin_class_init(AgsVst3PluginClass *vst3_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_vst3_plugin_parent_class = g_type_class_peek_parent(vst3_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) vst3_plugin;

  gobject->set_property = ags_vst3_plugin_set_property;
  gobject->get_property = ags_vst3_plugin_get_property;

  gobject->finalize = ags_vst3_plugin_finalize;

  /* properties */

  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) vst3_plugin;

  base_plugin->instantiate_with_params = ags_vst3_plugin_instantiate_with_params;

  base_plugin->connect_port = ags_vst3_plugin_connect_port;

  base_plugin->activate = ags_vst3_plugin_activate;
  base_plugin->deactivate = ags_vst3_plugin_deactivate;

  base_plugin->run = ags_vst3_plugin_run;

  base_plugin->load_plugin = ags_vst3_plugin_load_plugin;

  /* AgsVst3PluginClass */
}

void
ags_vst3_plugin_init(AgsVst3Plugin *vst3_plugin)
{
  if(ags_vst3_plugin_process_data == NULL){
    ags_vst3_plugin_process_data = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							 NULL,
							 NULL);
  }

  vst3_plugin->program = g_hash_table_new_full(g_direct_hash, g_str_equal,
					       NULL,
					       NULL);

  vst3_plugin->get_plugin_factory = NULL;

  vst3_plugin->host_context = NULL;

  vst3_plugin->cid = NULL;

  vst3_plugin->icomponent = NULL;
  vst3_plugin->iedit_controller = NULL;
}

void
ags_vst3_plugin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsVst3Plugin *vst3_plugin;

  GRecMutex *base_plugin_mutex;

  vst3_plugin = AGS_VST3_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_vst3_plugin_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsVst3Plugin *vst3_plugin;

  GRecMutex *base_plugin_mutex;

  vst3_plugin = AGS_VST3_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_vst3_plugin_finalize(GObject *gobject)
{
  AgsVst3Plugin *vst3_plugin;

  vst3_plugin = AGS_VST3_PLUGIN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_vst3_plugin_parent_class)->finalize(gobject);
}

gpointer
ags_vst3_plugin_instantiate_with_params(AgsBasePlugin *base_plugin,
					guint *n_params,
					gchar ***parameter_name,
					GValue **value)
{
  AgsVstIPluginFactory *iplugin_factory;
  AgsVstIComponent *icomponent;
  AgsVstIEditController *iedit_controller;

  gpointer retval;

  guint buffer_length;
  guint samplerate;
  gint position;
  guint i, i_stop;
  AgsVstTResult val;
  
  AgsVstIPluginFactory* (*GetPluginFactory)();
  
  GRecMutex *base_plugin_mutex;

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  buffer_length = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  position = ags_strv_index(parameter_name[0], "buffer-size");

  if(position >= 0){
    buffer_length = g_value_get_uint(&(value[0][position]));
  }

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  position = ags_strv_index(parameter_name[0], "samplerate");

  if(position >= 0){
    samplerate = g_value_get_uint(&(value[0][position]));
  }
  
  /* get instantiate */
  g_rec_mutex_lock(base_plugin_mutex);

  GetPluginFactory = AGS_VST3_PLUGIN(base_plugin)->get_plugin_factory;
    
  g_rec_mutex_unlock(base_plugin_mutex);

  retval = NULL;
  icomponent = NULL;

  if(GetPluginFactory != NULL){    
    AgsVstPClassInfo *info;

    AgsVstTUID iedit_controller_id;
    
    iplugin_factory = GetPluginFactory();

    info = ags_vst_pclass_info_alloc();
    
    i_stop = ags_vst_iplugin_factory_count_classes(iplugin_factory);
    
    for(i = 0; i < i_stop; i++){
      ags_vst_iplugin_factory_get_class_info(iplugin_factory,
					     i, info);

      if(!g_strcmp0(ags_vst_pclass_info_get_category(info), AGS_VST_KAUDIO_EFFECT_CLASS) == FALSE){
	continue;
      }

      /* icomponent */
      val = ags_vst_iplugin_factory_create_instance(iplugin_factory,
						    ags_vst_pclass_info_get_cid(info),
						    ags_vst_icomponent_get_iid(),
						    (void **) &icomponent);

      if(val != AGS_VST_KRESULT_TRUE){
	g_warning("failed to create VST3 instance with plugin factory - IComponent");
	
	break;
      }

      retval = icomponent;
      
      if((position = ags_strv_index(parameter_name[0], "icomponent")) >= 0){
	g_value_set_pointer(&(value[0][position]),
			    icomponent);
      }
      
      ags_vst_icomponent_set_io_mode(icomponent,
				     AGS_VST_KADVANCED);

      AGS_VST3_PLUGIN(base_plugin)->host_context = ags_vst_host_context_get_instance();
      
      ags_vst_iplugin_base_initialize((AgsVstIPluginBase *) icomponent,
				      AGS_VST3_PLUGIN(base_plugin)->host_context);

      /* edit controller */
      iedit_controller = NULL;

      ags_vst_icomponent_get_controller_class_id(icomponent,
						 &iedit_controller_id);
      
      val = ags_vst_iplugin_factory_create_instance(iplugin_factory,
						    &iedit_controller_id,
						    ags_vst_iedit_controller_get_iid(),
						    (void **) &iedit_controller);

      if(val != AGS_VST_KRESULT_TRUE){
	g_warning("failed to create VST3 instance with plugin factory - IEditController");
	
	break;
      }

      if((position = ags_strv_index(parameter_name[0], "iedit-controller")) >= 0){
	g_value_set_pointer(&(value[0][position]),
			    iedit_controller);
      }

      ags_vst_iplugin_base_initialize((AgsVstIPluginBase *) iedit_controller,
				      AGS_VST3_PLUGIN(base_plugin)->host_context);

      break;
    }    
  }
  
  if(icomponent != NULL){
    AgsVstIAudioProcessor *iaudio_processor;
    AgsVstProcessSetup *setup;

    iaudio_processor = NULL;
      
    val = ags_vst_funknown_query_interface(icomponent,
					   ags_vst_iaudio_processor_get_iid(), &iaudio_processor);

    if(val != AGS_VST_KRESULT_TRUE){
      g_warning("failed to query interface of VST3 plugin - IAudioProcessor");
    }

    /* process setup */
    setup = ags_vst_process_setup_alloc();

    ags_vst_process_setup_set_process_mode(setup,
					   AGS_VST_KREALTIME);
  
    ags_vst_process_setup_set_symbolic_sample_size(setup,
						   AGS_VST_KSAMPLE32);  

    ags_vst_process_setup_set_max_samples_per_block(setup,
						    buffer_length);

    ags_vst_process_setup_set_samplerate(setup,
					 samplerate);

    val = ags_vst_iaudio_processor_setup_processing(iaudio_processor,
						    setup);
  
    if(val != AGS_VST_KRESULT_TRUE){
      g_warning("failed to setup VST3 processing - ProcessSetup");
    }  
    
    /* return value */
    if((position = ags_strv_index(parameter_name[0], "iaudio-processor")) >= 0){      
      g_value_set_pointer(&(value[0][position]),
			  iaudio_processor);
    }    
  }
  
  return(retval);
}

void
ags_vst3_plugin_connect_port(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle,
			     guint port_index,
			     gpointer data_location)
{  
  //TODO:JK: implement me
}

void
ags_vst3_plugin_activate(AgsBasePlugin *base_plugin,
			 gpointer plugin_handle)
{
  ags_vst_icomponent_set_active(plugin_handle,
				TRUE);
}

void
ags_vst3_plugin_deactivate(AgsBasePlugin *base_plugin,
			   gpointer plugin_handle)
{
  ags_vst_icomponent_set_active(plugin_handle,
				FALSE);
}

void
ags_vst3_plugin_run(AgsBasePlugin *base_plugin,
		    gpointer plugin_handle,
		    snd_seq_event_t *seq_event,
		    guint frame_count)
{
  AgsVstIComponent *icomponent;
  AgsVstIAudioProcessor *iaudio_processor;
  AgsVstProcessData *data;
  
  AgsVstTResult val;

  icomponent = plugin_handle;

  iaudio_processor = NULL;

  val = ags_vst_funknown_query_interface(icomponent,
					 ags_vst_iaudio_processor_get_iid(), &iaudio_processor);

  if(val != AGS_VST_KRESULT_TRUE){
    g_warning("failed to query interface of VST3 plugin - IAudioProcessor");

    return;
  }

  data = g_hash_table_lookup(ags_vst3_plugin_process_data,
			     icomponent);
  
  ags_vst_iaudio_processor_process(iaudio_processor,
				   data);  
}

void
ags_vst3_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  AgsVstIPluginFactory *iplugin_factory;
  AgsVstIPluginFactory2 *iplugin_factory2;

  GList *plugin_port;

  gpointer retval;

  gchar *sub_categories;
  
  guint i, i_stop;
  gboolean success;  

  GError *error;

  gboolean (*InitDll)();
  AgsVstIPluginFactory* (*GetPluginFactory)();
  
  GRecMutex *base_plugin_mutex;

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  /* dlopen */
  g_rec_mutex_lock(base_plugin_mutex);

#ifdef AGS_W32API
  base_plugin->plugin_so = LoadLibrary(base_plugin->filename);
#else
  base_plugin->plugin_so = dlopen(base_plugin->filename,
				  RTLD_NOW);
#endif
  
  if(base_plugin->plugin_so == NULL){
    g_warning("ags_vst3_plugin.c - failed to load static object file");
    
#ifndef AGS_W32API    
    dlerror();
#endif
    
    g_rec_mutex_unlock(base_plugin_mutex);

    return;
  }

  plugin_port = NULL;

  success = FALSE;

#ifdef AGS_W32API
  InitDll = GetProcAddress(base_plugin->plugin_so,
			   "InitDll");
    
  success = (AGS_VST3_PLUGIN(base_plugin)->get_plugin_factory != NULL) ? TRUE: FALSE;
#else
  InitDll = dlsym(base_plugin->plugin_so,
		  "InitDll");
  
  success = (dlerror() == NULL) ? TRUE: FALSE;
#endif

  if(InitDll){
    InitDll();
  }
    
#ifdef AGS_W32API
  GetPluginFactory =
    AGS_VST3_PLUGIN(base_plugin)->get_plugin_factory = GetProcAddress(base_plugin->plugin_so,
								      "GetPluginFactory");
    
  success = (AGS_VST3_PLUGIN(base_plugin)->get_plugin_factory != NULL) ? TRUE: FALSE;
#else
  GetPluginFactory =
    AGS_VST3_PLUGIN(base_plugin)->get_plugin_factory = dlsym(base_plugin->plugin_so,
							     "GetPluginFactory");
  
  success = (dlerror() == NULL) ? TRUE: FALSE;
#endif

  g_rec_mutex_unlock(base_plugin_mutex);

  if(success){
    AgsVstPClassInfo *info;
    AgsVstPClassInfo2 *info2;

    AgsVstTUID iedit_controller_id;
    
    char *cid;
    
    AgsVstTResult val;
    
    info = ags_vst_pclass_info_alloc();
    info2 = ags_vst_pclass_info2_alloc();    
    
    iplugin_factory = GetPluginFactory();

    i_stop = ags_vst_iplugin_factory_count_classes(iplugin_factory);
    
    for(i = 0; i < i_stop; i++){
      ags_vst_iplugin_factory_get_class_info(iplugin_factory,
					     i, info);

      if((!g_strcmp0(ags_vst_pclass_info_get_category(info), AGS_VST_KAUDIO_EFFECT_CLASS)) == FALSE){
	continue;
      }

      cid = (char *) ags_vst_pclass_info_get_cid(info);
      
      if((!strncmp(AGS_VST3_PLUGIN(base_plugin)->cid, cid, 16)) == FALSE){
	continue;
      }

      /* component */
      AGS_VST3_PLUGIN(base_plugin)->icomponent = NULL;

      val = ags_vst_iplugin_factory_create_instance(iplugin_factory,
						    cid,
						    ags_vst_icomponent_get_iid(),
						    (void **) &(AGS_VST3_PLUGIN(base_plugin)->icomponent));

      if(val != AGS_VST_KRESULT_TRUE){
	g_warning("failed to create VST3 instance with plugin factory - IComponent");
	
	break;
      }

      ags_vst_funknown_query_interface(iplugin_factory,
				       ags_vst_iplugin_factory2_get_iid(), &iplugin_factory2);

      sub_categories = NULL;

      if(iplugin_factory2 != NULL){
	ags_vst_iplugin_factory2_get_class_info2(iplugin_factory2,
						 i, info2);

	sub_categories = ags_vst_pclass_info2_get_sub_categories(info2);
      }
      
      if(sub_categories != NULL &&
	 strstr(sub_categories, "Instrument") != NULL){
	ags_base_plugin_set_flags(base_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT);
      }
      
      ags_vst_icomponent_set_io_mode(AGS_VST3_PLUGIN(base_plugin)->icomponent,
				     AGS_VST_KADVANCED);

      AGS_VST3_PLUGIN(base_plugin)->host_context = ags_vst_host_context_get_instance();
      
      ags_vst_iplugin_base_initialize((AgsVstIPluginBase *) AGS_VST3_PLUGIN(base_plugin)->icomponent,
				      AGS_VST3_PLUGIN(base_plugin)->host_context);

      /* edit controller */
      AGS_VST3_PLUGIN(base_plugin)->iedit_controller = NULL;
      
      ags_vst_icomponent_get_controller_class_id(AGS_VST3_PLUGIN(base_plugin)->icomponent,
						 &iedit_controller_id);
      
      val = ags_vst_iplugin_factory_create_instance(iplugin_factory,
						    (char *) iedit_controller_id,
						    (char *) (ags_vst_iedit_controller_get_iid())[0],
						    (void **) &(AGS_VST3_PLUGIN(base_plugin)->iedit_controller));

      if(val != AGS_VST_KRESULT_TRUE){
	g_warning("failed to create VST3 instance with plugin factory - IEditController");

	break;
      }

      ags_vst_iplugin_base_initialize((AgsVstIPluginBase *) AGS_VST3_PLUGIN(base_plugin)->iedit_controller,
				      AGS_VST3_PLUGIN(base_plugin)->host_context);

      break;
    }

    ags_vst_pclass_info_free(info);

    ags_vst_pclass_info2_free(info2);
    
    if(AGS_VST3_PLUGIN(base_plugin)->iedit_controller != NULL){
      i_stop = ags_vst_iedit_controller_get_parameter_count(AGS_VST3_PLUGIN(base_plugin)->iedit_controller);

      for(i = 0; i < i_stop; i++){
	AgsPluginPort *current_plugin_port;
      
	AgsVstParameterInfo *info;

	AgsVstParamID id;
      
	guint flags;
	gint32 step_count;
	AgsVstParamValue default_normalized_value;
      
	info = ags_vst_parameter_info_alloc();
      
	ags_vst_iedit_controller_get_parameter_info(AGS_VST3_PLUGIN(base_plugin)->iedit_controller,
						    i, info);

	flags = ags_vst_parameter_info_get_flags(info);

	if((AGS_VST_KIS_PROGRAM_CHANGE & (flags)) != 0){
	  gchar *program;
	  
	  program = g_utf16_to_utf8(ags_vst_parameter_info_get_title(info),
				    128,
				    NULL,
				    NULL,
				    &error);
	    
	  g_hash_table_insert(AGS_VST3_PLUGIN(base_plugin)->program,
			      program, GUINT_TO_POINTER(i));

	  continue;
	}
	
	current_plugin_port = ags_plugin_port_new();
	g_object_ref(current_plugin_port);

	plugin_port = g_list_prepend(plugin_port,
				     current_plugin_port);

	g_value_init(current_plugin_port->default_value,
		     G_TYPE_FLOAT);
	g_value_init(current_plugin_port->lower_value,
		     G_TYPE_FLOAT);
	g_value_init(current_plugin_port->upper_value,
		     G_TYPE_FLOAT);

	step_count = ags_vst_parameter_info_get_step_count(info);

	id = ags_vst_parameter_info_get_param_id(info);

	current_plugin_port->flags |= AGS_PLUGIN_PORT_CONTROL;
	
	current_plugin_port->port_index = i;

	error = NULL;
	current_plugin_port->port_name = g_utf16_to_utf8(ags_vst_parameter_info_get_title(info),
							 128,
							 NULL,
							 NULL,
							 &error);

	if(error != NULL){
	  g_warning("%s", error->message);
	}

	default_normalized_value = ags_vst_parameter_info_get_default_normalized_value(info);

	if(step_count == 0){
	  /* set lower */
	  g_value_set_float(current_plugin_port->lower_value,
			    0.0);
	    
	  /* set upper */
	  g_value_set_float(current_plugin_port->upper_value,
			    1.0);

	  /* set default */
	  g_value_set_float(current_plugin_port->default_value,
			    ags_vst_iedit_controller_normalized_param_to_plain(AGS_VST3_PLUGIN(base_plugin)->iedit_controller,
									       id,
									       default_normalized_value));
	
	  current_plugin_port->scale_steps = -1;
	}else if(step_count == 1){
	  /* set lower */
	  g_value_set_float(current_plugin_port->lower_value,
			    0.0);
	    
	  /* set upper */
	  g_value_set_float(current_plugin_port->upper_value,
			    1.0);

	  /* set default */
	  current_plugin_port->flags |= AGS_PLUGIN_PORT_TOGGLED;

	  g_value_set_float(current_plugin_port->default_value,
			    default_normalized_value);
	
	  current_plugin_port->scale_steps = step_count;
	}else{
	  /* set lower */
	  g_value_set_float(current_plugin_port->lower_value,
			    0.0);
	    
	  /* set upper */
	  g_value_set_float(current_plugin_port->upper_value,
			    (gfloat) step_count);

	  /* set default */
	  g_value_set_float(current_plugin_port->default_value,
			    fmin(step_count, default_normalized_value * (step_count + 1)));
	
	  current_plugin_port->scale_steps = step_count;
	}

      
	if((AGS_VST_KCAN_AUTOMATE & (flags)) != 0){
	  //TODO:JK: implement me
	}
      
	if((AGS_VST_KIS_READ_ONLY & (flags)) != 0){
	  current_plugin_port->flags |= AGS_PLUGIN_PORT_OUTPUT;
	}else{
	  current_plugin_port->flags |= AGS_PLUGIN_PORT_INPUT;
	}
      
	if((AGS_VST_KIS_WRAP_AROUND & (flags)) != 0){	
	  //TODO:JK: implement me
	}
      
	if((AGS_VST_KIS_LIST & (flags)) != 0){
	  //TODO:JK: implement me
	}
      
	if((AGS_VST_KIS_HIDDEN & (flags)) != 0){
	  //TODO:JK: implement me
	}
      
	if((AGS_VST_KIS_BYPASS & (flags)) != 0){
	  current_plugin_port->flags |= AGS_PLUGIN_PORT_TOGGLED;
	}      
      
	ags_vst_parameter_info_free(info);
      }

      base_plugin->plugin_port = g_list_reverse(plugin_port);
    }
  }
}

/**
 * ags_vst3_plugin_get_audio_output_bus_count:
 * @vst3_plugin: the #AgsVst3Plugin
 * 
 * Get audio output bus count.
 * 
 * Returns: the audio output bus count
 * 
 * Since: 3.10.5
 */
guint
ags_vst3_plugin_get_audio_output_bus_count(AgsVst3Plugin *vst3_plugin)
{
  guint audio_output_bus_count;

  audio_output_bus_count = 0;
  
  if(vst3_plugin->icomponent != NULL){
    audio_output_bus_count = ags_vst_icomponent_get_bus_count(vst3_plugin->icomponent,
							      AGS_VST_KAUDIO, AGS_VST_KOUTPUT);
  }

  return(audio_output_bus_count);
}

/**
 * ags_vst3_plugin_get_audio_input_bus_count:
 * @vst3_plugin: the #AgsVst3Plugin
 * 
 * Get audio input bus count.
 * 
 * Returns: the audio input bus count
 * 
 * Since: 3.10.5
 */
guint
ags_vst3_plugin_get_audio_input_bus_count(AgsVst3Plugin *vst3_plugin)
{
  guint audio_input_bus_count;

  audio_input_bus_count = 0;
  
  if(vst3_plugin->icomponent != NULL){
    audio_input_bus_count = ags_vst_icomponent_get_bus_count(vst3_plugin->icomponent,
							     AGS_VST_KAUDIO, AGS_VST_KINPUT);
  }

  return(audio_input_bus_count);
}

/**
 * ags_vst3_plugin_get_event_output_bus_count:
 * @vst3_plugin: the #AgsVst3Plugin
 * 
 * Get event output bus count.
 * 
 * Returns: the event output bus count
 * 
 * Since: 3.10.5
 */
guint
ags_vst3_plugin_get_event_output_bus_count(AgsVst3Plugin *vst3_plugin)
{
  guint event_output_bus_count;

  event_output_bus_count = 0;
  
  if(vst3_plugin->icomponent != NULL){
    event_output_bus_count = ags_vst_icomponent_get_bus_count(vst3_plugin->icomponent,
							      AGS_VST_KEVENT, AGS_VST_KOUTPUT);
  }

  return(event_output_bus_count);
}

/**
 * ags_vst3_plugin_get_event_input_bus_count:
 * @vst3_plugin: the #AgsVst3Plugin
 * 
 * Get event input bus count.
 * 
 * Returns: the event input bus count
 * 
 * Since: 3.10.5
 */
guint
ags_vst3_plugin_get_event_input_bus_count(AgsVst3Plugin *vst3_plugin)
{
  guint event_input_bus_count;

  event_input_bus_count = 0;
  
  if(vst3_plugin->icomponent != NULL){
    event_input_bus_count = ags_vst_icomponent_get_bus_count(vst3_plugin->icomponent,
							     AGS_VST_KEVENT, AGS_VST_KINPUT);
  }

  return(event_input_bus_count);
}

/**
 * ags_vst3_plugin_get_audio_output_port_count:
 * @vst3_plugin: the #AgsVst3Plugin
 * @bus_index: the bus index
 * 
 * Get audio output port count.
 * 
 * Returns: the audio output port count
 * 
 * Since: 3.10.10
 */
guint
ags_vst3_plugin_get_audio_output_port_count(AgsVst3Plugin *vst3_plugin,
					    guint bus_index)
{
  guint audio_output_port_count;

  audio_output_port_count = 0;
  
  if(vst3_plugin->icomponent != NULL){
    AgsVstIAudioProcessor *iaudio_processor;
    
    AgsVstSpeakerArrangement arrangement;

    guint i;
    
    iaudio_processor = NULL;
  
    ags_vst_funknown_query_interface(vst3_plugin->icomponent,
				     ags_vst_iaudio_processor_get_iid(), &iaudio_processor);
    
    ags_vst_iaudio_processor_get_bus_arrangement(iaudio_processor,
						 AGS_VST_KOUTPUT, bus_index,
						 &arrangement);

    for(i = 0; i < 8 * sizeof(AgsVstSpeakerArrangement); i++){
      if((1 << i) & arrangement != 0){
	audio_output_port_count++;
      }
    }
  }

  return(audio_output_port_count);
}

/**
 * ags_vst3_plugin_get_audio_input_port_count:
 * @vst3_plugin: the #AgsVst3Plugin
 * @bus_index: the bus index
 * 
 * Get audio input port count.
 * 
 * Returns: the audio input port count
 * 
 * Since: 3.10.10
 */
guint
ags_vst3_plugin_get_audio_input_port_count(AgsVst3Plugin *vst3_plugin,
					   guint bus_index)
{
  guint audio_input_port_count;

  audio_input_port_count = 0;
  
  if(vst3_plugin->icomponent != NULL){
    AgsVstIAudioProcessor *iaudio_processor;
    
    AgsVstSpeakerArrangement arrangement;

    guint i;
    
    iaudio_processor = NULL;
  
    ags_vst_funknown_query_interface(vst3_plugin->icomponent,
				     ags_vst_iaudio_processor_get_iid(), &iaudio_processor);
    
    ags_vst_iaudio_processor_get_bus_arrangement(iaudio_processor,
						 AGS_VST_KINPUT, bus_index,
						 &arrangement);

    for(i = 0; i < 8 * sizeof(AgsVstSpeakerArrangement); i++){
      if((1 << i) & arrangement != 0){
	audio_input_port_count++;
      }
    }
  }

  return(audio_input_port_count);
}

/**
 * ags_vst3_plugin_process_data_lookup:
 * @icomponent: the icomponent as key
 * 
 * Lookup process data.
 *
 * Since: 3.10.5
 */
AgsVstProcessData*
ags_vst3_plugin_process_data_lookup(AgsVstIComponent *icomponent)
{
  return(g_hash_table_lookup(ags_vst3_plugin_process_data,
			     icomponent));
}

/**
 * ags_vst3_plugin_process_data_insert:
 * @icomponent: the icomponent as key
 * @data: the data as value
 *
 * Insert process data
 * 
 * Since: 3.10.5
 */
void
ags_vst3_plugin_process_data_insert(AgsVstIComponent *icomponent,
				    AgsVstProcessData *data)
{
  g_hash_table_insert(ags_vst3_plugin_process_data,
		      icomponent, data);
}

/**
 * ags_vst3_plugin_process_data_remove:
 * @icomponent: the icomponent as key
 * 
 * Remove process data.
 *
 * Since: 3.10.5
 */
void
ags_vst3_plugin_process_data_remove(AgsVstIComponent *icomponent)
{
  g_hash_table_remove(ags_vst3_plugin_process_data,
		      icomponent);
}

/**
 * ags_vst3_plugin_new:
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @effect_index: the effect's index
 *
 * Create a new instance of #AgsVst3Plugin
 *
 * Returns: the new #AgsVst3Plugin
 *
 * Since: 3.10.2
 */
AgsVst3Plugin*
ags_vst3_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsVst3Plugin *vst3_plugin;

  vst3_plugin = (AgsVst3Plugin *) g_object_new(AGS_TYPE_VST3_PLUGIN,
					       "filename", filename,
					       "effect", effect,
					       "effect-index", effect_index,
					       NULL);

  return(vst3_plugin);
}
