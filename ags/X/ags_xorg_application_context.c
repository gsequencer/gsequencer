/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_effect_line.h>
#include <ags/X/ags_effect_separator.h>

#include <ags/X/file/ags_gui_file_xml.h>
#include <ags/X/file/ags_simple_file.h>

#include <ags/X/thread/ags_gui_thread.h>
#include <ags/X/thread/ags_simple_autosave_thread.h>

#include <ags/X/machine/ags_panel_input_pad.h>
#include <ags/X/machine/ags_panel_input_line.h>
#include <ags/X/machine/ags_mixer_input_pad.h>
#include <ags/X/machine/ags_mixer_input_line.h>
#include <ags/X/machine/ags_drum_output_pad.h>
#include <ags/X/machine/ags_drum_output_line.h>
#include <ags/X/machine/ags_drum_input_line.h>
#include <ags/X/machine/ags_synth_input_pad.h>
#include <ags/X/machine/ags_synth_input_line.h>
#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_lv2_bridge.h>
#include <ags/X/machine/ags_dssi_bridge.h>

#include <pango/pango.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#ifdef AGS_WITH_QUARTZ
#include <gtkmacintegration-gtk2/gtkosxapplication.h>
#endif

#include <sys/types.h>
#include <pwd.h>

#include <sys/mman.h>

#include <stdbool.h>

#include <ags/i18n.h>

void ags_xorg_application_context_signal_handler(int signr);
static void ags_xorg_application_context_signal_cleanup();

void ags_xorg_application_context_class_init(AgsXorgApplicationContextClass *xorg_application_context);
void ags_xorg_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_xorg_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void ags_xorg_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider);
void ags_xorg_application_context_ui_provider_interface_init(AgsUiProviderInterface *ui_provider);
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
GList* ags_xorg_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider);
void ags_xorg_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					     GList *worker);
GList* ags_xorg_application_context_get_soundcard(AgsSoundProvider *sound_provider);
void ags_xorg_application_context_set_soundcard(AgsSoundProvider *sound_provider,
						GList *soundcard);
GObject* ags_xorg_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider);
void ags_xorg_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
							       GObject *soundcard_thread);
GList* ags_xorg_application_context_get_sequencer(AgsSoundProvider *sound_provider);
void ags_xorg_application_context_set_sequencer(AgsSoundProvider *sound_provider,
						GList *sequencer);
GList* ags_xorg_application_context_get_distributed_manager(AgsSoundProvider *sound_provider);
GtkWidget* ags_xorg_application_context_get_window(AgsUiProvider *ui_provider);
void ags_xorg_application_context_set_window(AgsUiProvider *ui_provider,
					     GtkWidget *widget);
void ags_xorg_application_context_dispose(GObject *gobject);
void ags_xorg_application_context_finalize(GObject *gobject);

void ags_xorg_application_context_load_config(AgsApplicationContext *application_context);

void ags_xorg_application_context_prepare(AgsApplicationContext *application_context);
void ags_xorg_application_context_setup(AgsApplicationContext *application_context);

void ags_xorg_application_context_register_types(AgsApplicationContext *application_context);

void ags_xorg_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context);
xmlNode* ags_xorg_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context);

void ags_xorg_application_context_quit(AgsApplicationContext *application_context);

void ags_xorg_application_context_launch(AgsFileLaunch *launch, AgsXorgApplicationContext *application_context);

void ags_xorg_application_context_clear_cache(AgsTaskThread *task_thread,
					      gpointer data);

/**
 * SECTION:ags_xorg_application_context
 * @short_description: The xorg application context
 * @title: AgsXorgApplicationContext
 * @section_id:
 * @include: ags/X/ags_xorg_application_context.h
 *
 * #AgsXorgApplicationContext is a application context providing
 * the main window and sets up a functional audio layer.
 */

enum{
  PROP_0,
  PROP_WINDOW,
};

static gpointer ags_xorg_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_xorg_application_context_parent_connectable_interface;

AgsXorgApplicationContext *ags_xorg_application_context;
volatile gboolean ags_show_start_animation;

extern AgsApplicationContext *ags_application_context;

//TODO:JK: implement get functions
#ifndef AGS_USE_TIMER
static sigset_t ags_wait_mask;
struct sigaction ags_sigact;
#else
static sigset_t ags_timer_mask;

struct sigaction ags_sigact_timer;

struct sigevent ags_sev_timer;
struct itimerspec its;
#endif

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

    static const GInterfaceInfo ags_ui_provider_interface_info = {
      (GInterfaceInitFunc) ags_xorg_application_context_ui_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_xorg_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
							       "AgsXorgApplicationContext",
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

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_UI_PROVIDER,
				&ags_ui_provider_interface_info);
  }

  return (ags_type_xorg_application_context);
}

#ifndef AGS_USE_TIMER
void
ags_xorg_application_context_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_sigact.sa_mask));
    
    //    if(signr == AGS_ASYNC_QUEUE_SIGNAL_HIGH){
    // pthread_yield();
    //    }
  }
}

static void
ags_xorg_application_context_signal_cleanup()
{
  sigemptyset(&(ags_sigact.sa_mask));
}
#endif

