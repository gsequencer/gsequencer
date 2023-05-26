/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/ags_export_window.h>
#include <ags/app/ags_export_window_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>

#include <stdlib.h>
#include <ags/i18n.h>

void ags_export_window_class_init(AgsExportWindowClass *export_window);
void ags_export_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_export_window_init(AgsExportWindow *export_window);
void ags_export_window_finalize(GObject *gobject);

void ags_export_window_connect(AgsConnectable *connectable);
void ags_export_window_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_export_window
 * @short_description: The export dialog
 * @title: AgsExportWindow
 * @section_id:
 * @include: ags/app/ags_export_window.h
 *
 * #AgsExportWindow lets you export to audio files. This widget
 * can contain mulitple #AgsExportSoundcard widgets.
 */

static gpointer ags_export_window_parent_class = NULL;

GType
ags_export_window_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_export_window = 0;

    static const GTypeInfo ags_export_window_info = {
      sizeof (AgsExportWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_export_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsExportWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_export_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_export_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_export_window = g_type_register_static(GTK_TYPE_WINDOW,
						    "AgsExportWindow", &ags_export_window_info,
						    0);
    
    g_type_add_interface_static(ags_type_export_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_export_window);
  }

  return g_define_type_id__volatile;
}

void
ags_export_window_class_init(AgsExportWindowClass *export_window)
{
  GObjectClass *gobject;

  ags_export_window_parent_class = g_type_class_peek_parent(export_window);

  /* GObjectClass */
  gobject = (GObjectClass *) export_window;

  gobject->finalize = ags_export_window_finalize;
}

void
ags_export_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = NULL;
  connectable->connect = ags_export_window_connect;
  connectable->disconnect = ags_export_window_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_export_window_init(AgsExportWindow *export_window)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkGrid *grid;
  GtkLabel *label;

  AgsConfig *config;
  
  gchar *str;
  
  config = ags_config_get_instance();

  export_window->flags = 0;
  export_window->connectable_flags = 0;
  
  g_object_set(export_window,
	       "title", i18n("Export to audio data"),
	       NULL);

  gtk_window_set_hide_on_close((GtkWindow *) export_window,
			       TRUE);

  g_atomic_int_set(&(export_window->do_stop),
		   FALSE);
  
  /* pack */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  
  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 FALSE);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) export_window,
		       (GtkWidget *) vbox);

  /* live export */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "disable-feature");
  
  if(str == NULL ||
     !g_ascii_strncasecmp(str,
			  "experimental",
			  13)){
    export_window->live_export = NULL;
  }else{
    export_window->live_export = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("live export"));
    gtk_check_button_set_active((GtkCheckButton *) export_window->live_export,
				TRUE);
    gtk_box_append(vbox,
		   (GtkWidget *) export_window->live_export);
  }

  g_free(str);
  
  /* exclude sequencer */
  export_window->exclude_sequencer = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("exclude sequencers"));
  gtk_check_button_set_active(export_window->exclude_sequencer,
			      TRUE);
  gtk_box_append(vbox,
		 (GtkWidget *) export_window->exclude_sequencer);
  
  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_START);
  
  gtk_widget_set_vexpand((GtkWidget *) grid,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) grid,
			 FALSE);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_PADDING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_PADDING);
  
  gtk_box_append(vbox,
		 (GtkWidget *) grid);

  /* mode */
  label = (GtkLabel *) gtk_label_new(i18n("mode"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_hexpand((GtkWidget *) label,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  export_window->mode = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(export_window->mode,
				 "tact");
  //TODO:JK: uncomment me
  //  gtk_combo_box_text_append_text(export_window->mode,
  //				 "time");
  gtk_combo_box_set_active((GtkComboBox *) export_window->mode,
			   0);

  gtk_widget_set_halign((GtkWidget *) export_window->mode,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) export_window->mode,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) export_window->mode,
		  1, 1,
		  1, 1);

  /* tact */
  label = (GtkLabel *) gtk_label_new(i18n("tact"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_hexpand((GtkWidget *) label,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 2,
		  1, 1);

  export_window->tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_NOTATION_DEFAULT_LENGTH, 0.25);
  gtk_spin_button_set_digits(export_window->tact,
			     2);

  gtk_widget_set_halign((GtkWidget *) export_window->tact,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) export_window->tact,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) export_window->tact,
		  1, 2,
		  1, 1);

  /* time */
  label = (GtkLabel *) gtk_label_new(i18n("time"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_hexpand((GtkWidget *) label,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  /* duration */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_box_set_spacing(hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_hexpand((GtkWidget *) hbox,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) hbox,
			 TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) hbox,
		  1, 3,
		  1, 1);

  export_window->duration = (GtkLabel *) gtk_label_new(ags_navigation_tact_to_time_string(0.0,
											  AGS_SOUNDCARD_DEFAULT_BPM,
											  1.0));
  gtk_box_append(hbox,
		 (GtkWidget *) export_window->duration);

  /* export soundcard */
  export_window->export_soundcard = NULL;
  
  export_window->export_soundcard_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							       0);

  gtk_box_set_spacing(export_window->export_soundcard_box,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) export_window->export_soundcard_box);

  /* add */
  export_window->add = (GtkButton *) gtk_button_new_from_icon_name("list-add");
  
  gtk_widget_set_halign((GtkWidget *) export_window->add,
			GTK_ALIGN_END);
  gtk_widget_set_valign((GtkWidget *) export_window->add,
			GTK_ALIGN_START);
  gtk_widget_set_hexpand((GtkWidget *) export_window->add,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) export_window->add,
			 TRUE);
  
  gtk_box_append(vbox,
		 (GtkWidget *) export_window->add);

  /* remove files */
  export_window->remove_filename = NULL;
  
  /* export */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  export_window->export = (GtkToggleButton *) gtk_toggle_button_new_with_label(i18n("export"));
  gtk_box_append(hbox,
		 (GtkWidget *) export_window->export);

  gtk_window_set_default_size((GtkWindow *) export_window,
			      800, 600);
}

