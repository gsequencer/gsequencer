/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_gsequencer_application_context.h>

#include "config.h"

#if defined(AGS_WITH_MAC_INTEGRATION)
#include <gtkosxapplication.h>
#endif

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_animation_window.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_export_window.h>
#include <ags/app/ags_export_soundcard.h>
#include <ags/app/ags_preferences.h>
#include <ags/app/ags_meta_data_window.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_effect_bulk.h>
#include <ags/app/ags_effect_pad.h>
#include <ags/app/ags_effect_line.h>
#include <ags/app/ags_effect_separator.h>
#include <ags/app/ags_bulk_member.h>
#include <ags/app/ags_line_member.h>

#include <ags/app/file/ags_simple_file.h>

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_panel_input_pad.h>
#include <ags/app/machine/ags_panel_input_line.h>
#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_mixer_input_pad.h>
#include <ags/app/machine/ags_mixer_input_line.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_drum_output_pad.h>
#include <ags/app/machine/ags_drum_output_line.h>
#include <ags/app/machine/ags_drum_input_line.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_synth_input_pad.h>
#include <ags/app/machine/ags_synth_input_line.h>
#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/app/machine/ags_ffplayer.h>
#endif
#include <ags/app/machine/ags_ladspa_bridge.h>
#include <ags/app/machine/ags_lv2_bridge.h>
#include <ags/app/machine/ags_dssi_bridge.h>

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

#include <sys/types.h>

#if !defined(AGS_W32API)
#include <pwd.h>
#endif

#include <stdbool.h>

#include <unistd.h>

#if !defined(AGS_W32API)
#include <sys/utsname.h>
#endif

#include <sys/types.h>
#include <signal.h>

#include <string.h>
#include <strings.h>

#include <math.h>

#define _GNU_SOURCE
#include <locale.h>

#include <ags/i18n.h>

void ags_gsequencer_application_context_signal_handler(int signr);
static void ags_gsequencer_application_context_signal_cleanup();

void ags_gsequencer_application_context_class_init(AgsGSequencerApplicationContextClass *gsequencer_application_context);
void ags_gsequencer_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gsequencer_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void ags_gsequencer_application_context_service_provider_interface_init(AgsServiceProviderInterface *service_provider);
void ags_gsequencer_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider);
void ags_gsequencer_application_context_ui_provider_interface_init(AgsUiProviderInterface *ui_provider);
void ags_gsequencer_application_context_init(AgsGSequencerApplicationContext *gsequencer_application_context);
void ags_gsequencer_application_context_set_property(GObject *gobject,
						     guint prop_id,
						     const GValue *value,
						     GParamSpec *param_spec);
void ags_gsequencer_application_context_get_property(GObject *gobject,
						     guint prop_id,
						     GValue *value,
						     GParamSpec *param_spec);
void ags_gsequencer_application_context_dispose(GObject *gobject);
void ags_gsequencer_application_context_finalize(GObject *gobject);

void ags_gsequencer_application_context_connect(AgsConnectable *connectable);
void ags_gsequencer_application_context_disconnect(AgsConnectable *connectable);

AgsThread* ags_gsequencer_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
void ags_gsequencer_application_context_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
						      AgsThread *main_loop);
AgsTaskLauncher* ags_gsequencer_application_context_get_task_launcher(AgsConcurrencyProvider *concurrency_provider);
void ags_gsequencer_application_context_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
							  AgsTaskLauncher *task_launcher);
AgsThreadPool* ags_gsequencer_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
void ags_gsequencer_application_context_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
							AgsThreadPool *thread_pool);
GList* ags_gsequencer_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider);
void ags_gsequencer_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
						   GList *worker);

gboolean ags_gsequencer_application_context_is_operating(AgsServiceProvider *service_provider);

AgsServerStatus* ags_gsequencer_application_context_server_status(AgsServiceProvider *service_provider);

void ags_gsequencer_application_context_set_registry(AgsServiceProvider *service_provider,
						     AgsRegistry *registry);
AgsRegistry* ags_gsequencer_application_context_get_registry(AgsServiceProvider *service_provider);

void ags_gsequencer_application_context_set_server(AgsServiceProvider *service_provider,
						   GList *server);
GList* ags_gsequencer_application_context_get_server(AgsServiceProvider *service_provider);

GObject* ags_gsequencer_application_context_get_default_soundcard(AgsSoundProvider *sound_provider);
void ags_gsequencer_application_context_set_default_soundcard(AgsSoundProvider *sound_provider,
							      GObject *soundcard);
GObject* ags_gsequencer_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider);
void ags_gsequencer_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
								     GObject *soundcard_thread);
GList* ags_gsequencer_application_context_get_soundcard(AgsSoundProvider *sound_provider);
void ags_gsequencer_application_context_set_soundcard(AgsSoundProvider *sound_provider,
						      GList *soundcard);
GList* ags_gsequencer_application_context_get_sequencer(AgsSoundProvider *sound_provider);
void ags_gsequencer_application_context_set_sequencer(AgsSoundProvider *sound_provider,
						      GList *sequencer);
GList* ags_gsequencer_application_context_get_audio(AgsSoundProvider *sound_provider);
void ags_gsequencer_application_context_set_audio(AgsSoundProvider *sound_provider,
						  GList *audio);
GList* ags_gsequencer_application_context_get_sound_server(AgsSoundProvider *sound_provider);
void ags_gsequencer_application_context_set_sound_server(AgsSoundProvider *sound_provider,
							 GList *sound_server);
GList* ags_gsequencer_application_context_get_osc_server(AgsSoundProvider *sound_provider);
void ags_gsequencer_application_context_set_osc_server(AgsSoundProvider *sound_provider,
						       GList *osc_server);