#ifdef AGS_USE_TIMER
void
ags_xorg_application_context_signal_handler_timer(int sig, siginfo_t *si, void *uc)
{
  pthread_mutex_lock(AGS_THREAD(ags_application_context->main_loop)->timer_mutex);

  g_atomic_int_set(&(AGS_THREAD(ags_application_context->main_loop)->timer_expired),
		   TRUE);
  
  if(AGS_THREAD(ags_application_context->main_loop)->timer_wait){
    pthread_cond_signal(AGS_THREAD(ags_application_context->main_loop)->timer_cond);
  }
    
  pthread_mutex_unlock(AGS_THREAD(ags_application_context->main_loop)->timer_mutex);
  //  signal(sig, SIG_IGN);
}
#endif

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

  gobject->dispose = ags_xorg_application_context_dispose;
  gobject->finalize = ags_xorg_application_context_finalize;

  /* properties */
  /**
   * AgsXorgApplicationContext:window:
   *
   * The assigned window.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("window",
				   i18n_pspec("window of xorg application context"),
				   i18n_pspec("The window which this xorg application context assigned to"),
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WINDOW,
				  param_spec);

  /* AgsXorgApplicationContextClass */
  application_context = (AgsApplicationContextClass *) xorg_application_context;
  
  application_context->load_config = ags_xorg_application_context_load_config;

  application_context->prepare = ags_xorg_application_context_prepare;
  application_context->setup = ags_xorg_application_context_setup;

  application_context->register_types = ags_xorg_application_context_register_types;
  
  application_context->quit = ags_xorg_application_context_quit;

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
  concurrency_provider->get_worker = ags_xorg_application_context_get_worker;
  concurrency_provider->set_worker = ags_xorg_application_context_set_worker;
}

void
ags_xorg_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider)
{
  sound_provider->get_soundcard = ags_xorg_application_context_get_soundcard;
  sound_provider->set_soundcard = ags_xorg_application_context_set_soundcard;
  sound_provider->get_default_soundcard_thread = ags_xorg_application_context_get_default_soundcard_thread;
  sound_provider->set_default_soundcard_thread = ags_xorg_application_context_set_default_soundcard_thread;
  sound_provider->get_sequencer = ags_xorg_application_context_get_sequencer;
  sound_provider->set_sequencer = ags_xorg_application_context_set_sequencer;
  sound_provider->get_distributed_manager = ags_xorg_application_context_get_distributed_manager;
}

void
ags_xorg_application_context_ui_provider_interface_init(AgsUiProviderInterface *ui_provider)
{
  ui_provider->get_window = ags_xorg_application_context_get_window;
  ui_provider->set_window = ags_xorg_application_context_set_window;
}

void
ags_xorg_application_context_init(AgsXorgApplicationContext *xorg_application_context)
{
  AgsConfig *config;

  if(ags_application_context == NULL){
    ags_application_context = xorg_application_context;
  }
  
  /* fundamental instances */
  config = ags_config_get_instance();
  AGS_APPLICATION_CONTEXT(xorg_application_context)->config = config;
  g_object_ref(config);
  g_object_set(config,
	       "application-context", xorg_application_context,
	       NULL);

  AGS_APPLICATION_CONTEXT(xorg_application_context)->log = ags_log_get_instance();
  g_object_ref(AGS_APPLICATION_CONTEXT(xorg_application_context)->log);
  
  /* Xorg application context */
  g_atomic_int_set(&(xorg_application_context->gui_ready),
		   FALSE);

  g_atomic_int_set(&(xorg_application_context->show_animation),
		   TRUE);

  g_atomic_int_set(&(xorg_application_context->file_ready),
		   FALSE);

  xorg_application_context->thread_pool = NULL;

  xorg_application_context->polling_thread = NULL;

  xorg_application_context->worker = NULL;
  
  xorg_application_context->soundcard_thread = NULL;
  xorg_application_context->export_thread = NULL;

  xorg_application_context->gui_thread = NULL;
  
  xorg_application_context->autosave_thread = NULL;

  xorg_application_context->server = NULL;
  
  xorg_application_context->soundcard = NULL;
  xorg_application_context->sequencer = NULL;

  xorg_application_context->distributed_manager = NULL;
  
  xorg_application_context->window = NULL;
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

      if(window == xorg_application_context->window){
	return;
      }

      if(xorg_application_context->window != NULL){
	g_object_unref(xorg_application_context->window);
      }
      
      if(window != NULL){
	g_object_ref(G_OBJECT(window));
      }
      
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

  GList *soundcard, *sequencer;
  
  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(xorg_application_context)->flags)) != 0){
    return;
  }

  ags_xorg_application_context_parent_connectable_interface->connect(connectable);

  /* soundcard */
  soundcard = xorg_application_context->soundcard;

  while(soundcard != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(soundcard->data));

    soundcard = soundcard->next;
  }

  /* sequencer */
  sequencer = xorg_application_context->sequencer;

  while(sequencer != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(sequencer->data));

    sequencer = sequencer->next;
  }
  
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
  return((AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->main_loop);
}

AgsThread*
ags_xorg_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider)
{
  return((AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->task_thread);
}

AgsThreadPool*
ags_xorg_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(concurrency_provider)->thread_pool);
}

GList*
ags_xorg_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(concurrency_provider)->worker);
}

void
ags_xorg_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					GList *worker)
{
  AGS_XORG_APPLICATION_CONTEXT(concurrency_provider)->worker = worker;
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

GObject*
ags_xorg_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider)
{
  return((GObject *) AGS_XORG_APPLICATION_CONTEXT(sound_provider)->soundcard_thread);
}

