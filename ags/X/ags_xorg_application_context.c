/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_xorg_application_context.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_main_loop.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>

#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_thread-posix.h>
#include <ags/thread/ags_thread_pool.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_midiin.h>
#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_devout.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <ags/X/file/ags_gui_file_xml.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <sys/types.h>
#include <pwd.h>

#include <sys/mman.h>

#include <jack/jslist.h>
#include <jack/jack.h>
#include <jack/control.h>
#include <stdbool.h>

void ags_xorg_application_context_class_init(AgsXorgApplicationContextClass *xorg_application_context);
void ags_xorg_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_xorg_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void ags_xorg_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider);
void ags_xorg_application_context_init(AgsXorgApplicationContext *xorg_application_context);
void ags_xorg_application_context_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void ags_xorg_application_context_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);
void ags_xorg_application_context_connect(AgsConnectable *connectable);
void ags_xorg_application_context_disconnect(AgsConnectable *connectable);
AgsThread* ags_xorg_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
AgsThread* ags_xorg_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider);
AgsThreadPool* ags_xorg_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
GList* ags_xorg_application_context_get_soundcard(AgsSoundProvider *sound_provider);
void ags_xorg_application_context_set_soundcard(AgsSoundProvider *sound_provider,
						GList *soundcar);
GList* ags_xorg_application_context_get_sequencer(AgsSoundProvider *sound_provider);
void ags_xorg_application_context_set_sequencer(AgsSoundProvider *sound_provider,
						GList *sequencer);
GList* ags_xorg_application_context_get_distributed_manager(AgsSoundProvider *sound_provider);
void ags_xorg_application_context_finalize(GObject *gobject);

void ags_xorg_application_context_load_config(AgsApplicationContext *application_context);

void ags_xorg_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context);
xmlNode* ags_xorg_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context);

void ags_xorg_application_context_launch(AgsFileLaunch *launch, AgsXorgApplicationContext *application_context);

void ags_xorg_application_context_clear_cache(AgsTaskThread *task_thread,
					      gpointer data);

static gpointer ags_xorg_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_xorg_application_context_parent_connectable_interface;

enum{
  PROP_0,
  PROP_WINDOW,
};

AgsXorgApplicationContext *ags_xorg_application_context;

GType
ags_xorg_application_context_get_type()
{
  static GType ags_type_xorg_application_context = 0;

  if(!ags_type_xorg_application_context){
    static const GTypeInfo ags_xorg_application_context_info = {
      sizeof (AgsXorgApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xorg_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXorgApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xorg_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_xorg_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrency_provider_interface_info = {
      (GInterfaceInitFunc) ags_xorg_application_context_concurrency_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_provider_interface_info = {
      (GInterfaceInitFunc) ags_xorg_application_context_sound_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_xorg_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
							       "AgsXorgApplicationContext\0",
							       &ags_xorg_application_context_info,
							       0);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_CONCURRENCY_PROVIDER,
				&ags_concurrency_provider_interface_info);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_SOUND_PROVIDER,
				&ags_sound_provider_interface_info);
  }

  return (ags_type_xorg_application_context);
}

void
ags_xorg_application_context_class_init(AgsXorgApplicationContextClass *xorg_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;
  GParamSpec *param_spec;

  ags_xorg_application_context_parent_class = g_type_class_peek_parent(xorg_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) xorg_application_context;

  gobject->set_property = ags_xorg_application_context_set_property;
  gobject->get_property = ags_xorg_application_context_get_property;

  gobject->finalize = ags_xorg_application_context_finalize;
  
  /**
   * AgsXorgApplicationContext:window:
   *
   * The assigned window.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("window\0",
				   "window of xorg application context\0",
				   "The window which this xorg application context assigned to\0",
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WINDOW,
				  param_spec);

  /* AgsXorgApplicationContextClass */
  application_context = (AgsApplicationContextClass *) xorg_application_context;
  
  application_context->load_config = ags_xorg_application_context_load_config;
  application_context->register_types = ags_xorg_application_context_register_types;

  application_context->write = ags_xorg_application_context_write;
  application_context->read = ags_xorg_application_context_read;
}

void
ags_xorg_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_xorg_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_xorg_application_context_connect;
  connectable->disconnect = ags_xorg_application_context_disconnect;
}

void
ags_xorg_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider)
{
  concurrency_provider->get_main_loop = ags_xorg_application_context_get_main_loop;
  concurrency_provider->get_task_thread = ags_xorg_application_context_get_task_thread;
  concurrency_provider->get_thread_pool = ags_xorg_application_context_get_thread_pool;
}