gboolean ags_gsequencer_application_context_get_show_animation(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_show_animation(AgsUiProvider *ui_provider,
							   gboolean show_animation);
gboolean ags_gsequencer_application_context_get_gui_ready(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_gui_ready(AgsUiProvider *ui_provider,
						      gboolean gui_ready);
gboolean ags_gsequencer_application_context_get_file_ready(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_file_ready(AgsUiProvider *ui_provider,
						       gboolean file_ready);
gdouble ags_gsequencer_application_context_get_gui_scale_factor(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_gui_scale_factor(AgsUiProvider *ui_provider,
							     gdouble gui_scale_factor);
void ags_gsequencer_application_context_schedule_task(AgsUiProvider *ui_provider,
						      AgsTask *task);
void ags_gsequencer_application_context_schedule_task_all(AgsUiProvider *ui_provider,
							  GList *task);
void ags_gsequencer_application_context_clean_message(AgsUiProvider *ui_provider);
GtkWidget* ags_gsequencer_application_context_get_animation_window(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_animation_window(AgsUiProvider *ui_provider,
							     GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_window(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_window(AgsUiProvider *ui_provider,
						   GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_export_window(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_export_window(AgsUiProvider *ui_provider,
							  GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_online_help_window(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_online_help_window(AgsUiProvider *ui_provider,
							       GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_preferences(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_preferences(AgsUiProvider *ui_provider,
							GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_history_browser(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_history_browser(AgsUiProvider *ui_provider,
							    GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_midi_browser(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_midi_browser(AgsUiProvider *ui_provider,
							 GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_sample_browser(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_sample_browser(AgsUiProvider *ui_provider,
							   GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_midi_import_wizard(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_midi_import_wizard(AgsUiProvider *ui_provider,
							       GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_midi_export_wizard(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_midi_export_wizard(AgsUiProvider *ui_provider,
							       GtkWidget *widget);
GList* ags_gsequencer_application_context_get_machine(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_machine(AgsUiProvider *ui_provider,
						    GList *machine);
GtkWidget* ags_gsequencer_application_context_get_composite_editor(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_composite_editor(AgsUiProvider *ui_provider,
							     GtkWidget *widget);
GtkWidget* ags_gsequencer_application_context_get_navigation(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_navigation(AgsUiProvider *ui_provider,
						       GtkWidget *widget);
GtkApplication* ags_gsequencer_application_context_get_app(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_app(AgsUiProvider *ui_provider,
						GtkApplication *app);
GtkWidget* ags_gsequencer_application_context_get_meta_data_window(AgsUiProvider *ui_provider);
void ags_gsequencer_application_context_set_meta_data_window(AgsUiProvider *ui_provider,
							  GtkWidget *widget);

void ags_gsequencer_application_context_prepare(AgsApplicationContext *application_context);
void ags_gsequencer_application_context_setup(AgsApplicationContext *application_context);

void ags_gsequencer_application_context_register_types(AgsApplicationContext *application_context);

void ags_gsequencer_application_context_quit(AgsApplicationContext *application_context);

void ags_gsequencer_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context);
xmlNode* ags_gsequencer_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context);

void* ags_gsequencer_application_context_server_main_loop_thread(GMainLoop *main_loop);
void* ags_gsequencer_application_context_audio_main_loop_thread(GMainLoop *main_loop);

/**
 * SECTION:ags_gsequencer_application_context
 * @short_description: The gsequencer application context
 * @title: AgsGSequencerApplicationContext
 * @section_id:
 * @include: ags/app/ags_gsequencer_application_context.h
 *
 * #AgsGSequencerApplicationContext is a application context providing
 * the main window and sets up a functional audio layer.
 */

enum{
  PROP_0,
  PROP_WINDOW,
};

static gpointer ags_gsequencer_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_gsequencer_application_context_parent_connectable_interface;

extern AgsApplicationContext *ags_application_context;

//TODO:JK: implement get functions
#if !defined(AGS_W32API)
static struct sigaction ags_sigact;
#endif

static GMutex locale_mutex;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
static char *locale_env;
#else
static locale_t c_locale;
#endif

static gboolean locale_initialized = FALSE;

GType
ags_gsequencer_application_context_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gsequencer_application_context = 0;

    static const GTypeInfo ags_gsequencer_application_context_info = {
      sizeof (AgsGSequencerApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gsequencer_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGSequencerApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gsequencer_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gsequencer_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrency_provider_interface_info = {
      (GInterfaceInitFunc) ags_gsequencer_application_context_concurrency_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_service_provider_interface_info = {
      (GInterfaceInitFunc) ags_gsequencer_application_context_service_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_sound_provider_interface_info = {
      (GInterfaceInitFunc) ags_gsequencer_application_context_sound_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_ui_provider_interface_info = {
      (GInterfaceInitFunc) ags_gsequencer_application_context_ui_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_gsequencer_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
								     "AgsGSequencerApplicationContext",
								     &ags_gsequencer_application_context_info,
								     0);

    g_type_add_interface_static(ags_type_gsequencer_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_gsequencer_application_context,
				AGS_TYPE_CONCURRENCY_PROVIDER,
				&ags_concurrency_provider_interface_info);

    g_type_add_interface_static(ags_type_gsequencer_application_context,
				AGS_TYPE_SERVICE_PROVIDER,
				&ags_service_provider_interface_info);

    g_type_add_interface_static(ags_type_gsequencer_application_context,
				AGS_TYPE_SOUND_PROVIDER,
				&ags_sound_provider_interface_info);

    g_type_add_interface_static(ags_type_gsequencer_application_context,
				AGS_TYPE_UI_PROVIDER,
				&ags_ui_provider_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gsequencer_application_context);
  }

  return g_define_type_id__volatile;
}

void
ags_gsequencer_application_context_signal_handler(int signr)
{
#if !defined(AGS_W32API)
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_sigact.sa_mask));
    
    //    if(signr == AGS_ASYNC_QUEUE_SIGNAL_HIGH){
    //    }
  }
#endif
}

static void
ags_gsequencer_application_context_signal_cleanup()
{
#if !defined(AGS_W32API)
  sigemptyset(&(ags_sigact.sa_mask));
#endif
}

void
ags_gsequencer_application_context_class_init(AgsGSequencerApplicationContextClass *gsequencer_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;
  GParamSpec *param_spec;

  ags_gsequencer_application_context_parent_class = g_type_class_peek_parent(gsequencer_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) gsequencer_application_context;

  gobject->set_property = ags_gsequencer_application_context_set_property;
  gobject->get_property = ags_gsequencer_application_context_get_property;

  gobject->dispose = ags_gsequencer_application_context_dispose;
  gobject->finalize = ags_gsequencer_application_context_finalize;

  /* properties */
  /**
   * AgsGSequencerApplicationContext:window:
   *
   * The assigned window.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("window",
				   i18n_pspec("window of gsequencer application context"),
				   i18n_pspec("The window which this gsequencer application context assigned to"),
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WINDOW,
				  param_spec);

  /* AgsGSequencerApplicationContextClass */
  application_context = (AgsApplicationContextClass *) gsequencer_application_context;
 
  application_context->prepare = ags_gsequencer_application_context_prepare;
  application_context->setup = ags_gsequencer_application_context_setup;

  application_context->register_types = ags_gsequencer_application_context_register_types;
  
  application_context->quit = ags_gsequencer_application_context_quit;

  application_context->write = ags_gsequencer_application_context_write;
  application_context->read = ags_gsequencer_application_context_read;
}

void
ags_gsequencer_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_gsequencer_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_gsequencer_application_context_connect;
  connectable->disconnect = ags_gsequencer_application_context_disconnect;
}

void
ags_gsequencer_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider)
{
  concurrency_provider->get_main_loop = ags_gsequencer_application_context_get_main_loop;
  concurrency_provider->set_main_loop = ags_gsequencer_application_context_set_main_loop;

  concurrency_provider->get_task_launcher = ags_gsequencer_application_context_get_task_launcher;
  concurrency_provider->set_task_launcher = ags_gsequencer_application_context_set_task_launcher;

  concurrency_provider->get_thread_pool = ags_gsequencer_application_context_get_thread_pool;
  concurrency_provider->set_thread_pool = ags_gsequencer_application_context_set_thread_pool;

  concurrency_provider->get_worker = ags_gsequencer_application_context_get_worker;
  concurrency_provider->set_worker = ags_gsequencer_application_context_set_worker;
}

void
ags_gsequencer_application_context_service_provider_interface_init(AgsServiceProviderInterface *service_provider)
{
  service_provider->is_operating = ags_gsequencer_application_context_is_operating;

  service_provider->server_status = ags_gsequencer_application_context_server_status;

  service_provider->set_registry = ags_gsequencer_application_context_set_registry;
  service_provider->get_registry = ags_gsequencer_application_context_get_registry;
  
  service_provider->set_server = ags_gsequencer_application_context_set_server;
  service_provider->get_server = ags_gsequencer_application_context_get_server;
}

void
ags_gsequencer_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider)
{
  sound_provider->get_default_soundcard = ags_gsequencer_application_context_get_default_soundcard;
  sound_provider->set_default_soundcard = ags_gsequencer_application_context_set_default_soundcard;

  sound_provider->get_default_soundcard_thread = ags_gsequencer_application_context_get_default_soundcard_thread;
  sound_provider->set_default_soundcard_thread = ags_gsequencer_application_context_set_default_soundcard_thread;

  sound_provider->get_soundcard = ags_gsequencer_application_context_get_soundcard;
  sound_provider->set_soundcard = ags_gsequencer_application_context_set_soundcard;

  sound_provider->get_sequencer = ags_gsequencer_application_context_get_sequencer;
  sound_provider->set_sequencer = ags_gsequencer_application_context_set_sequencer;

  sound_provider->get_sound_server = ags_gsequencer_application_context_get_sound_server;
  sound_provider->set_sound_server = ags_gsequencer_application_context_set_sound_server;

  sound_provider->get_audio = ags_gsequencer_application_context_get_audio;
  sound_provider->set_audio = ags_gsequencer_application_context_set_audio;

  sound_provider->get_osc_server = ags_gsequencer_application_context_get_osc_server;
  sound_provider->set_osc_server = ags_gsequencer_application_context_set_osc_server;
}

void
ags_gsequencer_application_context_ui_provider_interface_init(AgsUiProviderInterface *ui_provider)
{
  ui_provider->get_show_animation = ags_gsequencer_application_context_get_show_animation;
  ui_provider->set_show_animation = ags_gsequencer_application_context_set_show_animation;

  ui_provider->get_gui_ready = ags_gsequencer_application_context_get_gui_ready;
  ui_provider->set_gui_ready = ags_gsequencer_application_context_set_gui_ready;

  ui_provider->get_file_ready = ags_gsequencer_application_context_get_file_ready;
  ui_provider->set_file_ready = ags_gsequencer_application_context_set_file_ready;

  ui_provider->get_gui_scale_factor = ags_gsequencer_application_context_get_gui_scale_factor;
  ui_provider->set_gui_scale_factor = ags_gsequencer_application_context_set_gui_scale_factor;

  ui_provider->schedule_task = ags_gsequencer_application_context_schedule_task;
  ui_provider->schedule_task_all = ags_gsequencer_application_context_schedule_task_all;

  ui_provider->clean_message = ags_gsequencer_application_context_clean_message;

  ui_provider->get_animation_window = ags_gsequencer_application_context_get_animation_window;
  ui_provider->set_animation_window = ags_gsequencer_application_context_set_animation_window;

  ui_provider->get_window = ags_gsequencer_application_context_get_window;
  ui_provider->set_window = ags_gsequencer_application_context_set_window;

  ui_provider->get_export_window = ags_gsequencer_application_context_get_export_window;
  ui_provider->set_export_window = ags_gsequencer_application_context_set_export_window;

  ui_provider->get_online_help_window = ags_gsequencer_application_context_get_online_help_window;
  ui_provider->set_online_help_window = ags_gsequencer_application_context_set_online_help_window;

  ui_provider->get_preferences = ags_gsequencer_application_context_get_preferences;
  ui_provider->set_preferences = ags_gsequencer_application_context_set_preferences;

  ui_provider->get_history_browser = ags_gsequencer_application_context_get_history_browser;
  ui_provider->set_history_browser = ags_gsequencer_application_context_set_history_browser;

  ui_provider->get_midi_browser = ags_gsequencer_application_context_get_midi_browser;
  ui_provider->set_midi_browser = ags_gsequencer_application_context_set_midi_browser;

  ui_provider->get_sample_browser = ags_gsequencer_application_context_get_sample_browser;
  ui_provider->set_sample_browser = ags_gsequencer_application_context_set_sample_browser;

  ui_provider->get_midi_import_wizard = ags_gsequencer_application_context_get_midi_import_wizard;
  ui_provider->set_midi_import_wizard = ags_gsequencer_application_context_set_midi_import_wizard;

  ui_provider->get_midi_export_wizard = ags_gsequencer_application_context_get_midi_export_wizard;
  ui_provider->set_midi_export_wizard = ags_gsequencer_application_context_set_midi_export_wizard;

  ui_provider->get_machine = ags_gsequencer_application_context_get_machine;
  ui_provider->set_machine = ags_gsequencer_application_context_set_machine;

  ui_provider->get_composite_editor = ags_gsequencer_application_context_get_composite_editor;
  ui_provider->set_composite_editor = ags_gsequencer_application_context_set_composite_editor;

  ui_provider->get_navigation = ags_gsequencer_application_context_get_navigation;
  ui_provider->set_navigation = ags_gsequencer_application_context_set_navigation;
  
  ui_provider->get_app = ags_gsequencer_application_context_get_app;
  ui_provider->set_app = ags_gsequencer_application_context_set_app;

  ui_provider->get_meta_data_window = ags_gsequencer_application_context_get_meta_data_window;
  ui_provider->set_meta_data_window = ags_gsequencer_application_context_set_meta_data_window;
}

void
ags_gsequencer_application_context_init(AgsGSequencerApplicationContext *gsequencer_application_context)
{
  AgsConfig *config;
  AgsLog *log;
  
  if(ags_application_context == NULL){
    ags_application_context = (AgsApplicationContext *) gsequencer_application_context;
  }
  
  /* fundamental instances */
  config = ags_config_get_instance();

  AGS_APPLICATION_CONTEXT(gsequencer_application_context)->config = config;
  g_object_ref(config);

  log = ags_log_get_instance();

  AGS_APPLICATION_CONTEXT(gsequencer_application_context)->log = (GObject *) log;
  g_object_ref(log);
  
  /* GSequencer application context */  
  gsequencer_application_context->thread_pool = NULL;

  gsequencer_application_context->worker = NULL;
  
  gsequencer_application_context->is_operating = FALSE;

  gsequencer_application_context->server_status = NULL;

  gsequencer_application_context->registry = NULL;

  gsequencer_application_context->server = NULL;

  gsequencer_application_context->default_soundcard = NULL;

  gsequencer_application_context->default_soundcard_thread = NULL;
  gsequencer_application_context->default_export_thread = NULL;
    
  gsequencer_application_context->soundcard = NULL;
  gsequencer_application_context->sequencer = NULL;

  gsequencer_application_context->sound_server = NULL;
  
  gsequencer_application_context->audio = NULL;

  gsequencer_application_context->osc_server = NULL;

  gsequencer_application_context->gui_ready = FALSE;
  gsequencer_application_context->show_animation = TRUE;
  gsequencer_application_context->file_ready = FALSE;

  gsequencer_application_context->gui_scale_factor = 1.0;

  gsequencer_application_context->task = NULL;

  gsequencer_application_context->animation_window = NULL;
  gsequencer_application_context->window = NULL;
  gsequencer_application_context->automation_window = NULL;
  gsequencer_application_context->wave_window = NULL;
  gsequencer_application_context->sheet_window = NULL;

  gsequencer_application_context->export_window = NULL;

  gsequencer_application_context->online_help_window = NULL;

  gsequencer_application_context->preferences = NULL;
  
  gsequencer_application_context->history_browser = NULL;
  gsequencer_application_context->midi_browser = NULL;
  gsequencer_application_context->sample_browser = NULL;

  gsequencer_application_context->midi_import_wizard = NULL;
  gsequencer_application_context->midi_export_wizard = NULL;

  gsequencer_application_context->machine = NULL;

  gsequencer_application_context->use_composite_editor = TRUE;

  gsequencer_application_context->composite_editor = NULL;

  gsequencer_application_context->navigation = NULL;

  gsequencer_application_context->start_loader = FALSE;

  gsequencer_application_context->setup_ready = FALSE;
  gsequencer_application_context->loader_ready = FALSE;
  gsequencer_application_context->loader_completed = FALSE;

  gsequencer_application_context->ladspa_loading = FALSE;
  gsequencer_application_context->dssi_loading = FALSE;
  gsequencer_application_context->lv2_loading = FALSE;
  gsequencer_application_context->vst3_loading = FALSE;
  
  gsequencer_application_context->ladspa_loader = NULL;
  gsequencer_application_context->dssi_loader = NULL;
  gsequencer_application_context->lv2_loader = NULL;
  gsequencer_application_context->vst3_loader = NULL;

  gsequencer_application_context->lv2_turtle_scanner = NULL;

  gsequencer_application_context->app = NULL;

  gsequencer_application_context->meta_data_window = NULL;

  g_timeout_add(AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL,
		ags_gsequencer_application_context_loader_timeout,
		gsequencer_application_context);
}

void
ags_gsequencer_application_context_set_property(GObject *gobject,
						guint prop_id,
						const GValue *value,
						GParamSpec *param_spec)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_WINDOW:
  {
    AgsWindow *window;
      
    window = (AgsWindow *) g_value_get_object(value);

    if(window == (AgsWindow *) gsequencer_application_context->window){
      return;
    }

    if(gsequencer_application_context->window != NULL){
      g_object_unref(gsequencer_application_context->window);
    }
      
    if(window != NULL){
      g_object_ref(G_OBJECT(window));
    }
      
    gsequencer_application_context->window = window;
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gsequencer_application_context_get_property(GObject *gobject,
						guint prop_id,
						GValue *value,
						GParamSpec *param_spec)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_WINDOW:
  {
    g_value_set_object(value, gsequencer_application_context->window);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gsequencer_application_context_dispose(GObject *gobject)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  GRecMutex *application_context_mutex;
  
  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(gobject);

  /* get application context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  g_rec_mutex_lock(application_context_mutex);
  
  /* thread pool */
  if(gsequencer_application_context->thread_pool != NULL){
    g_object_unref(gsequencer_application_context->thread_pool);
    
    gsequencer_application_context->thread_pool = NULL;
  }

  /* worker thread */
  if(gsequencer_application_context->worker != NULL){    
    g_list_free_full(gsequencer_application_context->worker,
		     g_object_unref);

    gsequencer_application_context->worker = NULL;
  }

  /* server status */
  if(gsequencer_application_context->server_status != NULL){
    g_object_unref(gsequencer_application_context->server_status);
    
    gsequencer_application_context->server_status = NULL;
  }

  /* server thread */
  if(gsequencer_application_context->server != NULL){    
    g_list_free_full(gsequencer_application_context->server,
		     g_object_unref);

    gsequencer_application_context->server = NULL;
  }

  /* default soundcard */
  if(gsequencer_application_context->default_soundcard != NULL){
    g_object_unref(gsequencer_application_context->default_soundcard);

    gsequencer_application_context->default_soundcard = NULL;
  }

  /* soundcard and export thread */
  if(gsequencer_application_context->default_soundcard_thread != NULL){
    g_object_unref(gsequencer_application_context->default_soundcard_thread);

    gsequencer_application_context->default_soundcard_thread = NULL;
  }

  if(gsequencer_application_context->default_export_thread != NULL){
    g_object_unref(gsequencer_application_context->default_export_thread);

    gsequencer_application_context->default_export_thread = NULL;
  }

  /* soundcard and sequencer */
  if(gsequencer_application_context->soundcard != NULL){    
    g_list_free_full(gsequencer_application_context->soundcard,
		     g_object_unref);

    gsequencer_application_context->soundcard = NULL;
  }

  if(gsequencer_application_context->sequencer != NULL){
    g_list_free_full(gsequencer_application_context->sequencer,
		     g_object_unref);

    gsequencer_application_context->sequencer = NULL;
  }

  /* audio */
  if(gsequencer_application_context->audio != NULL){
    g_list_free_full(gsequencer_application_context->audio,
		     g_object_unref);

    gsequencer_application_context->audio = NULL;
  }

  /* sound server */
  if(gsequencer_application_context->sound_server != NULL){
    g_list_free_full(gsequencer_application_context->sound_server,
		     g_object_unref);

    gsequencer_application_context->sound_server = NULL;
  }

  /* osc server */
  if(gsequencer_application_context->osc_server != NULL){
    g_list_free_full(gsequencer_application_context->osc_server,
		     g_object_unref);

    gsequencer_application_context->osc_server = NULL;
  }
  
  /* window */
  if(gsequencer_application_context->window != NULL){
    gtk_window_destroy(GTK_WIDGET(gsequencer_application_context->window));

    gsequencer_application_context->window = NULL;
  }  

  g_rec_mutex_unlock(application_context_mutex);
  
  /* call parent */
  G_OBJECT_CLASS(ags_gsequencer_application_context_parent_class)->dispose(gobject);
}

void
ags_gsequencer_application_context_finalize(GObject *gobject)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(gobject);

  /* thread pool */
  if(gsequencer_application_context->thread_pool != NULL){
    g_object_unref(gsequencer_application_context->thread_pool);
  }

  /* worker thread */
  if(gsequencer_application_context->worker != NULL){    
    g_list_free_full(gsequencer_application_context->worker,
		     g_object_unref);
  }

  /* server status */
  if(gsequencer_application_context->server_status != NULL){
    g_object_unref(gsequencer_application_context->server_status);
  }

  /* server thread */
  if(gsequencer_application_context->server != NULL){    
    g_list_free_full(gsequencer_application_context->server,
		     g_object_unref);
  }

  /* default soundcard */
  if(gsequencer_application_context->default_soundcard != NULL){
    g_object_unref(gsequencer_application_context->default_soundcard);
  }

  /* soundcard and export thread */
  if(gsequencer_application_context->default_soundcard_thread != NULL){
    g_object_unref(gsequencer_application_context->default_soundcard_thread);
  }

  if(gsequencer_application_context->default_export_thread != NULL){
    g_object_unref(gsequencer_application_context->default_export_thread);
  }

  /* soundcard and sequencer */
  if(gsequencer_application_context->soundcard != NULL){    
    g_list_free_full(gsequencer_application_context->soundcard,
		     g_object_unref);
  }

  if(gsequencer_application_context->sequencer != NULL){
    g_list_free_full(gsequencer_application_context->sequencer,
		     g_object_unref);
  }

  /* audio */
  if(gsequencer_application_context->audio != NULL){
    g_list_free_full(gsequencer_application_context->audio,
		     g_object_unref);
  }

  /* sound server */
  if(gsequencer_application_context->sound_server != NULL){
    g_list_free_full(gsequencer_application_context->sound_server,
		     g_object_unref);
  }

  /* osc server */
  if(gsequencer_application_context->osc_server != NULL){
    g_list_free_full(gsequencer_application_context->osc_server,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_gsequencer_application_context_parent_class)->finalize(gobject);
}

void
ags_gsequencer_application_context_connect(AgsConnectable *connectable)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  GList *start_soundcard, *soundcard;
  GList *start_sequencer, *sequencer;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(connectable);

  if(ags_application_context_test_flags(AGS_APPLICATION_CONTEXT(gsequencer_application_context), AGS_APPLICATION_CONTEXT_CONNECTED)){
    return;
  }

  ags_gsequencer_application_context_parent_connectable_interface->connect(connectable);
    
  /* soundcard */
  soundcard = 
    start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(gsequencer_application_context));
  
  while(soundcard != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(soundcard->data));

    soundcard = soundcard->next;
  }

  g_list_free_full(start_soundcard,
		   g_object_unref);
  
  /* sequencer */
  sequencer = 
    start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(gsequencer_application_context));

  while(sequencer != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(sequencer->data));

    sequencer = sequencer->next;
  }

  g_list_free_full(start_sequencer,
		   g_object_unref);

  /* window */
  ags_connectable_connect(AGS_CONNECTABLE(gsequencer_application_context->window));

  /* export window */
  ags_connectable_connect(AGS_CONNECTABLE(gsequencer_application_context->export_window));
}

void
ags_gsequencer_application_context_disconnect(AgsConnectable *connectable)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  GList *start_soundcard, *soundcard;
  GList *start_sequencer, *sequencer;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(connectable);

  if(!ags_application_context_test_flags(AGS_APPLICATION_CONTEXT(gsequencer_application_context), AGS_APPLICATION_CONTEXT_CONNECTED)){
    return;
  }

  ags_gsequencer_application_context_parent_connectable_interface->disconnect(connectable);
  
  /* soundcard */
  soundcard = 
    start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(gsequencer_application_context));
  
  while(soundcard != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(soundcard->data));

    soundcard = soundcard->next;
  }

  g_list_free_full(start_soundcard,
		   g_object_unref);
  
  /* sequencer */
  sequencer = 
    start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(gsequencer_application_context->sequencer));

  while(sequencer != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(sequencer->data));

    sequencer = sequencer->next;
  }

  g_list_free_full(start_sequencer,
		   g_object_unref);

  /* window */
  ags_connectable_disconnect(AGS_CONNECTABLE(gsequencer_application_context->window));

  /* export window */
  ags_connectable_disconnect(AGS_CONNECTABLE(gsequencer_application_context->export_window));
}

AgsThread*
ags_gsequencer_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThread *main_loop;

  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  if(main_loop != NULL){
    g_object_ref(main_loop);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(main_loop);
}

void
ags_gsequencer_application_context_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
						 AgsThread *main_loop)
{
  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if((AgsThread *) application_context->main_loop == main_loop){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(application_context->main_loop != NULL){
    g_object_unref(application_context->main_loop);
  }
  
  if(main_loop != NULL){
    g_object_ref(main_loop);
  }
  
  application_context->main_loop = (GObject *) main_loop;
  
  g_rec_mutex_unlock(application_context_mutex);
}

AgsTaskLauncher*
ags_gsequencer_application_context_get_task_launcher(AgsConcurrencyProvider *concurrency_provider)
{
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  task_launcher = (AgsTaskLauncher *) application_context->task_launcher;

  if(task_launcher != NULL){
    g_object_ref(task_launcher);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(task_launcher);
}

void
ags_gsequencer_application_context_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
						     AgsTaskLauncher *task_launcher)
{
  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if((AgsTaskLauncher *) application_context->task_launcher == task_launcher){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(application_context->task_launcher != NULL){
    g_object_unref(application_context->task_launcher);
  }
  
  if(task_launcher != NULL){
    g_object_ref(task_launcher);
  }
  
  application_context->task_launcher = (GObject *) task_launcher;
  
  g_rec_mutex_unlock(application_context_mutex);
}

AgsThreadPool*
ags_gsequencer_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThreadPool *thread_pool;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  thread_pool = gsequencer_application_context->thread_pool;

  if(thread_pool != NULL){
    g_object_ref(thread_pool);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(thread_pool);
}

void
ags_gsequencer_application_context_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
						   AgsThreadPool *thread_pool)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(gsequencer_application_context->thread_pool == thread_pool){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(gsequencer_application_context->thread_pool != NULL){
    g_object_unref(gsequencer_application_context->thread_pool);
  }
  
  if(thread_pool != NULL){
    g_object_ref(thread_pool);
  }
  
  gsequencer_application_context->thread_pool = thread_pool;
  
  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_gsequencer_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GList *worker;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get worker */
  g_rec_mutex_lock(application_context_mutex);

  worker = g_list_copy_deep(gsequencer_application_context->worker,
			    (GCopyFunc) g_object_ref,
			    NULL);
  
  g_rec_mutex_unlock(application_context_mutex);

  return(worker);
}

void
ags_gsequencer_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					      GList *worker)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* set worker */
  g_rec_mutex_lock(application_context_mutex);

  if(gsequencer_application_context->worker == worker){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(gsequencer_application_context->worker,
		   g_object_unref);
  
  gsequencer_application_context->worker = worker;

  g_rec_mutex_unlock(application_context_mutex);
}

gboolean
ags_gsequencer_application_context_is_operating(AgsServiceProvider *service_provider)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gboolean is_operating;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get gui ready */
  g_rec_mutex_lock(application_context_mutex);

  is_operating = gsequencer_application_context->is_operating;
   
  g_rec_mutex_unlock(application_context_mutex);

  return(is_operating);
}

AgsServerStatus*
ags_gsequencer_application_context_server_status(AgsServiceProvider *service_provider)
{
  AgsServerStatus *server_status;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  server_status = gsequencer_application_context->server_status;

  if(server_status != NULL){
    g_object_ref(server_status);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(server_status);
}

AgsRegistry*
ags_gsequencer_application_context_get_registry(AgsServiceProvider *service_provider)
{
  AgsRegistry *registry;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  registry = gsequencer_application_context->registry;

  if(registry != NULL){
    g_object_ref(registry);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(registry);
}

void
ags_gsequencer_application_context_set_registry(AgsServiceProvider *service_provider,
						AgsRegistry *registry)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(gsequencer_application_context->registry == registry){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(gsequencer_application_context->registry != NULL){
    g_object_unref(gsequencer_application_context->registry);
  }
  
  if(registry != NULL){
    g_object_ref(registry);
  }
  
  gsequencer_application_context->registry = registry;
  
  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_gsequencer_application_context_get_server(AgsServiceProvider *service_provider)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GList *server;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get server */
  g_rec_mutex_lock(application_context_mutex);

  server = g_list_copy_deep(gsequencer_application_context->server,
			    (GCopyFunc) g_object_ref,
			    NULL);
  
  g_rec_mutex_unlock(application_context_mutex);

  return(server);
}

void
ags_gsequencer_application_context_set_server(AgsServiceProvider *service_provider,
					      GList *server)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* set server */
  g_rec_mutex_lock(application_context_mutex);

  if(gsequencer_application_context->server == server){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(gsequencer_application_context->server,
		   g_object_unref);
  
  gsequencer_application_context->server = server;

  g_rec_mutex_unlock(application_context_mutex);
}

GObject*
ags_gsequencer_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider)
{
  GObject *soundcard_thread;
  
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get default soundcard thread */
  g_rec_mutex_lock(application_context_mutex);

  soundcard_thread = (GObject *) AGS_GSEQUENCER_APPLICATION_CONTEXT(sound_provider)->default_soundcard_thread;

  if(soundcard_thread != NULL){
    g_object_ref(soundcard_thread);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(soundcard_thread);
}

void
ags_gsequencer_application_context_set_default_soundcard(AgsSoundProvider *sound_provider,
							 GObject *soundcard)
{
  AgsMessageDelivery *message_delivery;
  GList *start_message_queue;

  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set default soundcard */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard == soundcard){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }

  if(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard != NULL){
    g_object_unref(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard);
  }
  
  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard = (GObject *) soundcard;

  g_rec_mutex_unlock(application_context_mutex);

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc(BAD_CAST "1.0");

    root_node = xmlNewNode(NULL,
			   BAD_CAST "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       BAD_CAST "method",
	       BAD_CAST "AgsSoundProvider::set-default-soundcard");

    /* add message */
    message = ags_message_envelope_new(G_OBJECT(sound_provider),
				       NULL,
				       doc);
    
    /* set parameter */
    message->n_params = 1;

    message->parameter_name = (gchar **) malloc(2 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    1);

    /* audio channels */
    message->parameter_name[0] = "default-soundcard";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_OBJECT);
    g_value_set_object(&(message->value[0]),
		       soundcard);

    /* terminate string vector */
    message->parameter_name[1] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      (GObject *) message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

GObject*
ags_gsequencer_application_context_get_default_soundcard(AgsSoundProvider *sound_provider)
{
  GObject *soundcard;
  
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get default soundcard */
  g_rec_mutex_lock(application_context_mutex);

  soundcard = (GObject *) AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard;

  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(soundcard);
}

void
ags_gsequencer_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
								GObject *soundcard_thread)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set default soundcard thread */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard_thread == (AgsThread *) soundcard_thread){
    g_rec_mutex_unlock(application_context_mutex);
  
    return;
  }

  if(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard_thread != NULL){
    g_object_unref(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard_thread);
  }
  
  if(soundcard_thread != NULL){
    g_object_ref(soundcard_thread);
  }
  
  AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->default_soundcard_thread = (AgsThread *) soundcard_thread;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_gsequencer_application_context_get_soundcard(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *soundcard;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get soundcard */
  g_rec_mutex_lock(application_context_mutex);
  
  soundcard = g_list_copy_deep(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->soundcard,
			       (GCopyFunc) g_object_ref,
			       NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(soundcard);
}

void
ags_gsequencer_application_context_set_soundcard(AgsSoundProvider *sound_provider,
						 GList *soundcard)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set soundcard */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->soundcard == soundcard){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }
  
  g_list_free_full(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->soundcard,
		   (GDestroyNotify) g_object_unref);

  AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->soundcard = soundcard;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_gsequencer_application_context_get_sequencer(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *sequencer;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get sequencer */
  g_rec_mutex_lock(application_context_mutex);
  
  sequencer = g_list_copy_deep(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->sequencer,
			       (GCopyFunc) g_object_ref,
			       NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(sequencer);
}

void
ags_gsequencer_application_context_set_sequencer(AgsSoundProvider *sound_provider,
						 GList *sequencer)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set sequencer */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->sequencer == sequencer){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }

  g_list_free_full(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->sequencer,
		   (GDestroyNotify) g_object_unref);
  
  AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->sequencer = sequencer;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_gsequencer_application_context_get_sound_server(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *sound_server;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get sound server */
  g_rec_mutex_lock(application_context_mutex);
  
  sound_server = g_list_copy_deep(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->sound_server,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(sound_server);
}

void
ags_gsequencer_application_context_set_sound_server(AgsSoundProvider *concurrency_provider,
						    GList *sound_server)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* set sound_server */
  g_rec_mutex_lock(application_context_mutex);

  if(gsequencer_application_context->sound_server == sound_server){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(gsequencer_application_context->sound_server,
		   g_object_unref);
  
  gsequencer_application_context->sound_server = sound_server;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_gsequencer_application_context_get_audio(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *audio;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get audio */
  g_rec_mutex_lock(application_context_mutex);
  
  audio = g_list_copy_deep(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->audio,
			   (GCopyFunc) g_object_ref,
			   NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(audio);
}

void
ags_gsequencer_application_context_set_audio(AgsSoundProvider *sound_provider,
					     GList *audio)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set audio */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->audio == audio){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }

  g_list_free_full(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->audio,
		   (GDestroyNotify) g_object_unref);

  AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->audio = audio;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_gsequencer_application_context_get_osc_server(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *osc_server;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get osc_server */
  g_rec_mutex_lock(application_context_mutex);
  
  osc_server = g_list_copy_deep(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->osc_server,
				(GCopyFunc) g_object_ref,
				NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(osc_server);
}

void
ags_gsequencer_application_context_set_osc_server(AgsSoundProvider *sound_provider,
						  GList *osc_server)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set osc_server */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->osc_server == osc_server){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }

  g_list_free_full(AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->osc_server,
		   (GDestroyNotify) g_object_unref);

  AGS_GSEQUENCER_APPLICATION_CONTEXT(application_context)->osc_server = osc_server;

  g_rec_mutex_unlock(application_context_mutex);
}

gboolean
ags_gsequencer_application_context_get_show_animation(AgsUiProvider *ui_provider)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gboolean show_animation;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get show animation */
  g_rec_mutex_lock(application_context_mutex);

  show_animation = gsequencer_application_context->show_animation;
   
  g_rec_mutex_unlock(application_context_mutex);

  return(show_animation);
}

void
ags_gsequencer_application_context_set_show_animation(AgsUiProvider *ui_provider,
						      gboolean show_animation)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* set show animation */
  g_rec_mutex_lock(application_context_mutex);

  gsequencer_application_context->show_animation = show_animation;
   
  g_rec_mutex_unlock(application_context_mutex);
}

gboolean
ags_gsequencer_application_context_get_gui_ready(AgsUiProvider *ui_provider)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gboolean gui_ready;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get gui ready */
  g_rec_mutex_lock(application_context_mutex);

  gui_ready = gsequencer_application_context->gui_ready;
   
  g_rec_mutex_unlock(application_context_mutex);

  return(gui_ready);
}

void
ags_gsequencer_application_context_set_gui_ready(AgsUiProvider *ui_provider,
						 gboolean gui_ready)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* set gui ready */
  g_rec_mutex_lock(application_context_mutex);

  gsequencer_application_context->gui_ready = gui_ready;
   
  g_rec_mutex_unlock(application_context_mutex);
}

gboolean
ags_gsequencer_application_context_get_file_ready(AgsUiProvider *ui_provider)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gboolean file_ready;
  
  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* get file ready */
  g_rec_mutex_lock(application_context_mutex);

  file_ready = gsequencer_application_context->file_ready;
   
  g_rec_mutex_unlock(application_context_mutex);

  return(file_ready);
}

void
ags_gsequencer_application_context_set_file_ready(AgsUiProvider *ui_provider,
						  gboolean file_ready)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  GRecMutex *application_context_mutex;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(gsequencer_application_context);

  /* set file ready */
  g_rec_mutex_lock(application_context_mutex);

  gsequencer_application_context->file_ready = file_ready;
   
  g_rec_mutex_unlock(application_context_mutex);
}

gdouble
ags_gsequencer_application_context_get_gui_scale_factor(AgsUiProvider *ui_provider)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gdouble gui_scale_factor;
  
  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);  

  /* get gui scale facotr */
  gui_scale_factor = gsequencer_application_context->gui_scale_factor;

  return(gui_scale_factor);
}

void
ags_gsequencer_application_context_set_gui_scale_factor(AgsUiProvider *ui_provider,
							gdouble gui_scale_factor)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  gsequencer_application_context->gui_scale_factor = gui_scale_factor;

  //TODO:JK: implement me
}

void
ags_gsequencer_application_context_schedule_task(AgsUiProvider *ui_provider,
						 AgsTask *task)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  AgsTaskLauncher *task_launcher;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(gsequencer_application_context));

  if(!AGS_IS_TASK_LAUNCHER(task_launcher) ||
     !AGS_IS_TASK(task)){
    return;
  }
  
  gsequencer_application_context->task = g_list_prepend(gsequencer_application_context->task,
							task);
  
  /* unref */
  g_object_unref(task_launcher);
}