void
ags_xorg_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
							  GObject *soundcard_thread)
{
  AGS_XORG_APPLICATION_CONTEXT(sound_provider)->soundcard_thread = (AgsThread *) soundcard_thread;
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
ags_xorg_application_context_dispose(GObject *gobject)
{
  AgsXorgApplicationContext *xorg_application_context;

  GList *list;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);

  /* thread pool */
  if(xorg_application_context->thread_pool != NULL){
    g_object_unref(xorg_application_context->thread_pool);
    
    xorg_application_context->thread_pool = NULL;
  }

  /* polling thread */
  if(xorg_application_context->polling_thread != NULL){
    g_object_unref(xorg_application_context->polling_thread);

    xorg_application_context->polling_thread = NULL;
  }

  /* worker thread */
  if(xorg_application_context->worker != NULL){
    list = xorg_application_context->worker;

    while(list != NULL){
      g_object_run_dispose(list->data);
      
      list = list->next;
    }
    
    g_list_free_full(xorg_application_context->worker,
		     g_object_unref);

    xorg_application_context->worker = NULL;
  }
  
  /* soundcard and export thread */
  if(xorg_application_context->soundcard_thread != NULL){
    g_object_unref(xorg_application_context->soundcard_thread);

    xorg_application_context->soundcard_thread = NULL;
  }

  if(xorg_application_context->export_thread != NULL){
    g_object_unref(xorg_application_context->export_thread);

    xorg_application_context->export_thread = NULL;
  }

  /* server */
  if(xorg_application_context->server != NULL){
    g_object_set(xorg_application_context->server,
		 "application-context", NULL,
		 NULL);
    
    g_object_unref(xorg_application_context->server);

    xorg_application_context->server = NULL;
  }

  /* soundcard and sequencer */
  if(xorg_application_context->soundcard != NULL){
    list = xorg_application_context->soundcard;

    while(list != NULL){
      g_object_set(list->data,
		   "application-context", NULL,
		   NULL);

      list = list->next;
    }
    
    g_list_free_full(xorg_application_context->soundcard,
		     g_object_unref);

    xorg_application_context->soundcard = NULL;
  }

  if(xorg_application_context->sequencer != NULL){
    list = xorg_application_context->sequencer;

    while(list != NULL){
      g_object_set(list->data,
		   "application-context", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(xorg_application_context->sequencer,
		     g_object_unref);

    xorg_application_context->sequencer = NULL;
  }

  /* distributed manager */
  if(xorg_application_context->distributed_manager != NULL){
    list = xorg_application_context->distributed_manager;

    while(list != NULL){
      g_object_set(list->data,
		   "application-context", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(xorg_application_context->distributed_manager,
		     g_object_unref);

    xorg_application_context->distributed_manager = NULL;
  }

  /* window */
  if(xorg_application_context->window != NULL){
    g_object_set(xorg_application_context->window,
		 "application-context", NULL,
		 NULL);
    
    gtk_widget_destroy(xorg_application_context->window);

    xorg_application_context->window = NULL;
  }  
  
  /* call parent */
  G_OBJECT_CLASS(ags_xorg_application_context_parent_class)->dispose(gobject);
}

void
ags_xorg_application_context_finalize(GObject *gobject)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);

  if(xorg_application_context->thread_pool != NULL){
    g_object_unref(xorg_application_context->thread_pool);
  }

  if(xorg_application_context->polling_thread != NULL){
    g_object_unref(xorg_application_context->polling_thread);
  }

  if(xorg_application_context->worker != NULL){
    g_list_free_full(xorg_application_context->worker,
		     g_object_unref);

    xorg_application_context->worker = NULL;
  }
  
  if(xorg_application_context->soundcard_thread != NULL){
    g_object_unref(xorg_application_context->soundcard_thread);
  }

  if(xorg_application_context->export_thread != NULL){
    g_object_unref(xorg_application_context->export_thread);
  }

  if(xorg_application_context->server != NULL){
    g_object_unref(xorg_application_context->server);
  }

  if(xorg_application_context->soundcard != NULL){
    g_list_free_full(xorg_application_context->soundcard,
		     g_object_unref);
  }

  if(xorg_application_context->sequencer != NULL){
    g_list_free_full(xorg_application_context->sequencer,
		     g_object_unref);
  }
  
  if(xorg_application_context->distributed_manager != NULL){
    g_list_free_full(xorg_application_context->distributed_manager,
		     g_object_unref);
  }
  
  if(xorg_application_context->window != NULL){
    gtk_widget_destroy(xorg_application_context->window);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_xorg_application_context_parent_class)->finalize(gobject);
}

GtkWidget*
ags_xorg_application_context_get_window(AgsUiProvider *ui_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(ui_provider)->window);
}

void
ags_xorg_application_context_set_window(AgsUiProvider *ui_provider,
					GtkWidget *widget)
{
  AGS_XORG_APPLICATION_CONTEXT(ui_provider)->window = widget;
}

void
ags_xorg_application_context_load_config(AgsApplicationContext *application_context)
{
  //TODO:JK: implement me
}

void
ags_xorg_application_context_prepare(AgsApplicationContext *application_context)
{
  AgsXorgApplicationContext *xorg_application_context;

  AgsThread *audio_loop, *polling_thread, *task_thread;
  AgsThread *gui_thread;
  AgsThreadPool *thread_pool;

  GList *start_queue;
  
  xorg_application_context = (AgsXorgApplicationContext *) application_context;

  /* call parent */
  AGS_APPLICATION_CONTEXT_CLASS(ags_xorg_application_context_parent_class)->prepare(application_context);
  
  /* register types */
  ags_application_context_register_types(xorg_application_context);

  /*
   * fundamental thread setup
   */
  /* AgsAudioLoop */
  audio_loop =
    application_context->main_loop = ags_audio_loop_new((GObject *) NULL,
							(GObject *) xorg_application_context);
  g_object_set(xorg_application_context,
	       "main-loop", audio_loop,
	       NULL);

  g_object_ref(audio_loop);
  ags_connectable_connect(AGS_CONNECTABLE(audio_loop));

  /* AgsPollingThread */
  polling_thread = 
    xorg_application_context->polling_thread = ags_polling_thread_new();
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				(AgsThread *) polling_thread,
				TRUE, TRUE);
  
  /* AgsTaskThread */
  task_thread = 
    application_context->task_thread = (GObject *) ags_task_thread_new();
  thread_pool = AGS_TASK_THREAD(task_thread)->thread_pool;
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(audio_loop),
				task_thread);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				(AgsThread *) task_thread,
				TRUE, TRUE);
  g_signal_connect(application_context->task_thread, "clear-cache",
		   G_CALLBACK(ags_xorg_application_context_clear_cache), NULL);
  
  /* AgsGuiThread */
  gui_thread = 
    xorg_application_context->gui_thread = (AgsThread *) ags_gui_thread_new();
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
  				(AgsThread *) gui_thread,
  				TRUE, TRUE);

  /* start engine */
  pthread_mutex_lock(audio_loop->start_mutex);
  
  start_queue = NULL;
  start_queue = g_list_prepend(start_queue,
			       polling_thread);
  start_queue = g_list_prepend(start_queue,
			       task_thread);

  g_atomic_pointer_set(&(audio_loop->start_queue),
		       start_queue);
  
  pthread_mutex_unlock(audio_loop->start_mutex);

  /* start audio loop and thread pool*/
  ags_thread_start(audio_loop);
  
  ags_thread_pool_start(thread_pool);

  /* wait for audio loop */
  pthread_mutex_lock(audio_loop->start_mutex);

  if(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE){	
    g_atomic_int_set(&(audio_loop->start_done),
		     FALSE);
      
    while(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE &&
	  g_atomic_int_get(&(audio_loop->start_done)) == FALSE){
      pthread_cond_wait(audio_loop->start_cond,
			audio_loop->start_mutex);
    }
  }
    
  pthread_mutex_unlock(audio_loop->start_mutex);

  /* start gui thread */
  ags_gui_thread_do_run(gui_thread);
}