void
ags_export_window_connect(AgsConnectable *connectable)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  GList *start_list, *list;

  export_window = AGS_EXPORT_WINDOW(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (export_window->connectable_flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();  

  g_signal_connect_after(application_context, "update-ui",
			 G_CALLBACK(ags_export_window_update_ui_callback), export_window);
  
  g_signal_connect(G_OBJECT(export_window->add), "clicked",
		   G_CALLBACK(ags_export_window_add_export_soundcard_callback), export_window);

  g_signal_connect_after(G_OBJECT(export_window->tact), "value-changed",
			 G_CALLBACK(ags_export_window_tact_callback), export_window);

  g_signal_connect_after(G_OBJECT(export_window->export), "clicked",
			 G_CALLBACK(ags_export_window_export_callback), export_window);

  export_window->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  /* export soundcard */
  list =
    start_list = ags_export_window_get_export_soundcard(export_window);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    /* iterate */
    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_export_window_disconnect(AgsConnectable *connectable)
{
  AgsExportWindow *export_window;

  AgsApplicationContext *application_context;

  GList *start_list, *list;

  export_window = AGS_EXPORT_WINDOW(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (export_window->connectable_flags)) == 0){
    return;
  }

  application_context = ags_application_context_get_instance();  

  g_object_disconnect(G_OBJECT(application_context),
		      "any_signal::update-ui",
		      G_CALLBACK(ags_export_window_update_ui_callback),
		      export_window,
		      NULL);

  g_object_disconnect(G_OBJECT(export_window->add),
		      "any_signal::clicked",
		      G_CALLBACK(ags_export_window_add_export_soundcard_callback),
		      export_window,
		      NULL);

  g_object_disconnect(G_OBJECT(export_window->tact),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_export_window_tact_callback),
		      export_window,
		      NULL);

  g_object_disconnect(G_OBJECT(export_window->export),
		      "any_signal::clicked",
		      G_CALLBACK(ags_export_window_export_callback),
		      export_window,
		      NULL);
  
  export_window->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  /* export soundcard */
  list =
    start_list = ags_export_window_get_export_soundcard(export_window);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_export_window_finalize(GObject *gobject)
{
  AgsExportWindow *export_window;

  export_window = (AgsExportWindow *) gobject;
  
  G_OBJECT_CLASS(ags_export_window_parent_class)->finalize(gobject);
}

/**
 * ags_export_window_test_flags:
 * @export_window: the #AgsExportWindow
 * @flags: the flags
 *
 * Test @flags of @export_window.
 * 
 * Returns: %TRUE if @flags is set, otherwise %FALSE
 *
 * Since: 4.5.0
 */
gboolean
ags_export_window_test_flags(AgsExportWindow *export_window,
			     guint flags)
{
  guint retval;
  
  g_return_val_if_fail(AGS_IS_EXPORT_WINDOW(export_window), FALSE);

  retval = (((flags &(export_window->flags))) != 0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_export_window_set_flags:
 * @export_window: the #AgsExportWindow
 * @flags: the flags
 *
 * Set @flags of @export_window.
 * 
 * Since: 4.5.0
 */
void
ags_export_window_set_flags(AgsExportWindow *export_window,
			    guint flags)
{
  g_return_if_fail(AGS_IS_EXPORT_WINDOW(export_window));

  //TODO:JK: implement me
  
  export_window->flags |= flags;
}

/**
 * ags_export_window_unset_flags:
 * @export_window: the #AgsExportWindow
 * @flags: the flags
 *
 * Unset @flags of @export_window.
 * 
 * Since: 4.5.0
 */
void
ags_export_window_unset_flags(AgsExportWindow *export_window,
			      guint flags)
{
  g_return_if_fail(AGS_IS_EXPORT_WINDOW(export_window));

  //TODO:JK: implement me
  
  export_window->flags &= (~flags);
}

/**
 * ags_export_window_get_export_soundcard:
 * @export_window: the #AgsExportWindow
 * 
 * Get soundcard editor.
 * 
 * Returns: the #GList-struct containing #AgsExportSoundcard
 * 
 * Since: 4.0.0
 */
GList*
ags_export_window_get_export_soundcard(AgsExportWindow *export_window)
{
  g_return_val_if_fail(AGS_IS_EXPORT_WINDOW(export_window),
		       NULL);

  return(g_list_reverse(g_list_copy(export_window->export_soundcard)));
}

/**
 * ags_export_window_add_export_soundcard:
 * @export_window: the #AgsExportWindow
 * @export_soundcard: the #AgsExportSoundcard
 * 
 * Add soundcard editor.
 * 
 * Since: 3.18.0
 */
void
ags_export_window_add_export_soundcard(AgsExportWindow *export_window,
				       AgsExportSoundcard *export_soundcard)
{
  g_return_if_fail(AGS_IS_EXPORT_WINDOW(export_window));
  g_return_if_fail(AGS_IS_EXPORT_SOUNDCARD(export_soundcard));

  if(g_list_find(export_window->export_soundcard, export_soundcard) == NULL){
    GtkButton *remove_button;

    /* instantiate export soundcard */
    export_window->export_soundcard = g_list_prepend(export_window->export_soundcard,
						     export_soundcard);
  
    gtk_box_append(export_window->export_soundcard_box,
		   (GtkWidget *) export_soundcard);
  
    ags_connectable_connect(AGS_CONNECTABLE(export_soundcard));
    
    /* remove button */
    remove_button = export_soundcard->remove_button;
    
    g_signal_connect(G_OBJECT(remove_button), "clicked",
		     G_CALLBACK(ags_export_window_remove_export_soundcard_callback), export_window);

    /* show all */
    gtk_widget_show((GtkWidget *) export_soundcard);
  }
}

/**
 * ags_export_window_remove_export_soundcard:
 * @export_window: the #AgsExportWindow
 * @export_soundcard: the #AgsExportSoundcard
 * 
 * Remove soundcard editor.
 * 
 * Since: 3.18.0
 */
void
ags_export_window_remove_export_soundcard(AgsExportWindow *export_window,
					  AgsExportSoundcard *export_soundcard)
{
  g_return_if_fail(AGS_IS_EXPORT_WINDOW(export_window));
  g_return_if_fail(AGS_IS_EXPORT_SOUNDCARD(export_soundcard));

  if(g_list_find(export_window->export_soundcard, export_soundcard) != NULL){
    export_window->export_soundcard = g_list_remove(export_window->export_soundcard,
						    export_soundcard);

    gtk_box_remove(export_window->export_soundcard_box,
		   (GtkWidget *) export_soundcard);
    
    g_object_unref(export_soundcard);
  }
}

/**
 * ags_export_window_reload_soundcard_editor:
 * @export_window: the #AgsExportWindow
 * 
 * Reload soundcard editor.
 * 
 * Since: 3.0.0
 */
void
ags_export_window_reload_soundcard_editor(AgsExportWindow *export_window)
{
  AgsExportSoundcard *export_soundcard;
  GtkButton *remove_button;
  
  AgsApplicationContext *application_context;

  GList *start_list, *list;

  gchar *backend;
  gchar *str;
  
  guint i;

  g_return_if_fail(AGS_IS_EXPORT_WINDOW(export_window));
  
  /* retrieve main window and application context */
  application_context = ags_application_context_get_instance();
  
  list =
    start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  /* create export soundcard */
  for(i = 0; list != NULL; i++){
    guint soundcard_capability;

    soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(list->data));
    
    if(soundcard_capability == AGS_SOUNDCARD_CAPABILITY_PLAYBACK){
      /* instantiate export soundcard */
      export_soundcard = (AgsExportSoundcard *) g_object_new(AGS_TYPE_EXPORT_SOUNDCARD,
							     "soundcard", list->data,
							     NULL);
      ags_export_window_add_export_soundcard(export_window,
					     export_soundcard);

      ags_connectable_connect(AGS_CONNECTABLE(export_soundcard));
      
      /* set backend */
      backend = NULL;

      if(AGS_IS_WASAPI_DEVOUT(list->data)){
	backend = "wasapi";
      }else if(AGS_IS_ALSA_DEVOUT(list->data)){
	backend = "alsa";
      }else if(AGS_IS_OSS_DEVOUT(list->data)){
	backend = "oss";
      }else if(AGS_IS_JACK_DEVOUT(list->data)){
	backend = "jack";
      }else if(AGS_IS_PULSE_DEVOUT(list->data)){
	backend = "pulse";
      }else if(AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
	backend = "core-audio";
      }

      ags_export_soundcard_set_backend(export_soundcard,
				       backend);
      ags_export_soundcard_refresh_card(export_soundcard);
    
      /* set card */
      str = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));

      ags_export_soundcard_set_card(export_soundcard,
				    str);

      /* filename */
      str = g_strdup_printf("out-%d.wav",
			    i);
    
      ags_export_soundcard_set_filename(export_soundcard,
					str);
    
      g_free(str);

      /* set format */
      ags_export_soundcard_set_format(export_soundcard,
				      AGS_EXPORT_SOUNDCARD_FORMAT_WAV);

      /* show all */
      gtk_widget_show((GtkWidget *) export_soundcard);
    }
    
    /* iterate */
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_export_window_start_export:
 * @export_window: the #AgsExportWindow
 * 
 * Start export.
 * 
 * Since: 4.0.0
 */
void
ags_export_window_start_export(AgsExportWindow *export_window)
{
  AgsWindow *window;

  AgsExportOutput *export_output;

  AgsExportThread *export_thread, *current_export_thread;
  
  AgsThread *main_loop;
  
  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;

  GList *start_machine, *machine;
  GList *start_export_soundcard, *export_soundcard;
  GList *task;
    
  gboolean live_performance;
  gboolean success;
  
  g_return_if_fail(AGS_IS_EXPORT_WINDOW(export_window));

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
    
  /* collect */  
  export_thread = (AgsExportThread *) ags_thread_find_type(main_loop,
							   AGS_TYPE_EXPORT_THREAD);

  export_soundcard =
    start_export_soundcard = ags_export_window_get_export_soundcard(export_window);

  /* get some preferences */
  if(export_window->live_export != NULL){
    live_performance = gtk_check_button_get_active((GtkCheckButton *) export_window->live_export);
  }else{
    live_performance = TRUE;
  }

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  machine =
    start_machine = ags_ui_provider_get_machine(AGS_UI_PROVIDER(application_context));

  success = FALSE;
  
  /* start machine */
  while(machine != NULL){
    AgsMachine *current_machine;
    
    current_machine = AGS_MACHINE(machine->data);

    if((AGS_MACHINE_IS_SEQUENCER & (current_machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (current_machine->flags)) != 0){
      g_message("found machine to play!");

      ags_machine_set_run_extended(current_machine,
				   TRUE,
				   !gtk_check_button_get_active((GtkCheckButton *) export_window->exclude_sequencer), TRUE, FALSE, FALSE);
      success = TRUE;
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (current_machine->flags)) != 0){
      g_message("found machine to play!");

      ags_machine_set_run_extended(current_machine,
				   TRUE,
				   FALSE, TRUE, FALSE, FALSE);
      success = TRUE;
    }

    machine = machine->next;
  }

  /* start export thread */
  if(success){
    gchar *str;
      
    guint tic;
    guint format;
      
    gdouble delay;
      
    /* create task */
    delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(default_soundcard));

    /*  */
    tic = (gtk_spin_button_get_value(export_window->tact) + 1) * (16.0 * delay);
      
    export_soundcard = start_export_soundcard;
    task = NULL;
      
    while(export_soundcard != NULL){
      GtkEntryBuffer *entry_buffer;
      
      gchar *filename;
      
      current_export_thread = ags_export_thread_find_soundcard(export_thread,
							       AGS_EXPORT_SOUNDCARD(export_soundcard->data)->soundcard);

      entry_buffer = gtk_entry_get_buffer(AGS_EXPORT_SOUNDCARD(export_soundcard->data)->filename);
      
      filename = gtk_entry_buffer_get_text(entry_buffer);

      export_output = ags_export_output_new(current_export_thread,
					    AGS_EXPORT_SOUNDCARD(export_soundcard->data)->soundcard,
					    filename,
					    tic,
					    live_performance);

      str = gtk_combo_box_text_get_active_text(AGS_EXPORT_SOUNDCARD(export_soundcard->data)->output_format);
      format = 0;

      if(!g_ascii_strncasecmp(str,
			      "wav",
			      4)){
	format = AGS_EXPORT_OUTPUT_FORMAT_WAV;
      }else if(!g_ascii_strncasecmp(str,
				    "flac",
				    5)){
	format = AGS_EXPORT_OUTPUT_FORMAT_FLAC;
      }else if(!g_ascii_strncasecmp(str,
				    "ogg",
				    4)){
	format = AGS_EXPORT_OUTPUT_FORMAT_OGG;
      }

      g_object_set(G_OBJECT(export_output),
		   "format", format,
		   NULL);
	
      task = g_list_prepend(task,
			    export_output);
	
      if(AGS_EXPORT_SOUNDCARD(export_soundcard->data)->soundcard == default_soundcard){
	ags_export_window_set_flags(export_window,
				    AGS_EXPORT_WINDOW_HAS_STOP_TIMEOUT);

	g_signal_connect(current_export_thread, "stop",
			 G_CALLBACK(ags_export_window_stop_callback), export_window);
      }

      g_free(filename);
      
      export_soundcard = export_soundcard->next;
    }
      
    /* append AgsStartSoundcard */
    task = g_list_reverse(task);
      
    ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
				      task);
      
    ags_navigation_set_seeking_sensitive(window->navigation,
					 FALSE);
  }

  g_list_free(start_machine);
  g_list_free(start_export_soundcard);

  g_object_unref(main_loop);
}