void
ags_xorg_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider)
{
  sound_provider->get_soundcard = ags_xorg_application_context_get_soundcard;
  sound_provider->set_soundcard = ags_xorg_application_context_set_soundcard;
  sound_provider->get_sequencer = ags_xorg_application_context_get_sequencer;
  sound_provider->set_sequencer = ags_xorg_application_context_set_sequencer;
  sound_provider->get_distributed_manager = ags_xorg_application_context_get_distributed_manager;
}

void
ags_xorg_application_context_init(AgsXorgApplicationContext *xorg_application_context)
{
  AgsWindow *window;

  AgsServer *server;

  AgsAudioLoop *audio_loop;
  GObject *soundcard;
  GObject *sequencer;
  AgsJackServer *jack_server;

  AgsConfig *config;

  JSList *jslist;

  gchar *str;
  gboolean jack_enabled;

  AGS_APPLICATION_CONTEXT(xorg_application_context)->log = NULL;

  /**/
  config = ags_config_get_instance();
  AGS_APPLICATION_CONTEXT(xorg_application_context)->config = config;
  g_object_set(config,
	       "application-context\0", xorg_application_context,
	       NULL);

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "jack\0");
  jack_enabled = (str != NULL && !g_ascii_strncasecmp(str, "enabled\0", 8)) ? TRUE: FALSE;

  if(str != NULL){
    free(str);
  }

  /* distributed manager */
  xorg_application_context->distributed_manager = NULL;

  if(jack_enabled){
    jack_server = ags_jack_server_new(xorg_application_context,
				      NULL);
    xorg_application_context->distributed_manager = g_list_prepend(xorg_application_context->distributed_manager,
								   jack_server);
    g_object_ref(G_OBJECT(jack_server));
  }
  
  /* AgsSoundcard */
  xorg_application_context->soundcard = NULL;
 
  soundcard = ags_devout_new(xorg_application_context);
  xorg_application_context->soundcard = g_list_prepend(xorg_application_context->soundcard,
						       soundcard);
  g_object_ref(G_OBJECT(soundcard));
  
  if(jack_enabled){
    GObject *tmp;
    
    //    jslist = jackctl_server_get_drivers_list(jack_server->jackctl);
    //  jackctl_server_start(jack_server->jackctl);
    //    jackctl_server_open(jack_server->jackctl,
    //			jslist->data);
    
    tmp = ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(jack_server),
						     TRUE);

    if(tmp != NULL){
      jack_nframes_t samplerate;

      soundcard = tmp;
      xorg_application_context->soundcard = g_list_prepend(xorg_application_context->soundcard,
							   soundcard);
      samplerate = jack_get_sample_rate(AGS_JACK_CLIENT(AGS_JACK_PORT(AGS_JACK_DEVOUT(soundcard)->jack_port)->jack_client)->client);
      ags_config_set_value(config,
			   AGS_CONFIG_SOUNDCARD,
			   "samplerate\0",
			   g_strdup_printf("%d\0", samplerate));
      g_object_set(soundcard,
		   "samplerate\0", samplerate,
		   NULL);
      
      g_object_ref(G_OBJECT(soundcard));
    }
  }
  
  /* AgsSequencer */
  xorg_application_context->sequencer = NULL;

  sequencer = ags_midiin_new(xorg_application_context);
  xorg_application_context->sequencer = g_list_prepend(xorg_application_context->sequencer,
						       sequencer);
  g_object_ref(G_OBJECT(sequencer));

  if(jack_enabled){
    GObject *tmp;
    
    tmp = ags_distributed_manager_register_sequencer(AGS_DISTRIBUTED_MANAGER(jack_server),
						     FALSE);

    if(tmp != NULL){
      sequencer = tmp;
      
      xorg_application_context->sequencer = g_list_prepend(xorg_application_context->sequencer,
							   sequencer);
      g_object_ref(G_OBJECT(sequencer));
    }
  }
  
  /* AgsWindow */
  window = ags_window_new(xorg_application_context);
  g_object_set(window,
	       "soundcard\0", soundcard,
	       NULL);
  AGS_XORG_APPLICATION_CONTEXT(xorg_application_context)->window = window;
  g_object_ref(G_OBJECT(window));

  gtk_window_set_default_size((GtkWindow *) window, 500, 500);
  gtk_paned_set_position((GtkPaned *) window->paned, 300);

  ags_connectable_connect(AGS_CONNECTABLE(window));
  gtk_widget_show_all((GtkWidget *) window);

  /* AgsServer */
  xorg_application_context->server = ags_server_new(xorg_application_context);

  /* AgsAudioLoop */
  AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop = 
    audio_loop = (AgsThread *) ags_audio_loop_new((GObject *) soundcard,
						  xorg_application_context);
  g_object_set(xorg_application_context,
	       "main-loop\0", audio_loop,
	       NULL);

  g_object_ref(audio_loop);
  ags_connectable_connect(AGS_CONNECTABLE(audio_loop));

  /* AgsPollingThread */
  xorg_application_context->polling_thread = ags_polling_thread_new();
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				xorg_application_context->polling_thread,
				TRUE, TRUE);
  
  /* AgsTaskThread */
  AGS_APPLICATION_CONTEXT(xorg_application_context)->task_thread = (AgsThread *) ags_task_thread_new();
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(audio_loop),
				AGS_APPLICATION_CONTEXT(xorg_application_context)->task_thread);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				AGS_APPLICATION_CONTEXT(xorg_application_context)->task_thread,
				TRUE, TRUE);
  g_signal_connect(AGS_APPLICATION_CONTEXT(xorg_application_context)->task_thread, "clear-cache\0",
		   G_CALLBACK(ags_xorg_application_context_clear_cache), NULL);

  
  /* AgsSoundcardThread */
  xorg_application_context->soundcard_thread = (AgsThread *) ags_soundcard_thread_new(soundcard);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				xorg_application_context->soundcard_thread,
				TRUE, TRUE);

  /* AgsExportThread */
  xorg_application_context->export_thread = (AgsThread *) ags_export_thread_new(soundcard,
										NULL);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				xorg_application_context->export_thread,
				TRUE, TRUE);

  /* AgsGuiThread */
  //  xorg_application_context->gui_thread = NULL;
  xorg_application_context->gui_thread = (AgsThread *) ags_gui_thread_new();
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
  				xorg_application_context->gui_thread,
  				TRUE, TRUE);

  /* AgsAutosaveThread */
  xorg_application_context->autosave_thread = NULL;
  
  if(!g_strcmp0(ags_config_get_value(AGS_APPLICATION_CONTEXT(xorg_application_context)->config,
				     AGS_CONFIG_GENERIC,
				     "autosave-thread\0"),
	       "true\0")){
    if(g_strcmp0(ags_config_get_value(AGS_APPLICATION_CONTEXT(xorg_application_context)->config,
				      AGS_CONFIG_GENERIC,
				      "simple-file\0"),
		 "false\0")){
      xorg_application_context->autosave_thread = ags_autosave_thread_new(xorg_application_context);
      ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				    xorg_application_context->autosave_thread,
				    TRUE, TRUE);
    }
  }
  
  /* AgsThreadPool */
  xorg_application_context->thread_pool = AGS_TASK_THREAD(AGS_APPLICATION_CONTEXT(xorg_application_context)->task_thread)->thread_pool;
}