void
ags_xorg_application_context_setup(AgsApplicationContext *application_context)
{
  AgsXorgApplicationContext *xorg_application_context;
  AgsWindow *window;

  AgsGuiThread *gui_thread;
  
  AgsServer *server;

  AgsAudioLoop *audio_loop;
  GObject *soundcard;
  GObject *sequencer;
  AgsJackServer *jack_server;
  AgsPulseServer *pulse_server;
  AgsCoreAudioServer *core_audio_server;

  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2WorkerManager *lv2_worker_manager;

  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *audio_message_queue;
  AgsMessageQueue *channel_message_queue; 
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  AgsThread *sequencer_thread;
  AgsDestroyWorker *destroy_worker;
  AgsThread *main_loop;
  AgsMutexManager *mutex_manager;

  AgsLog *log;
  AgsConfig *config;

  GList *list;  
  
  struct passwd *pw;

#ifdef AGS_USE_TIMER
  timer_t *timer_id;
#endif

  gchar *blacklist_filename;
  gchar *filename;
  gchar *soundcard_group;
  gchar *sequencer_group;
  gchar *str;
      
  uid_t uid;
  
  guint i;
  gboolean single_thread_enabled;
  gboolean has_core_audio;
  gboolean has_pulse;
  gboolean has_jack;
  
  pthread_mutex_t *application_mutex;
  
  xorg_application_context = (AgsXorgApplicationContext *) application_context;

  audio_loop = AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop;

  config = ags_config_get_instance();

  /* call parent */
  AGS_APPLICATION_CONTEXT_CLASS(ags_xorg_application_context_parent_class)->setup(application_context);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
  pthread_mutex_lock(application_mutex);

  main_loop = AGS_APPLICATION_CONTEXT(xorg_application_context)->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  gui_thread = ags_thread_find_type(main_loop,
				    AGS_TYPE_GUI_THREAD);

  log = ags_log_get_instance();

#ifndef AGS_USE_TIMER
  atexit(ags_xorg_application_context_signal_cleanup);

  /* Ignore interactive and job-control signals.  */
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  signal(AGS_THREAD_RESUME_SIG, SIG_IGN);
  signal(AGS_THREAD_SUSPEND_SIG, SIG_IGN);

  ags_sigact.sa_handler = ags_xorg_application_context_signal_handler;
  sigemptyset(&ags_sigact.sa_mask);
  ags_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_sigact, (struct sigaction *) NULL);