void
ags_gsequencer_application_context_schedule_task_all(AgsUiProvider *ui_provider,
						     GList *task)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  
  AgsTaskLauncher *task_launcher;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(gsequencer_application_context));

  if(!AGS_IS_TASK_LAUNCHER(task_launcher) ||
     task == NULL){
    return;
  }

  if(gsequencer_application_context->task != NULL){
    gsequencer_application_context->task = g_list_concat(g_list_reverse(task),
							 gsequencer_application_context->task);
  }else{
    gsequencer_application_context->task = g_list_reverse(task);
  }
  
  /* unref */
  g_object_unref(task_launcher);
}

void
ags_gsequencer_application_context_clean_message(AgsUiProvider *ui_provider)
{
  AgsMessageDelivery *message_delivery;
  
  GList *start_message_queue, *message_queue;
  GList *start_message_envelope, *message_envelope;

  GRecMutex *message_queue_mutex;

  message_delivery = ags_message_delivery_get_instance();

  message_queue = 
    start_message_queue = ags_message_delivery_find_recipient_namespace(message_delivery,
									"libgsequencer");

  while(message_queue != NULL){
    message_queue_mutex = AGS_MESSAGE_QUEUE_GET_OBJ_MUTEX(message_queue->data);

    g_rec_mutex_lock(message_queue_mutex);

    message_envelope =
      start_message_envelope = g_list_copy_deep(AGS_MESSAGE_QUEUE(message_queue->data)->message_envelope,
						(GCopyFunc) g_object_ref,
						NULL);

    g_rec_mutex_unlock(message_queue_mutex);

    while(message_envelope != NULL){
      ags_message_queue_remove_message_envelope(message_queue->data,
						message_envelope->data);

      /* iterate */
      message_envelope = message_envelope->next;
    }

    g_list_free_full(start_message_envelope,
		     g_object_unref);

    /* iterate */
    message_queue = message_queue->next;
  }

  g_list_free_full(start_message_queue,
		   g_object_unref);
}