void
ags_xorg_application_context_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_WINDOW:
    {
      AgsWindow *window;
      
      window = (AgsWindow *) g_value_get_object(value);

      if(window == xorg_application_context->window)
	return;

      if(xorg_application_context->window != NULL)
	g_object_unref(xorg_application_context->window);

      if(window != NULL)
	g_object_ref(G_OBJECT(window));

      xorg_application_context->window = window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_xorg_application_context_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_WINDOW:
    {
      g_value_set_object(value, xorg_application_context->window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_xorg_application_context_connect(AgsConnectable *connectable)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(xorg_application_context)->flags)) != 0){
    return;
  }

  ags_xorg_application_context_parent_connectable_interface->connect(connectable);

  g_message("connecting gui\0");

  ags_connectable_connect(AGS_CONNECTABLE(xorg_application_context->window));
}

void
ags_xorg_application_context_disconnect(AgsConnectable *connectable)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(xorg_application_context)->flags)) == 0){
    return;
  }

  ags_xorg_application_context_parent_connectable_interface->disconnect(connectable);
}

AgsThread*
ags_xorg_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_APPLICATION_CONTEXT(concurrency_provider)->main_loop);
}

AgsThread*
ags_xorg_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_APPLICATION_CONTEXT(concurrency_provider)->task_thread);
}

AgsThreadPool*
ags_xorg_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(concurrency_provider)->thread_pool);
}

GList*
ags_xorg_application_context_get_soundcard(AgsSoundProvider *sound_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(sound_provider)->soundcard);
}