#else
  timer_id = (timer_t *) malloc(sizeof(timer_t));
  
  /* create timer */
  ags_sigact_timer.sa_flags = SA_SIGINFO;
  ags_sigact_timer.sa_sigaction = ags_xorg_application_context_signal_handler_timer;
  sigemptyset(&ags_sigact_timer.sa_mask);
  
  if(sigaction(SIGRTMIN, &ags_sigact_timer, NULL) == -1){
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  
  /* Block timer signal temporarily */
  sigemptyset(&ags_timer_mask);
  sigaddset(&ags_timer_mask, SIGRTMIN);
  
  if(sigprocmask(SIG_SETMASK, &ags_timer_mask, NULL) == -1){
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  /* Create the timer */
  ags_sev_timer.sigev_notify = SIGEV_SIGNAL;
  ags_sev_timer.sigev_signo = SIGRTMIN;
  ags_sev_timer.sigev_value.sival_ptr = timer_id;
  
  if(timer_create(CLOCK_MONOTONIC, &ags_sev_timer, timer_id) == -1){
    perror("timer_create");
    exit(EXIT_FAILURE);
  }
#endif
  
  /* check filename */
  filename = NULL;

  //  pthread_mutex_lock(ags_gui_thread_get_dispatch_mutex());
  
  for(i = 0; i < AGS_APPLICATION_CONTEXT(xorg_application_context)->argc; i++){
    if(!strncmp(AGS_APPLICATION_CONTEXT(xorg_application_context)->argv[i], "--filename", 11)){
      AgsSimpleFile *simple_file;

      xmlXPathContext *xpath_context; 
      xmlXPathObject *xpath_object;
      xmlNode **node;

      xmlChar *xpath;

      gchar *buffer;
      guint buffer_length;
      
      filename = AGS_APPLICATION_CONTEXT(xorg_application_context)->argv[i + 1];
      simple_file = ags_simple_file_new();
      g_object_set(simple_file,
		   "filename", filename,
		   NULL);
      ags_simple_file_open(simple_file,
			   NULL);

      str = g_strdup_printf("* Read config from file: %s", filename);
      ags_log_add_message(log,
			  str);

      xpath = "/ags-simple-file/ags-sf-config";

      /* Create xpath evaluation context */
      xpath_context = xmlXPathNewContext(simple_file->doc);

      if(xpath_context == NULL) {
	g_warning("Error: unable to create new XPath context");

	break;
      }

      /* Evaluate xpath expression */
      xpath_object = xmlXPathEval(xpath, xpath_context);

      if(xpath_object == NULL) {
	g_warning("Error: unable to evaluate xpath expression \"%s\"", xpath);
	xmlXPathFreeContext(xpath_context); 

	break;
      }

      node = xpath_object->nodesetval->nodeTab;
  
      for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
	if(node[i]->type == XML_ELEMENT_NODE){
	  buffer = xmlNodeGetContent(node[i]);
	  buffer_length = strlen(buffer);
	  
	  break;
	}
      }
      
      if(buffer != NULL){
	//	ags_config_clear(ags_config_get_instance());
	ags_config_load_from_data(ags_config_get_instance(),
				  buffer, buffer_length);
      }
      
      break;
    }
  }

  /* get user information */
  uid = getuid();
  pw = getpwuid(uid);

  /* message delivery */
  message_delivery = ags_message_delivery_get_instance();

  audio_message_queue = ags_message_queue_new("libags-audio");
  ags_message_delivery_add_queue(message_delivery,
				 audio_message_queue);
    
  /* load ladspa manager */
  ladspa_manager = ags_ladspa_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s/%s/ladspa_plugin.blacklist",
				       pw->pw_dir,
				       AGS_DEFAULT_DIRECTORY);
  ags_ladspa_manager_load_blacklist(ladspa_manager,
				    blacklist_filename);

  ags_log_add_message(log,
		      "* Loading LADSPA plugins");
  
  ags_ladspa_manager_load_default_directory(ladspa_manager);

  /* load dssi manager */
  dssi_manager = ags_dssi_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s/%s/dssi_plugin.blacklist",
				       pw->pw_dir,
				       AGS_DEFAULT_DIRECTORY);
  ags_dssi_manager_load_blacklist(dssi_manager,
				  blacklist_filename);

  ags_log_add_message(log,
		      "* Loading DSSI plugins");

  ags_dssi_manager_load_default_directory(dssi_manager);

  /* load lv2 manager */
  lv2_manager = ags_lv2_manager_get_instance();
  lv2_worker_manager = ags_lv2_worker_manager_get_instance();    

  blacklist_filename = g_strdup_printf("%s/%s/lv2_plugin.blacklist",
				       pw->pw_dir,
				       AGS_DEFAULT_DIRECTORY);
  ags_lv2_manager_load_blacklist(lv2_manager,
				 blacklist_filename);

  ags_log_add_message(log,
		      "* Loading Lv2 plugins");

  ags_lv2_manager_load_default_directory(lv2_manager);

  /* load lv2ui manager */
  lv2ui_manager = ags_lv2ui_manager_get_instance();  

  blacklist_filename = g_strdup_printf("%s/%s/lv2ui_plugin.blacklist",
				       pw->pw_dir,
				       AGS_DEFAULT_DIRECTORY);
  ags_lv2ui_manager_load_blacklist(lv2ui_manager,
				   blacklist_filename);
  
  ags_log_add_message(log,
		      "* Loading Lv2ui plugins");

  ags_lv2ui_manager_load_default_directory(lv2ui_manager);
  
  /* fix cross-references in managers */
  lv2_worker_manager->thread_pool = ((AgsXorgApplicationContext *) ags_application_context)->thread_pool;

  /* launch GUI */
  ags_log_add_message(log,
		      "* Launch user interface");
  
  single_thread_enabled = FALSE;

#ifdef AGS_USE_TIMER
  ags_gui_thread_timer_launch(gui_thread,
			      timer_id,
			      single_thread_enabled);
#else
  ags_gui_thread_launch(gui_thread,
			single_thread_enabled);