GtkWidget*
ags_gsequencer_application_context_get_animation_window(AgsUiProvider *ui_provider)
{
  GtkWidget *animation_window;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get animation window */
  animation_window = gsequencer_application_context->animation_window;

  return(animation_window);
}

void
ags_gsequencer_application_context_set_animation_window(AgsUiProvider *ui_provider,
							GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set animation window */
  gsequencer_application_context->animation_window = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_window(AgsUiProvider *ui_provider)
{
  GtkWidget *window;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get window */
  window = gsequencer_application_context->window;

  return(window);
}

void
ags_gsequencer_application_context_set_window(AgsUiProvider *ui_provider,
					      GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_APPLICATION_CONTEXT(ui_provider);

  /* set window */
  gsequencer_application_context->window = widget;
}  

GtkWidget*
ags_gsequencer_application_context_get_export_window(AgsUiProvider *ui_provider)
{
  GtkWidget *export_window;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get export window */
  export_window = gsequencer_application_context->export_window;

  return(export_window);
}

void
ags_gsequencer_application_context_set_export_window(AgsUiProvider *ui_provider,
						     GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set export window */
  gsequencer_application_context->export_window = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_online_help_window(AgsUiProvider *ui_provider)
{
  GtkWidget *online_help_window;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get online help window */
  online_help_window = gsequencer_application_context->online_help_window;

  return(online_help_window);
}

void
ags_gsequencer_application_context_set_online_help_window(AgsUiProvider *ui_provider,
							  GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set online help window */
  gsequencer_application_context->online_help_window = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_preferences(AgsUiProvider *ui_provider)
{
  GtkWidget *preferences;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get preferences */
  preferences = gsequencer_application_context->preferences;

  return(preferences);
}

void
ags_gsequencer_application_context_set_preferences(AgsUiProvider *ui_provider,
						   GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set preferences */
  gsequencer_application_context->preferences = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_history_browser(AgsUiProvider *ui_provider)
{
  GtkWidget *history_browser;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get history browser */
  history_browser = gsequencer_application_context->history_browser;

  return(history_browser);
}

void
ags_gsequencer_application_context_set_history_browser(AgsUiProvider *ui_provider,
						       GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set history browser */
  gsequencer_application_context->history_browser = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_midi_browser(AgsUiProvider *ui_provider)
{
  GtkWidget *midi_browser;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get midi browser */
  midi_browser = gsequencer_application_context->midi_browser;

  return(midi_browser);
}

void
ags_gsequencer_application_context_set_midi_browser(AgsUiProvider *ui_provider,
						    GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set midi browser */
  gsequencer_application_context->midi_browser = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_sample_browser(AgsUiProvider *ui_provider)
{
  GtkWidget *sample_browser;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get sample browser */
  sample_browser = gsequencer_application_context->sample_browser;

  return(sample_browser);
}

void
ags_gsequencer_application_context_set_sample_browser(AgsUiProvider *ui_provider,
						      GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set sample browser */
  gsequencer_application_context->sample_browser = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_midi_import_wizard(AgsUiProvider *ui_provider)
{
  GtkWidget *midi_import_wizard;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get midi import wizard */
  midi_import_wizard = gsequencer_application_context->midi_import_wizard;

  return(midi_import_wizard);
}

void
ags_gsequencer_application_context_set_midi_import_wizard(AgsUiProvider *ui_provider,
							  GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set midi import wizard */
  gsequencer_application_context->midi_import_wizard = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_midi_export_wizard(AgsUiProvider *ui_provider)
{
  GtkWidget *midi_export_wizard;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get midi export wizard */
  midi_export_wizard = gsequencer_application_context->midi_export_wizard;

  return(midi_export_wizard);
}

void
ags_gsequencer_application_context_set_midi_export_wizard(AgsUiProvider *ui_provider,
							  GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set midi export wizard */
  gsequencer_application_context->midi_export_wizard = widget;
}

GList*
ags_gsequencer_application_context_get_machine(AgsUiProvider *ui_provider)
{
  GList *machine;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get machine */
  machine = g_list_copy(gsequencer_application_context->machine);

  return(machine);
}

void
ags_gsequencer_application_context_set_machine(AgsUiProvider *ui_provider,
					       GList *machine)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set machine */
  g_list_free(gsequencer_application_context->machine);
  
  gsequencer_application_context->machine = machine;
}

GtkWidget*
ags_gsequencer_application_context_get_composite_editor(AgsUiProvider *ui_provider)
{
  GtkWidget *composite_editor;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get composite editor */
  composite_editor = gsequencer_application_context->composite_editor;

  return(composite_editor);
}

void
ags_gsequencer_application_context_set_composite_editor(AgsUiProvider *ui_provider,
						  GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set composite editor */
  gsequencer_application_context->composite_editor = widget;
}

GtkWidget*
ags_gsequencer_application_context_get_navigation(AgsUiProvider *ui_provider)
{
  GtkWidget *navigation;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get navigation */
  navigation = gsequencer_application_context->navigation;

  return(navigation);
}

void
ags_gsequencer_application_context_set_navigation(AgsUiProvider *ui_provider,
						  GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set navigation */
  gsequencer_application_context->navigation = widget;
}

GtkApplication*
ags_gsequencer_application_context_get_app(AgsUiProvider *ui_provider)
{
  GtkWidget *app;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get app */
  app = gsequencer_application_context->app;

  return(app);
}

void
ags_gsequencer_application_context_set_app(AgsUiProvider *ui_provider,
					   GtkApplication *app)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set app */
  gsequencer_application_context->app = app;
}

GtkWidget*
ags_gsequencer_application_context_get_meta_data_window(AgsUiProvider *ui_provider)
{
  GtkWidget *meta_data_window;
  
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* get meta_data window */
  meta_data_window = gsequencer_application_context->meta_data_window;

  return(meta_data_window);
}

void
ags_gsequencer_application_context_set_meta_data_window(AgsUiProvider *ui_provider,
							GtkWidget *widget)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = AGS_GSEQUENCER_APPLICATION_CONTEXT(ui_provider);

  /* set meta_data window */
  gsequencer_application_context->meta_data_window = widget;
}

void
ags_gsequencer_application_context_prepare(AgsApplicationContext *application_context)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;
  GtkWidget *widget;
  AgsWindow *window;
  AgsExportWindow *export_window;
  AgsExportSoundcard *export_soundcard;
  GtkApplication *app;
  
#if defined(AGS_WITH_MAC_INTEGRATION)
  GtkosxApplication *osx_app;
#endif
  
  AgsThread *audio_loop;
  AgsTaskLauncher *task_launcher;
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;
  AgsMessageQueue *audio_message_queue;

  AgsConfig *config;
  
  GMainContext *server_main_context;
  GMainContext *audio_main_context;
  GMainContext *osc_server_main_context;
  GMainLoop *main_loop;
  
  gchar *filename;
  gchar *str;
  
  gdouble gui_scale_factor;
  gboolean no_config;
  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };
  
  gsequencer_application_context = (AgsGSequencerApplicationContext *) application_context;

  /* check filename */
  filename = NULL;

  no_config = FALSE;
  
  for(i = 0; i < AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argc;){
    if(!strncmp(AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i], "--filename", 11) &&
       i + 1 < AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argc &&
       AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i + 1] != NULL){      
      filename = AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i + 1];

      i += 2;
    }else if(!strncmp(AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i], "--no-config", 12)){
      no_config = TRUE;
      
      i++;
    }else{
      i++;
    }
  }
  
  config = ags_config_get_instance();

  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

  ags_ui_provider_set_gui_scale_factor(AGS_UI_PROVIDER(application_context),
				       gui_scale_factor);
  
  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(ags_gsequencer_application_context_parent_class)->prepare(application_context);
  
  /* register types */
  ags_application_context_register_types(application_context);

  /*
   * fundamental thread setup
   */
  /* server main context and main loop */
  server_main_context = g_main_context_new();
  g_main_context_ref(server_main_context);

  gsequencer_application_context->server_main_context = server_main_context;

  main_loop = g_main_loop_new(server_main_context,
			      TRUE);

  g_thread_new("Advanced Gtk+ Sequencer - server main loop",
	       (GThreadFunc) ags_gsequencer_application_context_server_main_loop_thread,
	       main_loop);

  /* audio main context and main loop */
#if 1
  audio_main_context = g_main_context_new();
  g_main_context_ref(audio_main_context);

  gsequencer_application_context->audio_main_context = audio_main_context;

  main_loop = g_main_loop_new(audio_main_context,
			      TRUE);

  g_thread_new("Advanced Gtk+ Sequencer - audio main loop",
	       (GThreadFunc) ags_gsequencer_application_context_audio_main_loop_thread,
	       main_loop);
#else
  gsequencer_application_context->audio_main_context = NULL;  