void
ags_xorg_application_context_set_soundcard(AgsSoundProvider *sound_provider,
					   GList *soundcard)
{
  AGS_XORG_APPLICATION_CONTEXT(sound_provider)->soundcard = soundcard;
}

GList*
ags_xorg_application_context_get_sequencer(AgsSoundProvider *sound_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(sound_provider)->sequencer);
}

void
ags_xorg_application_context_set_sequencer(AgsSoundProvider *sound_provider,
					   GList *sequencer)
{
  AGS_XORG_APPLICATION_CONTEXT(sound_provider)->sequencer = sequencer;
}

GList*
ags_xorg_application_context_get_distributed_manager(AgsSoundProvider *sound_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(sound_provider)->distributed_manager);
}

void
ags_xorg_application_context_finalize(GObject *gobject)
{
  AgsXorgApplicationContext *xorg_application_context;

  G_OBJECT_CLASS(ags_xorg_application_context_parent_class)->finalize(gobject);

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);
}

void
ags_xorg_application_context_load_config(AgsApplicationContext *application_context)
{
  //TODO:JK: implement me
}

void
ags_xorg_application_context_register_types(AgsApplicationContext *application_context)
{
  ags_gui_thread_get_type();

  /*  */
  ags_audio_loop_get_type();
  ags_soundcard_thread_get_type();
  ags_export_thread_get_type();
  ags_record_thread_get_type();
  ags_iterator_thread_get_type();
  ags_recycling_thread_get_type();

  /*  */
  ags_audio_file_get_type();
  ags_audio_file_link_get_type();
  
  /* register recalls */
  ags_recall_channel_run_dummy_get_type();

  ags_play_audio_get_type();
  ags_play_channel_get_type();
  ags_play_channel_run_get_type();
  ags_play_channel_run_master_get_type();

  ags_stream_channel_get_type();
  ags_stream_channel_run_get_type();

  ags_loop_channel_get_type();
  ags_loop_channel_run_get_type();

  ags_copy_channel_get_type();
  ags_copy_channel_run_get_type();

  ags_volume_channel_get_type();
  ags_volume_channel_run_get_type();

  ags_peak_channel_get_type();
  ags_peak_channel_run_get_type();

  ags_recall_ladspa_get_type();
  ags_recall_ladspa_run_get_type();

  ags_recall_dssi_get_type();
  ags_recall_dssi_run_get_type();

  ags_recall_lv2_get_type();
  ags_recall_lv2_run_get_type();

  ags_delay_audio_get_type();
  ags_delay_audio_run_get_type();

  ags_count_beats_audio_get_type();
  ags_count_beats_audio_run_get_type();

  ags_copy_pattern_audio_get_type();
  ags_copy_pattern_audio_run_get_type();
  ags_copy_pattern_channel_get_type();
  ags_copy_pattern_channel_run_get_type();

  ags_buffer_channel_get_type();
  ags_buffer_channel_run_get_type();

  ags_play_notation_audio_get_type();
  ags_play_notation_audio_run_get_type();

  ags_route_dssi_audio_get_type();
  ags_route_dssi_audio_run_get_type();

  ags_route_lv2_audio_get_type();
  ags_route_lv2_audio_run_get_type();
  
  /* gui */
  //TODO:JK: move me
  ags_dial_get_type();

  /* register machine */
  ags_effect_bridge_get_type();
  ags_effect_bulk_get_type();
  ags_effect_pad_get_type();
  ags_effect_line_get_type();

  ags_bulk_member_get_type();
  ags_line_member_get_type();  
  
  ags_panel_get_type();
  ags_panel_input_pad_get_type();
  ags_panel_input_line_get_type();

  ags_mixer_get_type();
  ags_mixer_input_pad_get_type();
  ags_mixer_input_line_get_type();

  ags_drum_get_type();
  ags_drum_output_pad_get_type();
  ags_drum_output_line_get_type();
  ags_drum_input_pad_get_type();
  ags_drum_input_line_get_type();

  ags_matrix_get_type();

  ags_synth_get_type();
  ags_synth_input_pad_get_type();
  ags_synth_input_line_get_type();

  ags_ffplayer_get_type();

  ags_ladspa_bridge_get_type();
  ags_lv2_bridge_get_type();
  ags_dssi_bridge_get_type();
}