#endif

  /* distributed manager */
  xorg_application_context->distributed_manager = NULL;

  /* core audio server */
  core_audio_server = ags_core_audio_server_new((GObject *) xorg_application_context,
						NULL);
  xorg_application_context->distributed_manager = g_list_append(xorg_application_context->distributed_manager,
								core_audio_server);
  g_object_ref(G_OBJECT(core_audio_server));

  has_core_audio = FALSE;

  /* pulse server */
  pulse_server = ags_pulse_server_new((GObject *) xorg_application_context,
				      NULL);
  xorg_application_context->distributed_manager = g_list_append(xorg_application_context->distributed_manager,
								pulse_server);
  g_object_ref(G_OBJECT(pulse_server));

  has_pulse = FALSE;
  
  /* jack server */
  jack_server = ags_jack_server_new((GObject *) xorg_application_context,
				    NULL);
  xorg_application_context->distributed_manager = g_list_append(xorg_application_context->distributed_manager,
								jack_server);
  g_object_ref(G_OBJECT(jack_server));

  has_jack = FALSE;

  /* AgsSoundcard */
  xorg_application_context->soundcard = NULL;
  soundcard = NULL;

  soundcard_group = g_strdup("soundcard");
  
  for(i = 0; ; i++){
    guint pcm_channels, buffer_size, samplerate, format;

    if(!g_key_file_has_group(config->key_file,
			     soundcard_group)){
      if(i == 0){
	g_free(soundcard_group);    
	soundcard_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SOUNDCARD,
					  i);
    	
	continue;
      }else{
	break;
      }
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "backend");
    
    /* change soundcard */
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "core-audio",
			      11)){
	soundcard = ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(core_audio_server),
							       TRUE);

	has_core_audio = TRUE;
      }else if(!g_ascii_strncasecmp(str,
			      "pulse",
			      6)){
	soundcard = ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(pulse_server),
							       TRUE);

	has_pulse = TRUE;
      }else if(!g_ascii_strncasecmp(str,
			      "jack",
			      5)){
	soundcard = ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(jack_server),
							       TRUE);

	has_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	soundcard = (GObject *) ags_devout_new((GObject *) xorg_application_context);
	AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_OSS);
	AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_ALSA;
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	soundcard = (GObject *) ags_devout_new((GObject *) xorg_application_context);
	AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_ALSA);
	AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_OSS;
      }else{
	g_warning(i18n("unknown soundcard backend - %s"), str);

	g_free(soundcard_group);    
	soundcard_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SOUNDCARD,
					  i);
    
	continue;
      }
    }else{
      g_warning(i18n("unknown soundcard backend - NULL"));

      g_free(soundcard_group);    
      soundcard_group = g_strdup_printf("%s-%d",
					AGS_CONFIG_SOUNDCARD,
					i);
          
      continue;
    }

    if(xorg_application_context->soundcard == NULL){
      g_object_set(audio_loop,
		   "soundcard", G_OBJECT(soundcard),
		   NULL);
    }
    
    xorg_application_context->soundcard = g_list_append(xorg_application_context->soundcard,
							soundcard);
    g_object_ref(soundcard);

    /* device */
    str = ags_config_get_value(config,
			       soundcard_group,
			       "device");

    if(str != NULL){
      ags_soundcard_set_device(AGS_SOUNDCARD(soundcard),
			       str);
      g_free(str);
    }

    /* presets */
    pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    format = AGS_SOUNDCARD_DEFAULT_FORMAT;

    str = ags_config_get_value(config,
			       soundcard_group,
			       "pcm-channels");

    if(str != NULL){
      pcm_channels = g_ascii_strtoull(str,
				      NULL,
				      10);
      g_free(str);
    }


    str = ags_config_get_value(config,
			       soundcard_group,
			       "buffer-size");

    if(str != NULL){
      buffer_size = g_ascii_strtoull(str,
				     NULL,
				     10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "samplerate");

    if(str != NULL){
      samplerate = g_ascii_strtoull(str,
				    NULL,
				    10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "format");

    if(str != NULL){
      format = g_ascii_strtoull(str,
				NULL,
				10);
      g_free(str);
    }

    ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			      pcm_channels,
			      samplerate,
			      buffer_size,
			      format);

    g_free(soundcard_group);    
    soundcard_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SOUNDCARD,
				      i);
  }

  if(xorg_application_context->soundcard != NULL){
    soundcard = xorg_application_context->soundcard->data;
  }  

  g_free(soundcard_group);

  /* AgsSequencer */
  xorg_application_context->sequencer = NULL;
  sequencer = NULL;

  sequencer_group = g_strdup("sequencer");
  
  for(i = 0; ; i++){
    guint pcm_channels, buffer_size, samplerate, format;

    if(!g_key_file_has_group(config->key_file,
			     sequencer_group)){
      if(i == 0){
	g_free(sequencer_group);    
	sequencer_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SEQUENCER,
					  i);
    	
	continue;
      }else{
	break;
      }
    }

    str = ags_config_get_value(config,
			       sequencer_group,
			       "backend");
    
    /* change sequencer */
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "jack",
			      5)){
	sequencer = ags_distributed_manager_register_sequencer(AGS_DISTRIBUTED_MANAGER(jack_server),
							       FALSE);

	has_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	sequencer = (GObject *) ags_midiin_new((GObject *) xorg_application_context);
	AGS_MIDIIN(sequencer)->flags &= (~AGS_MIDIIN_OSS);
	AGS_MIDIIN(sequencer)->flags |= AGS_MIDIIN_ALSA;
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	sequencer = (GObject *) ags_midiin_new((GObject *) xorg_application_context);
	AGS_MIDIIN(sequencer)->flags &= (~AGS_MIDIIN_ALSA);
	AGS_MIDIIN(sequencer)->flags |= AGS_MIDIIN_OSS;
      }else{
	g_warning(i18n("unknown sequencer backend - %s"), str);

	g_free(sequencer_group);    
	sequencer_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SEQUENCER,
					  i);
    
	continue;
      }
    }else{
      g_warning(i18n("unknown sequencer backend - NULL"));

      g_free(sequencer_group);    
      sequencer_group = g_strdup_printf("%s-%d",
					AGS_CONFIG_SEQUENCER,
					i);
          
      continue;
    }
    
    xorg_application_context->sequencer = g_list_append(xorg_application_context->sequencer,
							sequencer);
    g_object_ref(sequencer);

    /* device */
    str = ags_config_get_value(config,
			       sequencer_group,
			       "device");
    
    if(str != NULL){
      ags_sequencer_set_device(AGS_SEQUENCER(sequencer),
			       str);
      g_free(str);
    }

    g_free(sequencer_group);    
    sequencer_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SEQUENCER,
				      i);
  }

  g_free(sequencer_group);
  
  /* AgsWindow */
#ifdef AGS_WITH_QUARTZ
  g_object_new(GTKOSX_TYPE_APPLICATION,
	       NULL);