#endif
  
  /* message delivery */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_queue_new("libags");
  g_object_set(message_queue,
	       "recipient-namespace", "libgsequencer",
	       NULL);
  ags_message_delivery_add_message_queue(message_delivery,
					 (GObject *) message_queue);

  audio_message_queue = ags_message_queue_new("libags-audio");
  g_object_set(audio_message_queue,
	       "recipient-namespace", "libgsequencer",
	       NULL);
  ags_message_delivery_add_message_queue(message_delivery,
					 (GObject *) audio_message_queue);
  
  /* OSC server main context and main loop */
  osc_server_main_context = g_main_context_new();
  g_main_context_ref(osc_server_main_context);

  gsequencer_application_context->osc_server_main_context = osc_server_main_context;

  g_main_loop_new(osc_server_main_context,
		  TRUE);

  /* AgsAudioLoop */
  audio_loop = (AgsThread *) ags_audio_loop_new();
  ags_audio_loop_set_do_fx_staging((AgsAudioLoop *) audio_loop,
				   TRUE);
  ags_audio_loop_set_staging_program((AgsAudioLoop *) audio_loop,
				     staging_program,
				     1);
  
  g_object_ref(audio_loop);
  
  application_context->main_loop = (GObject *) audio_loop;
  ags_connectable_connect(AGS_CONNECTABLE(audio_loop));

  /* AgsTaskLauncher */
  task_launcher = ags_task_launcher_new();
  g_object_ref(task_launcher);

  application_context->task_launcher = (GObject *) task_launcher;
  ags_connectable_connect(AGS_CONNECTABLE(task_launcher));  

#if 0  
  ags_task_launcher_attach(task_launcher,
			   audio_main_context);
#endif
  
  /* start audio loop and thread pool*/
  ags_thread_start(audio_loop);
  
  /* wait for audio loop */
  g_mutex_lock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  if(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT)){
    ags_thread_unset_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE);
      
    while(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT) &&
	  !ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE)){
      g_cond_wait(AGS_THREAD_GET_START_COND(audio_loop),
		  AGS_THREAD_GET_START_MUTEX(audio_loop));
    }
  }
    
  g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  /* start gui */
  g_timeout_add((guint) (AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0),
		(GSourceFunc) ags_gsequencer_application_context_message_monitor_timeout,
		(gpointer) gsequencer_application_context);
  
  g_timeout_add((guint) (AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0),
		(GSourceFunc) ags_gsequencer_application_context_task_timeout,
		(gpointer) gsequencer_application_context);

  /* show animation */
  ags_ui_provider_set_gui_ready(AGS_UI_PROVIDER(application_context),
				TRUE);
  
  widget = (GtkWidget *) ags_animation_window_new();
  ags_ui_provider_set_animation_window(AGS_UI_PROVIDER(application_context),
				       widget);
  
  gtk_widget_show(widget);

  /* AgsWindow */
  window = (AgsWindow *) g_object_new(AGS_TYPE_WINDOW,
				      NULL);
  window->no_config = no_config;
  
  gsequencer_application_context->window = (GtkWidget *) window;

  gsequencer_application_context->navigation = (GtkWidget *) window->navigation;
  
#if defined(AGS_WITH_MAC_INTEGRATION)
  osx_app = gtkosx_application_get();

  gtkosx_application_set_menu_bar(osx_app,
				  window->menu_bar);
  
  gtkosx_application_ready(osx_app);
#endif

  gtk_window_set_default_size((GtkWindow *) window, 500, 500);
  gtk_paned_set_position((GtkPaned *) window->paned, 300);
  
  if(filename != NULL){
    window->filename = filename;
  }  

  /* AgsExportWindow */
  export_window = (AgsExportWindow *) g_object_new(AGS_TYPE_EXPORT_WINDOW,
						   NULL);
  ags_ui_provider_set_export_window(AGS_UI_PROVIDER(application_context),
				    export_window);

  export_soundcard = ags_export_soundcard_new();
  ags_export_window_add_export_soundcard(export_window,
					 export_soundcard);
  
  /* AgsMetaDataWindow */
  //  widget = (GtkWidget *) ags_meta_data_window_new();
  //  ags_ui_provider_set_meta_data_window(AGS_UI_PROVIDER(application_context),
  //				       widget);
}

void
ags_gsequencer_application_context_setup(AgsApplicationContext *application_context)
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  GObject *soundcard;
  GObject *sequencer;
  AgsJackServer *jack_server;
  AgsPulseServer *pulse_server;
  AgsCoreAudioServer *core_audio_server;

  AgsOscServer *osc_server;
  
  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;

#if defined(AGS_WITH_VST3)
  AgsVst3Manager *vst3_manager;
#endif
  
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2WorkerManager *lv2_worker_manager;

  AgsServer *server;
  
  AgsThread *main_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  AgsThread *sequencer_thread;
  AgsDestroyWorker *destroy_worker;
  AgsTaskLauncher *task_launcher;

  AgsLog *log;
  AgsConfig *config;

  GList *list;  
  
#if !defined(AGS_W32API)
  struct passwd *pw;
  
  uid_t uid;
#endif

  gchar *blacklist_path;
  gchar *blacklist_filename;
  gchar *filename;
  gchar *server_group;
  gchar *soundcard_group;
  gchar *sequencer_group;
  gchar *osc_server_group;
  gchar *str;
  gchar *capability;
#if defined(AGS_W32API)
  gchar *app_dir;
#endif

  guint i, j;
  gboolean no_config;
  gboolean has_core_audio;
  gboolean has_pulse;
  gboolean has_jack;
  gboolean is_output;
  
  gsequencer_application_context = (AgsGSequencerApplicationContext *) application_context;

  while(!ags_ui_provider_get_gui_ready(AGS_UI_PROVIDER(application_context))){
    usleep(4);
  }
  
  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(ags_gsequencer_application_context_parent_class)->setup(application_context);

  /* config and log */
  config = ags_config_get_instance();

  log = ags_log_get_instance();

  /* main loop and task launcher */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  /* signals */
  atexit(ags_gsequencer_application_context_signal_cleanup);

  /* Ignore interactive and job-control signals.  */
#if !defined(AGS_W32API)
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  
#if !defined(AGS_W32API)
  ags_sigact.sa_handler = ags_gsequencer_application_context_signal_handler;
  sigemptyset(&ags_sigact.sa_mask);
  ags_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_sigact, (struct sigaction *) NULL);
#endif
#endif
  
  /* check filename */
  filename = NULL;
  no_config = FALSE;

  for(i = 0; i < AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argc;){
    if(!strncmp(AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i], "--filename", 11) &&
       i + 1 < AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argc &&
       AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i + 1] != NULL){      
      filename = AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i + 1];

      i += 2;
    }else if(!strncmp(AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i], "--no-config", 12)){
      no_config = TRUE;
      
      i++;
    }else{
      i++;
    }
  }
  
  for(i = 0; i < AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argc; i++){
    if(!strncmp(AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i], "--filename", 11) &&
       i + 1 < AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argc &&
       AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i + 1] != NULL){
      AgsSimpleFile *simple_file;

      xmlXPathContext *xpath_context; 
      xmlXPathObject *xpath_object;
      xmlNode **node;

      xmlChar *xpath;

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
#else
      locale_t current;
#endif

      filename = AGS_APPLICATION_CONTEXT(gsequencer_application_context)->argv[i + 1];

      if(!g_file_test(filename,
		      G_FILE_TEST_EXISTS) ||
	 !g_file_test(filename,
		      G_FILE_TEST_IS_REGULAR)){
	i += 2;

	break;
      }

      g_mutex_lock(&locale_mutex);

      if(!locale_initialized){
#if defined(AGS_OSXAPI) || defined(AGS_W32API)
	locale_env = getenv("LC_ALL");
#else
	c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
#endif
    
	locale_initialized = TRUE;
      }

      g_mutex_unlock(&locale_mutex);

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      setlocale(LC_ALL, "C");
#else
      current = uselocale(c_locale);
#endif
      
      simple_file = ags_simple_file_new();
      g_object_set(simple_file,
		   "filename", filename,
		   NULL);
      ags_simple_file_open(simple_file,
			   NULL);

      if(!no_config){
	str = g_strdup_printf("* Read config from file: %s", filename);
	ags_log_add_message(log,
			    str);

	xpath = BAD_CAST "/ags-simple-file/ags-sf-config";

	/* Create xpath evaluation context */
	xpath_context = xmlXPathNewContext(simple_file->doc);

	if(xpath_context == NULL) {
	  g_warning("Error: unable to create new XPath context");

	  goto ags_gsequencer_application_context_setup_RESTORE_LOCALE;
	}

	/* Evaluate xpath expression */
	xpath_object = xmlXPathEval(xpath, xpath_context);

	if(xpath_object == NULL) {
	  g_warning("Error: unable to evaluate xpath expression \"%s\"", xpath);

	  xmlXPathFreeContext(xpath_context); 

	  goto ags_gsequencer_application_context_setup_RESTORE_LOCALE;
	}

	node = xpath_object->nodesetval->nodeTab;
      
	for(j = 0; j < xpath_object->nodesetval->nodeNr; j++){
	  if(node[j]->type == XML_ELEMENT_NODE){
	    ags_config_clear(config);
	    ags_simple_file_read_config(simple_file, node[j], &config);
	  	  
	    break;
	  }
	}
      }else{
	gchar *str;
	
#if defined(AGS_TEST_CONFIG)
#else
	if((str = getenv("AGS_TEST_CONFIG")) != NULL){
	}else{
	  g_message("no config defaults");
	  
	  ags_config_load_defaults(config);
	}
#endif	
      }
      
    ags_gsequencer_application_context_setup_RESTORE_LOCALE:

#if defined(AGS_OSXAPI) || defined(AGS_W32API)
      setlocale(LC_ALL, locale_env);
#else
      uselocale(current);
#endif
      
      i++;
      
      break;
    }
  }

  /* get user information */
#if defined(AGS_W32API)
  application_context = ags_application_context_get_instance();

  if(strlen(application_context->argv[0]) > strlen("gsequencer.exe")){
    app_dir = g_strndup(application_context->argv[0],
			strlen(application_context->argv[0]) - strlen("gsequencer.exe"));
  }else{
    app_dir = NULL;
  }
  
  blacklist_path = g_strdup_printf("%s%c%s",
				   g_get_current_dir(),
				   G_DIR_SEPARATOR,
				   app_dir);

  g_free(app_dir);
#else
  uid = getuid();
  pw = getpwuid(uid);

#if 0
  blacklist_path = g_strdup_printf("%s%c%s",
				   pw->pw_dir,
				   G_DIR_SEPARATOR,
				   AGS_DEFAULT_DIRECTORY);
#else
  blacklist_path = g_strdup_printf("/Users/joelkrahemann/.gsequencer");  
#endif
#endif
  
  /* load ladspa manager */
  ladspa_manager = ags_ladspa_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s%c%s",
				       blacklist_path,
				       G_DIR_SEPARATOR,
				       "ladspa_plugin.blacklist");
  ags_ladspa_manager_load_blacklist(ladspa_manager,
				    blacklist_filename);

  /* load dssi manager */
  dssi_manager = ags_dssi_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s%c%s",
				       blacklist_path,
				       G_DIR_SEPARATOR,
				       "dssi_plugin.blacklist");
  ags_dssi_manager_load_blacklist(dssi_manager,
				  blacklist_filename);

  /* load lv2 manager */
  lv2_manager = ags_lv2_manager_get_instance();
  lv2_worker_manager = ags_lv2_worker_manager_get_instance();    

  blacklist_filename = g_strdup_printf("%s%c%s",
				       blacklist_path,
				       G_DIR_SEPARATOR,
				       "lv2_plugin.blacklist");
  ags_lv2_manager_load_blacklist(lv2_manager,
				 blacklist_filename);

  /* load lv2ui manager */
  lv2ui_manager = ags_lv2ui_manager_get_instance();  

  blacklist_filename = g_strdup_printf("%s/lv2ui_plugin.blacklist",
				       blacklist_path);
  ags_lv2ui_manager_load_blacklist(lv2ui_manager,
				   blacklist_filename);

  /* load vst3 manager */
#if defined(AGS_WITH_VST3)
  vst3_manager = ags_vst3_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s%c%s",
				       blacklist_path,
				       G_DIR_SEPARATOR,
				       "vst3_plugin.blacklist");
  ags_vst3_manager_load_blacklist(vst3_manager,
				  blacklist_filename);