void
ags_xorg_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context)
{
  AgsXorgApplicationContext *gobject;

  AgsConfig *config;

  AgsFileLaunch *file_launch;

  xmlNode *child;

  if(*application_context == NULL){
    gobject = g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
			   NULL);

    *application_context = (GObject *) gobject;
  }else{
    gobject = (AgsApplicationContext *) *application_context;
  }

  g_object_set(G_OBJECT(file),
	       "application-context\0", gobject,
	       NULL);

  config = ags_config_get_instance();

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  /* properties */
  AGS_APPLICATION_CONTEXT(gobject)->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  AGS_APPLICATION_CONTEXT(gobject)->version = xmlGetProp(node,
							 AGS_FILE_VERSION_PROP);

  AGS_APPLICATION_CONTEXT(gobject)->build_id = xmlGetProp(node,
							  AGS_FILE_BUILD_ID_PROP);

  //TODO:JK: check version compatibelity

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-config\0",
		     child->name,
		     11)){
	ags_file_read_config(file,
			     child,
			     &(config));
      }else if(!xmlStrncmp("ags-window\0",
		     child->name,
		     11)){
	ags_file_read_window(file,
			     child,
			     &(gobject->window));
      }else if(!xmlStrncmp("ags-soundcard-list\0",
			   child->name,
			   19)){
	if(gobject->soundcard != NULL){
	  g_list_free_full(gobject->soundcard,
			   g_object_unref);

	  gobject->soundcard = NULL;
	}
	
	ags_file_read_soundcard_list(file,
				     child,
				     &(gobject->soundcard));
      }
    }

    child = child->next;
  }

  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_xorg_application_context_launch), gobject);
  ags_file_add_launch(file,
		      (GObject *) file_launch);
}

void
ags_xorg_application_context_launch(AgsFileLaunch *launch, AgsXorgApplicationContext *application_context)
{
  AgsMutexManager *mutex_manager;
  
  AgsThread *audio_loop, *task_thread, *gui_thread;

  GList *list;
  GList *start_queue;

  pthread_mutex_t *audio_loop_mutex;
  pthread_mutex_t *application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
  pthread_mutex_lock(application_mutex);
    
  audio_loop = AGS_APPLICATION_CONTEXT(application_context)->main_loop;
  audio_loop_mutex = ags_mutex_manager_lookup(mutex_manager,
					      audio_loop);
    
  pthread_mutex_unlock(application_mutex);

  /* show all */
  gtk_widget_show_all(application_context->window);

  /* wait for audio loop */
  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);
  application_context->thread_pool->parent = task_thread;

  gui_thread = ags_thread_find_type(audio_loop,
				    AGS_TYPE_GUI_THREAD);

  pthread_mutex_lock(audio_loop_mutex);
    
  start_queue = NULL;
  start_queue = g_list_prepend(start_queue,
			       task_thread);
  start_queue = g_list_prepend(start_queue,
			       gui_thread);
  g_atomic_pointer_set(&(audio_loop->start_queue),
		       start_queue);

  pthread_mutex_unlock(audio_loop_mutex);

  ags_thread_pool_start(application_context->thread_pool);
  ags_thread_start(audio_loop);
}

xmlNode*
ags_xorg_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context)
{
  AgsConfig *config;
  
  xmlNode *node, *child;
  
  gchar *id;

  config = ags_config_get_instance();
  
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-main\0");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", application_context,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_CONTEXT_PROP,
	     "xorg\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", ((~AGS_APPLICATION_CONTEXT_CONNECTED) & (AGS_APPLICATION_CONTEXT(application_context)->flags))));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_APPLICATION_CONTEXT(application_context)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_APPLICATION_CONTEXT(application_context)->build_id);

  /* add to parent */
  xmlAddChild(parent,
	      node);

  ags_file_write_config(file,
			node,
			config);
  
  ags_file_write_soundcard_list(file,
				node,
				AGS_XORG_APPLICATION_CONTEXT(application_context)->soundcard);
  
  ags_file_write_window(file,
			node,
			AGS_XORG_APPLICATION_CONTEXT(application_context)->window);

  return(node);
}

void
ags_xorg_application_context_clear_cache(AgsTaskThread *task_thread,
					 gpointer data)
{
  gdk_threads_enter();

  //TODO:JK: improve me
  pango_fc_font_map_cache_clear(pango_cairo_font_map_get_default());
  pango_cairo_font_map_set_default(NULL);
  //  cairo_debug_reset_static_data();
  //  FcFini();

  gdk_threads_leave();
}

AgsXorgApplicationContext*
ags_xorg_application_context_new()
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = (AgsXorgApplicationContext *) g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
									NULL);

  return(xorg_application_context);
}