#endif
  window = g_object_new(AGS_TYPE_WINDOW,
			"soundcard", soundcard,
			"application-context", xorg_application_context,
			NULL);
  g_object_set(xorg_application_context,
	       "window", window,
	       NULL);

  gtk_window_set_default_size((GtkWindow *) window, 500, 500);
  gtk_paned_set_position((GtkPaned *) window->paned, 300);

  ags_connectable_connect(AGS_CONNECTABLE(window));

  /* stop animation */
  g_atomic_int_set(&(xorg_application_context->show_animation),
		   FALSE);  

  /* AgsServer */
  xorg_application_context->server = ags_server_new((GObject *) xorg_application_context);
  
  /* AgsSoundcardThread and AgsExportThread */
  xorg_application_context->soundcard_thread = NULL;
  list = xorg_application_context->soundcard;
    
  while(list != NULL){
    AgsNotifySoundcard *notify_soundcard;
    
    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(list->data);
    ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				  (AgsThread *) soundcard_thread,
				  TRUE, TRUE);

    /* notify soundcard */
    notify_soundcard = ags_notify_soundcard_new(soundcard_thread);
    AGS_TASK(notify_soundcard)->task_thread = application_context->task_thread;
    
    if(AGS_IS_DEVOUT(list->data)){
      AGS_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_JACK_DEVOUT(list->data)){
      AGS_JACK_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_PULSE_DEVOUT(list->data)){
      AGS_PULSE_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
      AGS_CORE_AUDIO_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
    }

    ags_task_thread_append_cyclic_task(application_context->task_thread,
				       notify_soundcard);

    /* export thread */
    export_thread = (AgsThread *) ags_export_thread_new(list->data,
							NULL);
    ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				  (AgsThread *) export_thread,
				  TRUE, TRUE);

    /* default soundcard thread */
    if(xorg_application_context->soundcard_thread == NULL){
      xorg_application_context->soundcard_thread = soundcard_thread;
    }

    /* default export thread */
    if(export_thread != NULL){
      xorg_application_context->export_thread = export_thread;
    }

    /* iterate */
    list = list->next;      
  }
  
  /* AgsSequencerThread */
  list = xorg_application_context->sequencer;
    
  while(list != NULL){
    sequencer_thread = (AgsThread *) ags_sequencer_thread_new(list->data);
    ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				  (AgsThread *) sequencer_thread,
				  TRUE, TRUE);

    list = list->next;      
  }

  /* AgsAutosaveThread */
  xorg_application_context->autosave_thread = NULL;
  
  if(!g_strcmp0(ags_config_get_value(application_context->config,
				     AGS_CONFIG_GENERIC,
				     "autosave-thread"),
	       "true")){
    if(g_strcmp0(ags_config_get_value(application_context->config,
				      AGS_CONFIG_GENERIC,
				      "simple-file"),
		 "false")){
      xorg_application_context->autosave_thread = (AgsThread *) ags_autosave_thread_new((GObject *) xorg_application_context);
      ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				    (AgsThread *) xorg_application_context->autosave_thread,
				    TRUE, TRUE);
    }
  }

  /* AgsWorkerThread */
  xorg_application_context->worker = NULL;

  /* AgsDestroyWorker */
  destroy_worker = ags_destroy_worker_new();
  g_object_ref(destroy_worker);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				destroy_worker,
				TRUE, TRUE);
  xorg_application_context->worker = g_list_prepend(xorg_application_context->worker,
						    destroy_worker);
  ags_thread_start(destroy_worker);
  
  /* AgsThreadPool */
  xorg_application_context->thread_pool = AGS_TASK_THREAD(application_context->task_thread)->thread_pool;

  /* launch */
  if(has_core_audio){
    ags_core_audio_server_connect_client(core_audio_server);
  }

  if(has_pulse){
    ags_pulse_server_connect_client(pulse_server);

    ags_pulse_server_start_poll(pulse_server);
  }

  if(has_jack){
    ags_jack_server_connect_client(jack_server);
  }
  
  if(filename != NULL){
    window->filename = filename;
  }
  //  pthread_mutex_unlock(ags_gui_thread_get_dispatch_mutex());
}