#endif
    
  gsequencer_application_context->start_loader = TRUE;
  
  /* sound server */
  gsequencer_application_context->sound_server = NULL;

  /* core audio server */
  core_audio_server = ags_core_audio_server_new(NULL);
  gsequencer_application_context->sound_server = g_list_append(gsequencer_application_context->sound_server,
							       core_audio_server);
  g_object_ref(G_OBJECT(core_audio_server));

  has_core_audio = FALSE;

  /* pulse server */
  pulse_server = ags_pulse_server_new(NULL);
  gsequencer_application_context->sound_server = g_list_append(gsequencer_application_context->sound_server,
							       pulse_server);
  g_object_ref(G_OBJECT(pulse_server));

  has_pulse = FALSE;
  
  /* jack server */
  jack_server = ags_jack_server_new(NULL);
  gsequencer_application_context->sound_server = g_list_append(gsequencer_application_context->sound_server,
							       jack_server);
  g_object_ref(G_OBJECT(jack_server));

  has_jack = FALSE;

  /* AgsSoundcard */
  gsequencer_application_context->soundcard = NULL;
  soundcard = NULL;

  soundcard_group = g_strdup("soundcard");
  
  for(i = 0; ; i++){
    guint pcm_channels, buffer_size, samplerate, format;
    guint cache_buffer_size;
    gboolean use_cache;
    
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

    capability = ags_config_get_value(config,
				      soundcard_group,
				      "capability");

    is_output = TRUE;

    if(capability != NULL &&
       !g_ascii_strncasecmp(capability,
			    "capture",
			    8)){
      is_output = FALSE;
    }

    /* change soundcard */
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "core-audio",
			      11)){
	soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(core_audio_server),
							is_output);

	has_core_audio = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "pulse",
				    6)){
	soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(pulse_server),
							is_output);

	has_pulse = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "jack",
				    5)){
	if(!is_output){
	  AgsJackClient *input_client;

	  g_object_get(jack_server,
		       "input-jack-client", &input_client,
		       NULL);

	  if(input_client == NULL){
	    input_client = ags_jack_client_new((GObject *) jack_server);
	    g_object_set(jack_server,
			 "input-jack-client", input_client,
			 NULL);
	    ags_jack_server_add_client(jack_server,
				       (GObject *) input_client);
    
	    ags_jack_client_open((AgsJackClient *) input_client,
				 "ags-input-client");	    
	  }else{
	    g_object_unref(input_client);
	  }
	}
	
	soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
							is_output);
	
	has_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	if(is_output){
	  soundcard = (GObject *) ags_alsa_devout_new();
	}else{
	  soundcard = (GObject *) ags_alsa_devin_new();
	}
      }else if(!g_ascii_strncasecmp(str,
				    "wasapi",
				    7)){
	gchar *str;
	
	if(is_output){
	  soundcard = (GObject *) ags_wasapi_devout_new((GObject *) gsequencer_application_context);	  

	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-share-mode");

	  if(str != NULL &&
	     !g_ascii_strncasecmp(str,
				  "exclusive",
				  10)){
	    ags_wasapi_devout_set_flags(AGS_WASAPI_DEVOUT(soundcard),
					AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE);
	  }else{
	    ags_wasapi_devout_unset_flags(AGS_WASAPI_DEVOUT(soundcard),
					  AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE);
	  }

	  g_free(str);
	  
	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-buffer-size");

	  if(str != NULL){
	    AGS_WASAPI_DEVOUT(soundcard)->wasapi_buffer_size = g_ascii_strtoull(str,
										NULL,
										10);
	    
	    g_free(str);
	  }
	}else{
	  soundcard = (GObject *) ags_wasapi_devin_new((GObject *) gsequencer_application_context);

	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-share-mode");

	  if(str != NULL &&
	     !g_ascii_strncasecmp(str,
				  "exclusive",
				  10)){
	    ags_wasapi_devin_set_flags(AGS_WASAPI_DEVIN(soundcard),
				       AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE);
	  }else{
	    ags_wasapi_devin_unset_flags(AGS_WASAPI_DEVIN(soundcard),
					 AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE);
	  }

	  g_free(str);
	  
	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-buffer-size");

	  if(str != NULL){
	    AGS_WASAPI_DEVIN(soundcard)->wasapi_buffer_size = g_ascii_strtoull(str,
									       NULL,
									       10);
	    
	    g_free(str);
	  }
	}
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	if(is_output){
	  soundcard = (GObject *) ags_oss_devout_new();
	}else{
	  soundcard = (GObject *) ags_oss_devin_new();
	}
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
    
    gsequencer_application_context->soundcard = g_list_append(gsequencer_application_context->soundcard,
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

    use_cache = TRUE;
    str = ags_config_get_value(config,
			       soundcard_group,
			       "use-cache");

    if(str != NULL &&
       !g_ascii_strncasecmp(str,
			    "false",
			    5)){
      use_cache = FALSE;
    }

    cache_buffer_size = 4096;
    str = ags_config_get_value(config,
			       soundcard_group,
			       "cache-buffer-size");

    if(str != NULL){
      cache_buffer_size = g_ascii_strtoull(str,
					   NULL,
					   10);
    }

    if(AGS_IS_PULSE_DEVOUT(soundcard)){
      GList *start_port, *port;

      g_object_get(soundcard,
		   "pulse-port", &start_port,
		   NULL);

      port = start_port;

      while(port != NULL){
	ags_pulse_port_set_samplerate(port->data,
				      samplerate);
	ags_pulse_port_set_pcm_channels(port->data,
					pcm_channels);
	ags_pulse_port_set_buffer_size(port->data,
				       buffer_size);
	ags_pulse_port_set_format(port->data,
				  format);
	ags_pulse_port_set_cache_buffer_size(port->data,
					     buffer_size * ceil(cache_buffer_size / buffer_size));
	
	port = port->next;
      }

      g_list_free_full(start_port,
		       g_object_unref);
    }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
      GList *start_port, *port;

      g_object_get(soundcard,
		   "core-audio-port", &start_port,
		   NULL);

      port = start_port;

      while(port != NULL){
	ags_core_audio_port_set_samplerate(port->data,
					   samplerate);
	ags_core_audio_port_set_pcm_channels(port->data,
					     pcm_channels);
	ags_core_audio_port_set_buffer_size(port->data,
					    buffer_size);
	ags_core_audio_port_set_format(port->data,
				       format);
	ags_core_audio_port_set_cache_buffer_size(port->data,
						  buffer_size * ceil(cache_buffer_size / buffer_size));
	
	port = port->next;
      }

      g_list_free_full(start_port,
		       g_object_unref);
    }
    
    g_free(soundcard_group);    
    soundcard_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SOUNDCARD,
				      i);
  }

  if(gsequencer_application_context->soundcard != NULL){
    soundcard = gsequencer_application_context->soundcard->data;
  }  

  g_free(soundcard_group);

  ags_sound_provider_set_default_soundcard(AGS_SOUND_PROVIDER(gsequencer_application_context),
					   soundcard);
  
  /* AgsSequencer */
  gsequencer_application_context->sequencer = NULL;
  sequencer = NULL;

  sequencer_group = g_strdup("sequencer");
  
  for(i = 0; ; i++){
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
	AgsJackClient *input_client;

	g_object_get(jack_server,
		     "input-jack-client", &input_client,
		     NULL);

	if(input_client == NULL){
	  input_client = ags_jack_client_new((GObject *) jack_server);
	  g_object_set(jack_server,
		       "input-jack-client", input_client,
		       NULL);
	  ags_jack_server_add_client(jack_server,
				     (GObject *) input_client);
    
	  ags_jack_client_open((AgsJackClient *) input_client,
			       "ags-input-client");	    
	}else{
	  g_object_unref(input_client);
	}

	sequencer = ags_sound_server_register_sequencer(AGS_SOUND_SERVER(jack_server),
							FALSE);

	has_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	sequencer = (GObject *) ags_alsa_midiin_new();
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	sequencer = (GObject *) ags_oss_midiin_new();
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
    
    gsequencer_application_context->sequencer = g_list_append(gsequencer_application_context->sequencer,
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
  
  /* AgsServer */
  gsequencer_application_context->server = NULL;
  server = NULL;

  server_group = g_strdup("server");
  
  for(i = 0; ; i++){
    gchar *ip4, *ip6;

    guint server_port;
    gboolean auto_start;
    gboolean any_address;
    gboolean enable_ip4, enable_ip6;
    
    if(!g_key_file_has_group(config->key_file,
			     server_group)){
      if(i == 0){
	g_free(server_group);    
	server_group = g_strdup_printf("%s-%d",
				       AGS_CONFIG_SERVER,
				       i);
    	
	continue;
      }else{
	break;
      }
    }

    server = ags_server_new();

    gsequencer_application_context->server = g_list_append(gsequencer_application_context->server,
							   server);
    g_object_ref(server);

    /* realm */
    str = ags_config_get_value(config,
			       server_group,
			       "realm");
    
    if(str != NULL){
      g_object_set(server,
		   "realm", str,
		   NULL);
      
      g_free(str);
    }

    /* any address */
    any_address = FALSE;

    str = ags_config_get_value(config,
			       server_group,
			       "any-address");
    
    if(str != NULL){
      any_address = (!g_ascii_strncasecmp(str,
					  "true",
					  5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(any_address){
      ags_server_set_flags(server,
			   (AGS_SERVER_ANY_ADDRESS));
    }

    /* enable ip4 and ip6 */
    enable_ip4 = FALSE;
    enable_ip6 = FALSE;

    str = ags_config_get_value(config,
			       server_group,
			       "enable-ip4");
    
    if(str != NULL){
      enable_ip4 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    str = ags_config_get_value(config,
			       server_group,
			       "enable-ip6");

    if(str != NULL){
      enable_ip6 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(enable_ip4){
      ags_server_set_flags(server,
			   (AGS_SERVER_INET4));
    }

    if(enable_ip6){
      ags_server_set_flags(server,
			   (AGS_SERVER_INET6));
    }

    /* ip4 and ip6 address */
    ip4 = ags_config_get_value(config,
			       server_group,
			       "ip4-address");

    if(ip4 != NULL){
      g_object_set(server,
		   "ip4", ip4,
		   NULL);
      
      g_free(ip4);
    }

    ip6 = ags_config_get_value(config,
			       server_group,
			       "ip6-address");

    if(ip6 != NULL){
      g_object_set(server,
		   "ip6", ip6,
		   NULL);
      
      g_free(ip6);
    }

    /* server port */
    str = ags_config_get_value(config,
			       server_group,
			       "server-port");

    if(str != NULL){
      server_port = (guint) g_ascii_strtoull(str,
					     NULL,
					     10);

      g_object_set(server,
		   "server-port", server_port,
		   NULL);
    }
    
    /* auto-start */
    auto_start = FALSE;
    
    str = ags_config_get_value(config,
			       server_group,
			       "auto-start");

    if(str != NULL){
      auto_start = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(auto_start){
      ags_server_set_flags(server, AGS_SERVER_AUTO_START);
    }

    g_free(server_group);    
    server_group = g_strdup_printf("%s-%d",
				   AGS_CONFIG_SERVER,
				   i);
  }

  if(server == NULL){
    server = ags_server_new();
    ags_server_set_flags(server,
			 (AGS_SERVER_INET4));

    gsequencer_application_context->server = g_list_append(gsequencer_application_context->server,
							   server);
    g_object_ref(server);
  }
  
  /* AgsSoundcardThread and AgsExportThread */
  gsequencer_application_context->default_soundcard_thread = NULL;
  list = gsequencer_application_context->soundcard;
    
  while(list != NULL){
    guint soundcard_capability;

    soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(list->data));
    
    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(list->data,
							      soundcard_capability);
    ags_thread_add_child_extended(main_loop,
				  soundcard_thread,
				  TRUE, TRUE);

    /* export thread */
    export_thread = NULL;
    
    /* export thread */
    if(AGS_IS_ALSA_DEVOUT(list->data) ||
       AGS_IS_OSS_DEVOUT(list->data) ||
       AGS_IS_WASAPI_DEVOUT(list->data) ||
       AGS_IS_JACK_DEVOUT(list->data) ||
       AGS_IS_PULSE_DEVOUT(list->data) ||
       AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
      export_thread = (AgsThread *) ags_export_thread_new(list->data,
							  NULL);
      ags_thread_add_child_extended(main_loop,
				    (AgsThread *) export_thread,
				    TRUE, TRUE);
    }    

    /* default soundcard thread */
    if(gsequencer_application_context->default_soundcard_thread == NULL){
      ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(gsequencer_application_context),
						      (GObject *) soundcard_thread);
    }

    /* default export thread */
    if(export_thread != NULL){
      if(gsequencer_application_context->default_export_thread == NULL){
	gsequencer_application_context->default_export_thread = export_thread;
      }else{
	g_object_unref(export_thread);
      }
    }

    /* iterate */
    list = list->next;      
  }
  
  /* AgsSequencerThread */
  list = gsequencer_application_context->sequencer;
    
  while(list != NULL){
    sequencer_thread = (AgsThread *) ags_sequencer_thread_new(list->data);
    ags_thread_add_child_extended(main_loop,
				  (AgsThread *) sequencer_thread,
				  TRUE, TRUE);

    list = list->next;      
  }

  /* AgsOscServer */
  gsequencer_application_context->osc_server = NULL;
  osc_server = NULL;

  osc_server_group = g_strdup("osc-server");
  
  for(i = 0; ; i++){
    gchar *ip4, *ip6;

    guint server_port;
    gboolean auto_start;
    gboolean any_address;
    gboolean enable_ip4, enable_ip6;
    
    if(!g_key_file_has_group(config->key_file,
			     osc_server_group)){
      if(i == 0){
	g_free(osc_server_group);    
	osc_server_group = g_strdup_printf("%s-%d",
					   AGS_CONFIG_OSC_SERVER,
					   i);
    	
	continue;
      }else{
	break;
      }
    }

    osc_server = ags_osc_server_new();
    ags_osc_server_set_flags(osc_server,
			     (AGS_OSC_SERVER_TCP));

    gsequencer_application_context->osc_server = g_list_append(gsequencer_application_context->osc_server,
							       osc_server);
    g_object_ref(osc_server);

    /* any address */
    any_address = FALSE;

    str = ags_config_get_value(config,
			       osc_server_group,
			       "any-address");
    
    if(str != NULL){
      any_address = (!g_ascii_strncasecmp(str,
					  "true",
					  5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(any_address){
      ags_osc_server_set_flags(osc_server,
			       (AGS_OSC_SERVER_ANY_ADDRESS));
    }

    /* enable ip4 and ip6 */
    enable_ip4 = FALSE;
    enable_ip6 = FALSE;

    str = ags_config_get_value(config,
			       osc_server_group,
			       "enable-ip4");
    
    if(str != NULL){
      enable_ip4 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    str = ags_config_get_value(config,
			       osc_server_group,
			       "enable-ip6");

    if(str != NULL){
      enable_ip6 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(enable_ip4){
      ags_osc_server_set_flags(osc_server,
			       (AGS_OSC_SERVER_INET4));
    }

    if(enable_ip6){
      ags_osc_server_set_flags(osc_server,
			       (AGS_OSC_SERVER_INET6));
    }
    
    ags_osc_server_add_default_controller(osc_server);

    /* ip4 and ip6 address */
    str = ags_config_get_value(config,
			       osc_server_group,
			       "ip4-address");

    if(str != NULL){
      g_object_set(osc_server,
		   "ip4", str,
		   NULL);
      
      g_free(str);
    }

    str = ags_config_get_value(config,
			       osc_server_group,
			       "ip6-address");

    if(str != NULL){
      g_object_set(osc_server,
		   "ip6", str,
		   NULL);
      
      g_free(str);
    }

    /* server port */
    str = ags_config_get_value(config,
			       osc_server_group,
			       "server-port");

    if(str != NULL){
      server_port = (guint) g_ascii_strtoull(str,
					     NULL,
					     10);

      g_object_set(osc_server,
		   "server-port", server_port,
		   NULL);
    }
    
    /* auto-start */
    auto_start = FALSE;
    
    str = ags_config_get_value(config,
			       osc_server_group,
			       "auto-start");

    if(str != NULL){
      auto_start = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(auto_start){
      ags_osc_server_set_flags(osc_server, AGS_OSC_SERVER_AUTO_START);
    }

    g_free(osc_server_group);    
    osc_server_group = g_strdup_printf("%s-%d",
				       AGS_CONFIG_OSC_SERVER,
				       i);
  }

  if(osc_server == NULL){
    osc_server = ags_osc_server_new();
    ags_osc_server_set_flags(osc_server,
			     (AGS_OSC_SERVER_INET4 |
			      AGS_OSC_SERVER_TCP));

    ags_osc_server_add_default_controller(osc_server);

    gsequencer_application_context->osc_server = g_list_append(gsequencer_application_context->osc_server,
							       osc_server);
    g_object_ref(osc_server);
  }
  
  /* AgsWorkerThread */
  gsequencer_application_context->worker = NULL;

  /* AgsDestroyWorker */
  destroy_worker = ags_destroy_worker_get_instance();
  ags_thread_add_child_extended(main_loop,
				(AgsThread *) destroy_worker,
				TRUE, TRUE);
  gsequencer_application_context->worker = g_list_prepend(gsequencer_application_context->worker,
							  destroy_worker);
  ags_thread_start((AgsThread *) destroy_worker);
  
  /* AgsThreadPool */
  gsequencer_application_context->thread_pool = NULL;

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

  gsequencer_application_context->setup_ready = TRUE;
  
  /* unref */
  g_object_unref(main_loop);
  
  g_object_unref(task_launcher);
}

void
ags_gsequencer_application_context_register_types(AgsApplicationContext *application_context)
{
  ags_complex_get_type();

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
    
  /* gui */
  //TODO:JK: move me
  ags_led_get_type();
  ags_indicator_get_type();
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

#ifdef AGS_WITH_LIBINSTPATCH
  ags_ffplayer_get_type();
#endif
  
  ags_ladspa_bridge_get_type();
  ags_lv2_bridge_get_type();
  ags_dssi_bridge_get_type();
}

void
ags_gsequencer_application_context_quit(AgsApplicationContext *application_context)
{
  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;

#if defined(AGS_WITH_VST3)
  AgsVst3Manager *vst3_manager;
#endif

  AgsCoreAudioServer *core_audio_server;

  AgsPulseServer *pulse_server;

  AgsJackServer *jack_server;

  AgsTaskLauncher *task_launcher;
  AgsStopThread *stop_thread;
  
  GList *core_audio_client;
  GList *jack_client;
  GList *start_list, *list;

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  stop_thread = ags_stop_thread_new();
  ags_task_launcher_add_task(task_launcher,
			     stop_thread);

  g_usleep(2 * G_USEC_PER_SEC);
  
  /* free managers */
  ladspa_manager = ags_ladspa_manager_get_instance();
  g_object_unref(ladspa_manager);

  dssi_manager = ags_dssi_manager_get_instance();
  g_object_unref(dssi_manager);

  lv2_manager = ags_lv2_manager_get_instance();
  g_object_unref(lv2_manager);

#if defined(AGS_WITH_VST3)
  vst3_manager = ags_vst3_manager_get_instance();

  list = vst3_manager->vst3_plugin;

  while(list != NULL){
    if(AGS_VST3_PLUGIN(list->data)->plugin_exit != NULL){
      AGS_VST3_PLUGIN(list->data)->plugin_exit();
    }
    
    list = list->next;
  }
  
  g_object_unref(vst3_manager);
#endif
  
  /* retrieve core audio server */
  start_list = 
    list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
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

  g_list_free_full(start_list,
		   g_object_unref);
  
  /* retrieve pulseaudio server */
  start_list = 
    list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
  while((list = ags_list_util_find_type(list,
					AGS_TYPE_PULSE_SERVER)) != NULL){
    pulse_server = list->data;

#ifdef AGS_WITH_PULSE
    if(pulse_server->main_loop != NULL){
      pa_mainloop_quit(pulse_server->main_loop,
		       0);
    }
#endif
 
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  /* retrieve JACK server */
  start_list = 
    list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
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

  g_list_free_full(start_list,
		   g_object_unref);

  g_application_quit(ags_ui_provider_get_app(AGS_UI_PROVIDER(application_context)));
}

void
ags_gsequencer_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context)
{
  //TODO:JK: implement me
}

xmlNode*
ags_gsequencer_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context)
{  
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void*
ags_gsequencer_application_context_server_main_loop_thread(GMainLoop *main_loop)
{
  AgsApplicationContext *application_context;

  GList *start_list, *list;

  g_main_context_push_thread_default(g_main_loop_get_context(main_loop));
  
  application_context = ags_application_context_get_instance();
  
  while(ags_ui_provider_get_show_animation(AGS_UI_PROVIDER(application_context))){
    g_usleep(G_USEC_PER_SEC / 30);
  }

  list = 
    start_list = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(application_context));

  while(list != NULL){
    if(ags_server_test_flags(list->data, AGS_SERVER_AUTO_START)){
      ags_server_start(AGS_SERVER(list->data));
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  g_main_loop_run(main_loop);

  g_thread_exit(NULL);

  return(NULL);
}

void*
ags_gsequencer_application_context_audio_main_loop_thread(GMainLoop *main_loop)
{
  AgsApplicationContext *application_context;

  GList *start_list, *list;

#ifdef AGS_WITH_RT
  AgsPriority *priority;

  struct sched_param param;

  gchar *str;
#endif

  g_main_context_push_thread_default(g_main_loop_get_context(main_loop));
  
  application_context = ags_application_context_get_instance();

  while(ags_ui_provider_get_show_animation(AGS_UI_PROVIDER(application_context))){
    g_usleep(G_USEC_PER_SEC / 30);
  }

  /* real-time setup */
#ifdef AGS_WITH_RT
  priority = ags_priority_get_instance();  

  param.sched_priority = 95;

  str = ags_priority_get_value(priority,
			       AGS_PRIORITY_RT_THREAD,
			       AGS_PRIORITY_KEY_AUDIO_MAIN_LOOP);

  if(str != NULL){
    param.sched_priority = (int) g_ascii_strtoull(str,
						  NULL,
						  10);
  }

  if(str == NULL ||
     ((!g_ascii_strncasecmp(str,
			    "0",
			    2)) != TRUE)){
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed");
    }
  }
    
  g_free(str);
#endif
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    if(ags_osc_server_test_flags(list->data, AGS_OSC_SERVER_AUTO_START)){
      ags_osc_server_start(AGS_OSC_SERVER(list->data));
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  g_main_loop_run(main_loop);

  g_thread_exit(NULL);

  return(NULL);
}

gboolean
ags_gsequencer_application_context_message_monitor_timeout(AgsGSequencerApplicationContext *gsequencer_application_context)
{
  ags_ui_provider_check_message(AGS_UI_PROVIDER(gsequencer_application_context));
  ags_ui_provider_clean_message(AGS_UI_PROVIDER(gsequencer_application_context));

  return(TRUE);
}

gboolean
ags_gsequencer_application_context_task_timeout(AgsGSequencerApplicationContext *gsequencer_application_context)
{
  AgsTaskLauncher *task_launcher;

  if(gsequencer_application_context->task == NULL){
    return(TRUE);
  }
  
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(gsequencer_application_context));

  ags_task_launcher_add_task_all(task_launcher,
				 g_list_reverse(gsequencer_application_context->task));

  g_list_free_full(gsequencer_application_context->task,
		   g_object_unref);
  
  gsequencer_application_context->task = NULL;
  
  g_object_unref(task_launcher);

  return(TRUE);
}

gint
ags_gsequencer_application_context_compare_strv(gconstpointer a,
						gconstpointer b)
{
  return(g_strcmp0(((gchar **) a)[1], ((gchar **) b)[1]));
}

gboolean
ags_gsequencer_application_context_loader_timeout(AgsGSequencerApplicationContext *gsequencer_application_context)
{
  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;
  AgsLv2TurtleScanner *lv2_turtle_scanner;  

#if defined(AGS_WITH_VST3)
  AgsVst3Manager *vst3_manager;
#endif
  
  AgsLog *log;

#if !defined(AGS_W32API)
  struct utsname buf;
#endif
  
  gchar **ladspa_path;
  gchar **dssi_path;
  gchar **lv2_path;
  gchar **vst3_path;

  gchar *sysname;

  gint64 start_time;
  gint64 current_time;
  gboolean initial_load;

  GError *error;

  if(!gsequencer_application_context->start_loader){
    return(TRUE);
  }
  
  log = ags_log_get_instance();
  
  if(gsequencer_application_context->loader_ready &&
     gsequencer_application_context->ladspa_loader == NULL &&
     gsequencer_application_context->dssi_loader == NULL &&
     gsequencer_application_context->lv2_loader == NULL &&
     gsequencer_application_context->vst3_loader == NULL){
    ags_log_add_message(log,
			"* Launch user interface");

    while(!gsequencer_application_context->setup_ready){
      g_main_context_iteration(NULL,
			       FALSE);
    }
    
    /* stop animation */
    gsequencer_application_context->loader_completed = TRUE;

    ags_ui_provider_set_show_animation(AGS_UI_PROVIDER(gsequencer_application_context), FALSE);
      
    return(FALSE);
  }

  start_time = g_get_monotonic_time();

  initial_load = TRUE;

#if !defined(AGS_W32API)
  uname(&buf);

  sysname = g_ascii_strdown(buf.sysname,
			    -1);
#else
  sysname = g_strdup("win");
#endif
  
  /* get plugin managers */
  ladspa_manager = ags_ladspa_manager_get_instance();
  
  dssi_manager = ags_dssi_manager_get_instance();
  
  lv2_manager = ags_lv2_manager_get_instance();
  
#if defined(AGS_WITH_VST3)
  vst3_manager = ags_vst3_manager_get_instance();
#endif
  
  ladspa_path = ags_ladspa_manager_get_default_path();

  dssi_path = ags_dssi_manager_get_default_path();

  lv2_path = ags_lv2_manager_get_default_path();

#if defined(AGS_WITH_VST3)
  vst3_path = ags_vst3_manager_get_default_path();
#endif
  
  if(gsequencer_application_context->lv2_turtle_scanner == NULL){
    gsequencer_application_context->lv2_turtle_scanner = ags_lv2_turtle_scanner_new();
  }
  
  lv2_turtle_scanner = gsequencer_application_context->lv2_turtle_scanner;
    
  if(!gsequencer_application_context->loader_ready){
    AgsBasePlugin *plugin;
    
    GDir *dir;

    gchar *filename;

    /* prepare loader - ladspa */
    while(*ladspa_path != NULL){
      if(!g_file_test(*ladspa_path,
		      G_FILE_TEST_EXISTS)){
	ladspa_path++;
      
	continue;
      }
    
      error = NULL;
      dir = g_dir_open(*ladspa_path,
		       0,
		       &error);

      if(error != NULL){
	g_warning("%s", error->message);

	ladspa_path++;

	g_error_free(error);
      
	continue;
      }

      while((filename = g_dir_read_name(dir)) != NULL){
	if(g_str_has_suffix(filename,
			    AGS_LIBRARY_SUFFIX) &&
	   !g_list_find_custom(ladspa_manager->ladspa_plugin_blacklist,
			       filename,
			       g_strcmp0)){
	  gsequencer_application_context->ladspa_loader = g_list_prepend(gsequencer_application_context->ladspa_loader,
									 g_strdup_printf("%s%c%s",
											 ladspa_path[0],
											 G_DIR_SEPARATOR,
											 filename));
	}
      }
    
      ladspa_path++;
    }    

    gsequencer_application_context->ladspa_loader = g_list_reverse(gsequencer_application_context->ladspa_loader);

    /* prepare loader - dssi */
    while(*dssi_path != NULL){
      if(!g_file_test(*dssi_path,
		      G_FILE_TEST_EXISTS)){
	dssi_path++;
      
	continue;
      }
    
      error = NULL;
      dir = g_dir_open(*dssi_path,
		       0,
		       &error);

      if(error != NULL){
	g_warning("%s", error->message);

	dssi_path++;

	g_error_free(error);
      
	continue;
      }

      while((filename = g_dir_read_name(dir)) != NULL){
	if(g_str_has_suffix(filename,
			    AGS_LIBRARY_SUFFIX) &&
	   !g_list_find_custom(dssi_manager->dssi_plugin_blacklist,
			       filename,
			       g_strcmp0)){
	  gsequencer_application_context->dssi_loader = g_list_prepend(gsequencer_application_context->dssi_loader,
								       g_strdup_printf("%s%c%s",
										       dssi_path[0],
										       G_DIR_SEPARATOR,
										       filename));
	}
      }
    
      dssi_path++;
    }    

    gsequencer_application_context->dssi_loader = g_list_reverse(gsequencer_application_context->dssi_loader);	

    /* prepare loader - lv2 */    
    while(*lv2_path != NULL){
      gchar *plugin_path;
      
      if(!g_file_test(*lv2_path,
		      G_FILE_TEST_EXISTS)){
	lv2_path++;
      
	continue;
      }
    
      error = NULL;
      dir = g_dir_open(*lv2_path,
		       0,
		       &error);

      if(error != NULL){
	g_warning("%s", error->message);

	lv2_path++;

	g_error_free(error);
      
	continue;
      }

      while((filename = g_dir_read_name(dir)) != NULL){
	if(!g_ascii_strncasecmp(filename,
				"..",
				3) ||
	   !g_ascii_strncasecmp(filename,
				".",
				2)){
	  continue;
	}

	plugin_path = g_strdup_printf("%s%c%s",
				      lv2_path[0],
				      G_DIR_SEPARATOR,
				      filename);
      
	if(g_file_test(plugin_path,
		       G_FILE_TEST_IS_DIR) &&
	   !g_list_find_custom(lv2_manager->lv2_plugin_blacklist,
			       filename,
			       g_strcmp0)){
	  gsequencer_application_context->lv2_loader = g_list_prepend(gsequencer_application_context->lv2_loader,
								      g_strdup(plugin_path));
	}
      }
            
      lv2_path++;
    }

    gsequencer_application_context->lv2_loader = g_list_reverse(gsequencer_application_context->lv2_loader);

#if defined(AGS_WITH_VST3)
    while(vst3_path[0] != NULL){    
      gchar *plugin_path;
      
      if(!g_file_test(vst3_path[0],
		      G_FILE_TEST_EXISTS)){
	vst3_path++;
      
	continue;
      }

      error = NULL;
      dir = g_dir_open(vst3_path[0],
		       0,
		       &error);

      if(error != NULL){
	g_warning("%s", error->message);

	vst3_path++;

	g_error_free(error);
      
	continue;
      }


      while((filename = g_dir_read_name(dir)) != NULL){
	if(!g_ascii_strncasecmp(filename,
				"..",
				3) ||
	   !g_ascii_strncasecmp(filename,
				".",
				2)){
	  continue;
	}
      
	plugin_path = g_strdup_printf("%s%c%s",
				      vst3_path[0],
				      G_DIR_SEPARATOR,
				      filename);
      
	if(g_file_test(plugin_path,
		       G_FILE_TEST_IS_DIR) &&
	   !g_list_find_custom(vst3_manager->vst3_plugin_blacklist,
			       filename,
			       g_strcmp0)){
	  gsequencer_application_context->vst3_loader = g_list_prepend(gsequencer_application_context->vst3_loader,
								       g_strdup(plugin_path));
	}
      }
      
      vst3_path++;
    }

    gsequencer_application_context->vst3_loader = g_list_reverse(gsequencer_application_context->vst3_loader);
#endif
    
    gsequencer_application_context->loader_ready = TRUE;
  }
  
  /* load ladspa */
  current_time = g_get_monotonic_time();
  
  if(current_time < start_time + AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL &&
     !gsequencer_application_context->ladspa_loading){
    ags_log_add_message(log,
			"* Loading LADSPA plugins");

    gsequencer_application_context->ladspa_loading = TRUE;
  }
  
  while(gsequencer_application_context->ladspa_loader != NULL){
    gchar *loader_filename;
    gchar *path;
    gchar *filename;
    
    current_time = g_get_monotonic_time();
    
    if(!initial_load &&
       current_time > start_time + AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL){
      break;
    }

    loader_filename = gsequencer_application_context->ladspa_loader->data;

    path = g_path_get_dirname(loader_filename);
    filename = g_path_get_basename(loader_filename);

    if(path != NULL && filename != NULL){
      ags_ladspa_manager_load_file(ladspa_manager,
				   path,
				   filename);
    }
    
    gsequencer_application_context->ladspa_loader = g_list_remove(gsequencer_application_context->ladspa_loader,
								  loader_filename);
    
    initial_load = FALSE;
  }

  /* load dssi */
  current_time = g_get_monotonic_time();
  
  if(current_time < start_time + AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL &&
     !gsequencer_application_context->dssi_loading){
    ags_log_add_message(log,
			"* Loading DSSI plugins");

    gsequencer_application_context->dssi_loading = TRUE;
  }
  
  while(gsequencer_application_context->dssi_loader != NULL){
    gchar *loader_filename;
    gchar *path;
    gchar *filename;

    current_time = g_get_monotonic_time();
    
    if(!initial_load &&
       current_time > start_time + AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL){
      break;
    }

    loader_filename = gsequencer_application_context->dssi_loader->data;

    path = g_path_get_dirname(loader_filename);
    filename = g_path_get_basename(loader_filename);
    
    if(path != NULL && filename != NULL){
      ags_dssi_manager_load_file(dssi_manager,
				 path,
				 filename);
    }
    
    gsequencer_application_context->dssi_loader = g_list_remove(gsequencer_application_context->dssi_loader,
								loader_filename);
    
    initial_load = FALSE;
  }

  /* load lv2 */
  current_time = g_get_monotonic_time();
  
  if(current_time < start_time + AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL &&
     !gsequencer_application_context->lv2_loading){
    ags_log_add_message(log,
			"* Loading LV2 plugins");

    gsequencer_application_context->lv2_loading = TRUE;
  }
  
  while(gsequencer_application_context->lv2_loader != NULL){
    gchar *loader_filename;
    
    current_time = g_get_monotonic_time();
    
    if(!initial_load &&
       current_time > start_time + AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL){
      break;
    }

    loader_filename = gsequencer_application_context->lv2_loader->data;

    if(g_file_test(loader_filename,
		   G_FILE_TEST_IS_DIR)){
      AgsLv2TurtleParser *lv2_turtle_parser;
	
      AgsTurtle *manifest;
      AgsTurtle **turtle;

      GList *start_list, *list;

      gchar *manifest_filename;
	
      guint n_turtle;
	
      manifest_filename = g_strdup_printf("%s%c%s",
					  loader_filename,
					  G_DIR_SEPARATOR,
					  "manifest.ttl");

      if(!g_file_test(manifest_filename,
		      G_FILE_TEST_EXISTS)){
	gsequencer_application_context->lv2_loader = g_list_remove(gsequencer_application_context->lv2_loader,
								   loader_filename);

	g_free(manifest_filename);
	  
	continue;
      }

      g_message("quick scan turtle [Manifest] - %s", manifest_filename);
	
      ags_lv2_turtle_scanner_quick_scan(lv2_turtle_scanner,
					manifest_filename);
      
      g_free(manifest_filename);
      
    }
      
    gsequencer_application_context->lv2_loader = g_list_remove(gsequencer_application_context->lv2_loader,
							       loader_filename);
    
    initial_load = FALSE;
  }

  if(gsequencer_application_context->lv2_loading &&
     gsequencer_application_context->lv2_loader == NULL){
    GDir *dir;

    GList *start_lv2_cache_turtle, *lv2_cache_turtle;

    GList *start_plugin;
    GList *start_instrument;
  
    gchar **quick_scan_plugin_filename;
    gchar **quick_scan_plugin_effect;
  
    gchar **quick_scan_instrument_filename;
    gchar **quick_scan_instrument_effect;  

    gchar **lv2_path;
    gchar *path, *plugin_path;
    gchar *str;

    GError *error;

    /* read plugins */
    lv2_cache_turtle =
      start_lv2_cache_turtle = g_list_reverse(g_list_copy(lv2_turtle_scanner->cache_turtle));

    start_plugin = NULL;
    start_instrument = NULL; 
  
    quick_scan_plugin_filename = NULL;
    quick_scan_plugin_effect = NULL;

    quick_scan_instrument_filename = NULL;
    quick_scan_instrument_effect = NULL;

    while(lv2_cache_turtle != NULL){
      AgsLv2CacheTurtle *current;

      current = AGS_LV2_CACHE_TURTLE(lv2_cache_turtle->data);

      if(g_str_has_suffix(current->turtle_filename,
			  "manifest.ttl")){
	GList *start_list, *list;
      
	list =
	  start_list = g_hash_table_get_keys(current->plugin_filename);
      
	while(list != NULL){
	  gchar **strv;
	
	  gchar *filename;
	  gchar *effect;

	  gboolean is_instrument;

	  filename = g_hash_table_lookup(current->plugin_filename,
					 list->data);

	  effect = g_hash_table_lookup(current->plugin_effect,
				       list->data);
	
	  is_instrument = FALSE;

	  if(g_hash_table_contains(current->is_instrument,
				   list->data)){
	    is_instrument = TRUE;
	  }

	  strv = g_malloc(3 * sizeof(gchar *));	
	
	  strv[0] = g_strdup(filename);
	  strv[1] = g_strdup(effect);
	  strv[2] = NULL;
	
	  if(!is_instrument){
	    start_plugin = g_list_insert_sorted(start_plugin,
						strv,
						(GCompareFunc) ags_gsequencer_application_context_compare_strv);
	  }else{
	    start_instrument = g_list_insert_sorted(start_instrument,
						    strv,
						    (GCompareFunc) ags_gsequencer_application_context_compare_strv);
	  }
	
	  list = list->next;
	}

	g_list_free(start_list);
      }
    
      lv2_cache_turtle = lv2_cache_turtle->next;
    }

    g_list_free(start_lv2_cache_turtle);

    if(start_plugin != NULL){
      GList *plugin;
    
      guint length;
      guint i;

      plugin = start_plugin;
    
      length = g_list_length(start_plugin);
    
      quick_scan_plugin_filename = (gchar **) g_malloc((length + 1) * sizeof(gchar *));
      quick_scan_plugin_effect = (gchar **) g_malloc((length + 1) * sizeof(gchar *));

      for(i = 0; i < length; i++){
	quick_scan_plugin_filename[i] = ((gchar **) plugin->data)[0];
	quick_scan_plugin_effect[i] = ((gchar **) plugin->data)[1];
      
	plugin = plugin->next;
      }
    
      quick_scan_plugin_filename[i] = NULL;
      quick_scan_plugin_effect[i] = NULL;
    }
  
    if(start_instrument != NULL){
      GList *instrument;
    
      guint length;
      guint i;

      instrument = start_instrument;
    
      length = g_list_length(start_instrument);
    
      quick_scan_instrument_filename = (gchar **) g_malloc((length + 1) * sizeof(gchar *));
      quick_scan_instrument_effect = (gchar **) g_malloc((length + 1) * sizeof(gchar *));

      for(i = 0; i < length; i++){
	quick_scan_instrument_filename[i] = ((gchar **) instrument->data)[0];
	quick_scan_instrument_effect[i] = ((gchar **) instrument->data)[1];

	instrument = instrument->next;
      }
    
      quick_scan_instrument_filename[i] = NULL;
      quick_scan_instrument_effect[i] = NULL;
    }

    lv2_manager->quick_scan_plugin_filename = quick_scan_plugin_filename;
    lv2_manager->quick_scan_plugin_effect = quick_scan_plugin_effect;

    lv2_manager->quick_scan_instrument_filename = quick_scan_instrument_filename;
    lv2_manager->quick_scan_instrument_effect = quick_scan_instrument_effect;
  
    g_list_free_full(start_plugin,
		     g_free);

    g_list_free_full(start_instrument,
		     g_free);
  }
  
  /* load vst3 */
#if defined(AGS_WITH_VST3)
  current_time = g_get_monotonic_time();
  
  if(current_time < start_time + AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL &&
     !gsequencer_application_context->vst3_loading){
    ags_log_add_message(log,
			"* Loading VST3 plugins");

    gsequencer_application_context->vst3_loading = TRUE;
  }

  while(gsequencer_application_context->vst3_loader != NULL){
    GDir *arch_dir;

    gchar *arch_path;
    gchar *loader_filename;
    gchar *arch_filename;
    gchar *path;
    gchar *filename;

    current_time = g_get_monotonic_time();
    
    if(!initial_load &&
       current_time > start_time + AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL){
      break;
    }

    loader_filename = gsequencer_application_context->vst3_loader->data;

#if defined(AGS_OSXAPI)
    arch_path = g_strdup(loader_filename);
#else
    arch_path = g_strdup_printf("%s%cContents%c%s-%s",
				loader_filename,
				G_DIR_SEPARATOR,
				G_DIR_SEPARATOR,
#if defined(__x86_64__)
				"x86_64",
#elif defined(__i386__)
				"i386",
#elif defined(__aarch64__)
				"arm64",
#elif defined(__arm__)
				"arm",
#elif defined(__alpha__)
				"alpha",
#elif defined(__hppa__)
				"hppa",
#elif defined(__m68k__)
				"m68000",
#elif defined(__mips__)
				"mips",
#elif defined(__ppc__)
				"ppc",
#elif defined(__s390x__)
				"s390x",
#else
				"unknown"
#endif
				sysname);	
#endif

    if(g_file_test(arch_path,
		   G_FILE_TEST_IS_DIR)){
      error = NULL;
      arch_dir = g_dir_open(arch_path,
			    0,
			    &error);

      if(error != NULL){
	g_warning("%s", error->message);

	gsequencer_application_context->vst3_loader = g_list_remove(gsequencer_application_context->vst3_loader,
								    loader_filename);

	g_free(arch_path);

	g_error_free(error);
	
	continue;
      }

      while((arch_filename = g_dir_read_name(arch_dir)) != NULL){
#if defined(AGS_OSXAPI)
	if(!g_list_find_custom(vst3_manager->vst3_plugin_blacklist,
			       arch_filename,
			       g_strcmp0)){
	  ags_vst3_manager_load_file(vst3_manager,
				     arch_path,
				     arch_filename);
	}
#else
	if(g_str_has_suffix(arch_filename,
			    AGS_LIBRARY_SUFFIX) &&
	   !g_list_find_custom(vst3_manager->vst3_plugin_blacklist,
			       arch_filename,
			       strcmp)){
	  ags_vst3_manager_load_file(vst3_manager,
				     arch_path,
				     arch_filename);
	}
#endif
      }
    }

    gsequencer_application_context->vst3_loader = g_list_remove(gsequencer_application_context->vst3_loader,
								loader_filename);

    g_free(arch_path);
    
    initial_load = FALSE;
  }
#endif

  return(TRUE);
}

AgsGSequencerApplicationContext*
ags_gsequencer_application_context_new()
{
  AgsGSequencerApplicationContext *gsequencer_application_context;

  gsequencer_application_context = (AgsGSequencerApplicationContext *) g_object_new(AGS_TYPE_GSEQUENCER_APPLICATION_CONTEXT,
										    NULL);

  return(gsequencer_application_context);
}