/**
 * ags_export_window_stop_export:
 * @export_window: the #AgsExportWindow
 * 
 * Stop export.
 * 
 * Since: 4.0.0
 */
void
ags_export_window_stop_export(AgsExportWindow *export_window)
{  
  AgsWindow *window;

  AgsApplicationContext *application_context;

  GList *start_machine, *machine;

  gboolean success;

  g_return_if_fail(AGS_IS_EXPORT_WINDOW(export_window));

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  machine =
    start_machine = ags_ui_provider_get_machine(AGS_UI_PROVIDER(application_context));

  success = FALSE;
  
  /* stop machine */
  while(machine != NULL){
    AgsMachine *current_machine;
    
    current_machine = AGS_MACHINE(machine->data);

    if((AGS_MACHINE_IS_SEQUENCER & (current_machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (current_machine->flags)) != 0){
      g_message("found machine to stop!");
    
      ags_machine_set_run_extended(current_machine,
				   FALSE,
				   TRUE, TRUE, FALSE, FALSE);
	
      success = TRUE;
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (current_machine->flags)) != 0){
      g_message("found machine to stop!");
	
      ags_machine_set_run_extended(current_machine,
				   FALSE,
				   FALSE, TRUE, FALSE, FALSE);
      success = TRUE;
    }

    machine = machine->next;
  }

  /* disable auto-seeking */
  if(success){
    ags_navigation_set_seeking_sensitive(window->navigation,
					 TRUE);
  }

  g_list_free(start_machine);
}

/**
 * ags_export_window_new:
 * @transient_for: the transient for #GtkWindow
 * 
 * Create a new instance of #AgsExportWindow
 * 
 * Returns: the #AgsExportWindow
 * 
 * Since: 3.0.0
 */
AgsExportWindow*
ags_export_window_new(GtkWindow *transient_for)
{
  AgsExportWindow *export_window;

  export_window = (AgsExportWindow *) g_object_new(AGS_TYPE_EXPORT_WINDOW,
						   "transient-for", transient_for,
						   NULL);

  return(export_window);
}