void
ags_xorg_application_context_register_types(AgsApplicationContext *application_context)
{
  ags_complex_get_type();

  ags_gui_thread_get_type();

  /* */
  ags_connectable_get_type();
  
  /*  */
  ags_lv2_manager_get_type();
  ags_lv2_urid_manager_get_type();
  ags_lv2_worker_manager_get_type();
  ags_lv2_worker_get_type();
  
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

  /* register tasks */
  ags_cancel_audio_get_type();
  ags_cancel_channel_get_type();

  /* register backend */
  ags_core_audio_server_get_type();
  
  ags_pulse_server_get_type();

  ags_jack_server_get_type();

  //TODO:JK: extend me
  
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
  ags_led_get_type();
  ags_indicator_get_type();
  ags_vindicator_get_type();
  ags_hindicator_get_type();
  ags_dial_get_type();
  ags_notebook_get_type();
  ags_piano_get_type();
  
  /* register machine */
  ags_effect_bridge_get_type();
  ags_effect_bulk_get_type();
  ags_effect_pad_get_type();
  ags_effect_line_get_type();
  ags_effect_separator_get_type();

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
ags_xorg_application_context_quit(AgsApplicationContext *application_context)
{
  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;

  AgsCoreAudioServer *core_audio_server;

  AgsPulseServer *pulse_server;

  AgsJackServer *jack_server;

  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  GList *core_audio_client;
  GList *jack_client;
  GList *list;

  gchar *filename;
  gchar *str;

  gboolean autosave_thread_enabled;

  pthread_mutex_t *application_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);  

  pthread_mutex_lock(application_mutex);
  
  config = application_context->config;
  
  /* autosave thread */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "autosave-thread");
  autosave_thread_enabled = (str != NULL && !g_ascii_strncasecmp(str, "true", 8)) ? TRUE: FALSE;

  /* free managers */
  ladspa_manager = ags_ladspa_manager_get_instance();
  g_object_unref(ladspa_manager);

  dssi_manager = ags_dssi_manager_get_instance();
  g_object_unref(dssi_manager);

  lv2_manager = ags_lv2_manager_get_instance();
  g_object_unref(lv2_manager);
  
  /* delete autosave file */
  if(autosave_thread_enabled){
    GFile *autosave_file;

    struct passwd *pw;

    gchar *autosave_filename;

    uid_t uid;
    
    uid = getuid();
    pw = getpwuid(uid);

    autosave_filename = NULL;
    
    if(g_strcmp0(ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "simple-file"),
		 "false")){

      gchar *filename, *offset;
    
      filename = g_strdup_printf("%s/%s/%s",
				 pw->pw_dir,
				 AGS_DEFAULT_DIRECTORY,
				 AGS_SIMPLE_AUTOSAVE_THREAD_DEFAULT_FILENAME);

      if((offset = strstr(filename,
			  "{PID}")) != NULL){
	gchar *tmp;

	tmp = g_strndup(filename,
			offset - filename);
	autosave_filename = g_strdup_printf("%s%d%s",
					    tmp,
					    getpid(),
					    &(offset[5]));

	g_free(tmp);
	g_free(filename);
      }
    }else{
      autosave_filename = g_strdup_printf("%s/%s/%d-%s",
					  pw->pw_dir,
					  AGS_DEFAULT_DIRECTORY,
					  getpid(),
					  AGS_AUTOSAVE_THREAD_DEFAULT_FILENAME);
    }

    autosave_file = g_file_new_for_path(autosave_filename);
  
    if(g_file_query_exists(autosave_file,
			   NULL)){
      g_file_delete(autosave_file,
		    NULL,
		    NULL);
    }
    
    g_free(autosave_filename);
    g_object_unref(autosave_file);
  }

  /* retrieve core audio server */
  list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));
  
  while((list = ags_list_util_find_type(list,
					AGS_TYPE_CORE_AUDIO_SERVER)) != NULL){
    core_audio_server = list->data;

    /* close client */
    core_audio_client = core_audio_server->client;

    while(core_audio_client != NULL){
#ifdef AGS_WITH_CORE_AUDIO
      AUGraphStop(AGS_CORE_AUDIO_CLIENT(core_audio_client->data)->graph);
#endif

      core_audio_client = core_audio_client->next;
    }

    list = list->next;
  }
  
  /* retrieve pulseaudio server */
  list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));
  
  while((list = ags_list_util_find_type(list,
					AGS_TYPE_PULSE_SERVER)) != NULL){
    pulse_server = list->data;

#ifdef AGS_WITH_PULSE
    pa_mainloop_quit(pulse_server->main_loop,
		     0);
#endif
 
    list = list->next;
  }
  
  /* retrieve JACK server */
  list = ags_sound_provider_get_distributed_manager(AGS_SOUND_PROVIDER(application_context));
  
  if((list = ags_list_util_find_type(list,
				     AGS_TYPE_JACK_SERVER)) != NULL){
    jack_server = list->data;

    /* close client */
    jack_client = jack_server->client;

    while(jack_client != NULL){
#ifdef AGS_WITH_JACK
      jack_client_close(AGS_JACK_CLIENT(jack_client->data)->client);
#endif

      jack_client = jack_client->next;
    }
    
    list = list->next;
  }

  pthread_mutex_unlock(application_mutex);
  
  gtk_main_quit();
}

void
ags_xorg_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context)
{
  AgsXorgApplicationContext *gobject;

  AgsConfig *config;

  AgsFileLaunch *file_launch;

  xmlNode *child;

  if(*application_context == NULL){
    gobject = (AgsXorgApplicationContext *) g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
							 NULL);

    *application_context = (GObject *) gobject;
  }else{
    gobject = (AgsXorgApplicationContext *) *application_context;
  }

  g_object_set(G_OBJECT(file),
	       "application-context", gobject,
	       NULL);

  config = ags_config_get_instance();

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
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
      if(!xmlStrncmp("ags-config",
		     child->name,
		     11)){
	ags_file_read_config(file,
			     child,
			     (GObject **) &(config));
      }else if(!xmlStrncmp("ags-window",
		     child->name,
		     11)){
	ags_file_read_window(file,
			     child,
			     &(gobject->window));
      }else if(!xmlStrncmp("ags-soundcard-list",
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
  g_signal_connect(G_OBJECT(file_launch), "start",
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
    
  audio_loop = (AgsThread *) AGS_APPLICATION_CONTEXT(application_context)->main_loop;
  audio_loop_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) audio_loop);
    
  pthread_mutex_unlock(application_mutex);

  /* show all */
  gtk_widget_show_all((GtkWidget *) application_context->window);

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
		    "ags-main");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", application_context,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_CONTEXT_PROP,
	     "xorg");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x", ((~AGS_APPLICATION_CONTEXT_CONNECTED) & (AGS_APPLICATION_CONTEXT(application_context)->flags))));

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
			(GObject *) config);
  
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
  //TODO:JK: improve me
  //  pango_fc_font_map_cache_clear(pango_cairo_font_map_get_default());
  //  pango_cairo_font_map_set_default(NULL);
  //  cairo_debug_reset_static_data();
  //  FcFini();
}

AgsXorgApplicationContext*
ags_xorg_application_context_new()
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = (AgsXorgApplicationContext *) g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
									NULL);

  return(xorg_application_context);
}
