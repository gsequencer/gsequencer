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

#include <ags/X/ags_midi_dialog.h>
#include <ags/X/ags_midi_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/X/ags_window.h>

void ags_midi_dialog_class_init(AgsMidiDialogClass *midi_dialog);
void ags_midi_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_midi_dialog_init(AgsMidiDialog *midi_dialog);
void ags_midi_dialog_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_midi_dialog_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_midi_dialog_connect(AgsConnectable *connectable);
void ags_midi_dialog_disconnect(AgsConnectable *connectable);
void ags_midi_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_dialog_apply(AgsApplicable *applicable);
void ags_midi_dialog_reset(AgsApplicable *applicable);
void ags_midi_dialog_show_all(GtkWidget *widget);

/**
 * SECTION:ags_midi_dialog
 * @short_description: pack pad editors.
 * @title: AgsMidiDialog
 * @section_id:
 * @include: ags/X/ags_midi_dialog.h
 *
 * #AgsMidiDialog is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editors.
 */

enum{
  SET_MACHINE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MACHINE,
};

static guint midi_dialog_signals[LAST_SIGNAL];

GType
ags_midi_dialog_get_type(void)
{
  static GType ags_type_midi_dialog = 0;

  if(!ags_type_midi_dialog){
    static const GTypeInfo ags_midi_dialog_info = {
      sizeof (AgsMidiDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_midi_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi_dialog = g_type_register_static(GTK_TYPE_DIALOG,
						  "AgsMidiDialog\0", &ags_midi_dialog_info,
						  0);

    g_type_add_interface_static(ags_type_midi_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_midi_dialog);
}

void
ags_midi_dialog_class_init(AgsMidiDialogClass *midi_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) midi_dialog;

  gobject->set_property = ags_midi_dialog_set_property;
  gobject->get_property = ags_midi_dialog_get_property;

  /* properties */
  /**
   * AgsMachine:machine:
   *
   * The #AgsMachine to edit.
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_object("machine\0",
				   "assigned machine\0",
				   "The machine which this machine editor is assigned with\0",
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) midi_dialog;

  widget->show_all = ags_midi_dialog_show_all;
}

void
ags_midi_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_midi_dialog_connect;
  connectable->disconnect = ags_midi_dialog_disconnect;
}

void
ags_midi_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_midi_dialog_set_update;
  applicable->apply = ags_midi_dialog_apply;
  applicable->reset = ags_midi_dialog_reset;
}

void
ags_midi_dialog_init(AgsMidiDialog *midi_dialog)
{
  GtkLabel *label;
  GtkTable *table;
  GtkHBox *hbox;
  
  gtk_window_set_title((GtkWindow *) midi_dialog,
		       g_strdup("MIDI connection\0"));

  midi_dialog->flags = 0;

  midi_dialog->version = AGS_MIDI_DIALOG_DEFAULT_VERSION;
  midi_dialog->build_id = AGS_MIDI_DIALOG_DEFAULT_BUILD_ID;

  midi_dialog->machine = NULL;

  /* connection */
  midi_dialog->io_options = (GtkVBox *) gtk_vbox_new(FALSE,
						     0);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(midi_dialog)->vbox,
		     GTK_WIDGET(midi_dialog->io_options),
		     FALSE, FALSE,
		     0);

  midi_dialog->playback = (GtkCheckButton *) gtk_check_button_new_with_label("playback\0");
  gtk_box_pack_start((GtkBox *) midi_dialog->io_options,
		     GTK_WIDGET(midi_dialog->playback),
		     FALSE, FALSE,
		     0);

  midi_dialog->record = (GtkCheckButton *) gtk_check_button_new_with_label("record\0");
  gtk_box_pack_start((GtkBox *) midi_dialog->io_options,
		     GTK_WIDGET(midi_dialog->record),
		     FALSE, FALSE,
		     0);

  /* mapping */
  midi_dialog->mapping = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(midi_dialog)->vbox,
		     GTK_WIDGET(midi_dialog->mapping),
		     FALSE, FALSE,
		     0);

  table = (GtkTable *) gtk_table_new(4, 2,
				     FALSE);
  gtk_box_pack_start((GtkBox *) midi_dialog->mapping,
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);
  
  /* audio start */
  label = (GtkLabel *) gtk_label_new("audio start mapping\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  midi_dialog->audio_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									      65535.0,
									      1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->audio_start),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* audio end */
  label = (GtkLabel *) gtk_label_new("audio end mapping\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  midi_dialog->audio_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									    65535.0,
									    1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->audio_end),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* midi start */
  label = (GtkLabel *) gtk_label_new("midi start mapping\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  midi_dialog->midi_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									     128.0,
									     1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->midi_start),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* midi end */
  label = (GtkLabel *) gtk_label_new("midi end mapping\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  midi_dialog->midi_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									   128.0,
									   1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->midi_end),
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* device */
  midi_dialog->device = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(midi_dialog)->vbox,
		     GTK_WIDGET(midi_dialog->device),
		     FALSE, FALSE,
		     0);

  table = (GtkTable *) gtk_table_new(8, 2,
				     FALSE);
  gtk_box_pack_start((GtkBox *) midi_dialog->device,
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* backend */
  label = (GtkLabel *) gtk_label_new("midi backend\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  midi_dialog->backend =  (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(midi_dialog->backend,
				 "alsa\0");
  gtk_combo_box_text_append_text(midi_dialog->backend,
				 "jack\0");
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->backend),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_combo_box_set_active(GTK_COMBO_BOX(midi_dialog->backend),
			   0);
  
  /* midi device */
  label = (GtkLabel *) gtk_label_new("midi device\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  midi_dialog->midi_device = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->midi_device),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* sequencer */  
  label = (GtkLabel *) gtk_label_new("sequencer\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   6, 7,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_table_attach(table,
		   GTK_WIDGET(hbox),
		   0, 2,
		   7, 8,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  midi_dialog->add_sequencer = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(midi_dialog->add_sequencer),
		     FALSE, FALSE,
		     0);

  midi_dialog->remove_sequencer = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(midi_dialog->remove_sequencer),
		     FALSE, FALSE,
		     0);

  /* insensitive for alsa */
  gtk_widget_set_sensitive((GtkWidget *) midi_dialog->add_sequencer,
			   FALSE);
  gtk_widget_set_sensitive((GtkWidget *) midi_dialog->remove_sequencer,
			   FALSE);

  /* GtkButton's in GtkDialog->action_area  */
  midi_dialog->apply = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(midi_dialog)->action_area,
		     (GtkWidget *) midi_dialog->apply,
		     FALSE, FALSE,
		     0);
  
  midi_dialog->ok = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(midi_dialog)->action_area,
		     (GtkWidget *) midi_dialog->ok,
		     FALSE, FALSE,
		     0);
  
  midi_dialog->cancel = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(midi_dialog)->action_area,
		     (GtkWidget *) midi_dialog->cancel,
		     FALSE, FALSE,
		     0);
}

void
ags_midi_dialog_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;

      machine = (AgsMachine *) g_value_get_object(value);

      if(machine == midi_dialog->machine){
	return;
      }

      if(midi_dialog->machine != NULL){
	g_object_unref(midi_dialog->machine);
      }

      if(machine != NULL){
	g_object_ref(machine);
      }

      midi_dialog->machine = machine;

      /* set cards */
      ags_midi_dialog_load_sequencers(midi_dialog);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_dialog_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    g_value_set_object(value, midi_dialog->machine);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_dialog_connect(AgsConnectable *connectable)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(connectable);

  if((AGS_MIDI_DIALOG_CONNECTED & (midi_dialog->flags)) != 0){
    return;
  }

  midi_dialog->flags |= AGS_MIDI_DIALOG_CONNECTED;
  
  /* backend */
  g_signal_connect((GObject *) midi_dialog->backend, "changed\0",
		   G_CALLBACK(ags_midi_dialog_backend_changed_callback), (gpointer) midi_dialog);

  /* sequencer */
  g_signal_connect((GObject *) midi_dialog->add_sequencer, "clicked\0",
		   G_CALLBACK(ags_midi_dialog_add_sequencer_callback), (gpointer) midi_dialog);

  g_signal_connect((GObject *) midi_dialog->remove_sequencer, "clicked\0",
		   G_CALLBACK(ags_midi_dialog_remove_sequencer_callback), (gpointer) midi_dialog);

  /* applicable */
  g_signal_connect((GObject *) midi_dialog->apply, "clicked\0",
		   G_CALLBACK(ags_midi_dialog_apply_callback), (gpointer) midi_dialog);

  g_signal_connect((GObject *) midi_dialog->ok, "clicked\0",
		   G_CALLBACK(ags_midi_dialog_ok_callback), (gpointer) midi_dialog);

  g_signal_connect((GObject *) midi_dialog->cancel, "clicked\0",
		   G_CALLBACK(ags_midi_dialog_cancel_callback), (gpointer) midi_dialog);
}

void
ags_midi_dialog_disconnect(AgsConnectable *connectable)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(connectable);

  if((AGS_MIDI_DIALOG_CONNECTED & (midi_dialog->flags)) == 0){
    return;
  }

  midi_dialog->flags &= (~AGS_MIDI_DIALOG_CONNECTED);

  /* backend */
  g_object_disconnect((GObject *) midi_dialog->backend,
		      "changed\0",
		      G_CALLBACK(ags_midi_dialog_backend_changed_callback),
		      (gpointer) midi_dialog,
		      NULL);

  /* sequencer */
  g_object_disconnect((GObject *) midi_dialog->add_sequencer,
		      "clicked\0",
		      G_CALLBACK(ags_midi_dialog_add_sequencer_callback),
		      (gpointer) midi_dialog,
		      NULL);

  g_object_disconnect((GObject *) midi_dialog->remove_sequencer,
		      "clicked\0",
		      G_CALLBACK(ags_midi_dialog_remove_sequencer_callback),
		      (gpointer) midi_dialog,
		      NULL);

  /* applicable */
  g_object_disconnect((GObject *) midi_dialog->apply,
		      "clicked\0",
		      G_CALLBACK(ags_midi_dialog_apply_callback),
		      (gpointer) midi_dialog,
		      NULL);

  g_object_disconnect((GObject *) midi_dialog->ok,
		      "clicked\0",
		      G_CALLBACK(ags_midi_dialog_ok_callback),
		      (gpointer) midi_dialog,
		      NULL);

  g_object_disconnect((GObject *) midi_dialog->cancel,
		      "clicked\0",
		      G_CALLBACK(ags_midi_dialog_cancel_callback),
		      (gpointer) midi_dialog,
		      NULL);
}

void
ags_midi_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_midi_dialog_apply(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMidiDialog *midi_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  GObject *sequencer;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GType sequencer_type;
  GList *list;

  gchar *midi_device;
  gchar *backend;

  pthread_mutex_t *application_mutex;
  
  midi_dialog = AGS_MIDI_DIALOG(applicable);

  machine = midi_dialog->machine;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  /* application context and mutex manager */
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* audio and sequencer */
  audio = machine->audio;

  sequencer = NULL;
  backend = gtk_combo_box_text_get_active_text(midi_dialog->backend);
  midi_device = gtk_combo_box_text_get_active_text(midi_dialog->midi_device);
  
  /* find device */
  if(backend !=  NULL &&
     midi_device != NULL){
    pthread_mutex_lock(application_mutex);

    sequencer_type = G_TYPE_NONE;
    
    if(!g_ascii_strncasecmp("alsa\0",
			    backend,
			    4)){
      sequencer_type = AGS_TYPE_MIDIIN;    
    }else if(!g_ascii_strncasecmp("jack\0",
				  backend,
				  4)){
      sequencer_type = AGS_TYPE_JACK_MIDIIN;
    }
    
    list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

    while(list != NULL){
      if(g_type_is_a(G_OBJECT_TYPE(list->data),
		     sequencer_type) &&
	 !g_ascii_strcasecmp(ags_sequencer_get_device(AGS_SEQUENCER(list->data)),
			     midi_device)){
	sequencer = list->data;

	break;
      }    
    
      list = list->next;
    }
  
    pthread_mutex_unlock(application_mutex);
  }
  
  /* set properties */
  g_object_set(audio,
	       "audio-start-mapping\0", gtk_spin_button_get_value_as_int(midi_dialog->audio_start),
	       "audio-end-mapping\0", gtk_spin_button_get_value_as_int(midi_dialog->audio_end),
	       "midi-start-mapping\0", gtk_spin_button_get_value_as_int(midi_dialog->midi_start),
	       "midi-end-mapping\0", gtk_spin_button_get_value_as_int(midi_dialog->midi_end),
	       "sequencer\0", sequencer,
	       NULL);
}

void
ags_midi_dialog_reset(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMidiDialog *midi_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  GObject *sequencer;

  AgsApplicationContext *application_context;

  GList *list;
  GtkTreeModel *model;
  GtkTreeIter iter;

  gchar *midi_device;
  gchar *str;
  guint audio_start, audio_end;
  guint midi_start, midi_end;
  guint i;
  gboolean found_device;
    
  midi_dialog = AGS_MIDI_DIALOG(applicable);

  machine = midi_dialog->machine;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  /* application context and mutex manager */
  application_context = (AgsApplicationContext *) window->application_context;

  /* audio and sequencer */
  audio = machine->audio;

  sequencer = audio->sequencer;

  if(sequencer == NULL){
    GList *list;

    list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

    if(list != NULL){
      sequencer = list->data;
    }
  }

  if(AGS_IS_MIDIIN(sequencer)){
    gtk_combo_box_set_active(GTK_COMBO_BOX(midi_dialog->backend),
			     0);

    midi_device = ags_sequencer_get_device(AGS_SEQUENCER(sequencer));
  }else if(AGS_IS_JACK_MIDIIN(sequencer)){
    gtk_combo_box_set_active(GTK_COMBO_BOX(midi_dialog->backend),
			     1);
    
    midi_device = ags_sequencer_get_device(AGS_SEQUENCER(sequencer));
  }else{
    midi_device = NULL;
  }

  /*  */
  g_object_get(audio,
	       "audio-start-mapping\0", &audio_start,
	       "audio-end-mapping\0", &audio_end,
	       "midi-start-mapping\0", &midi_start,
	       "midi-end-mapping\0", &midi_end,
	       "sequencer\0", &sequencer,
	       NULL);
  
  /* mapping */
  gtk_spin_button_set_value(midi_dialog->audio_start,
			    (gdouble) audio_start);
  gtk_spin_button_set_value(midi_dialog->audio_end,
			    (gdouble) audio_end);
  
  gtk_spin_button_set_value(midi_dialog->midi_start,
			    (gdouble) midi_start);
  gtk_spin_button_set_value(midi_dialog->midi_end,
			    (gdouble) midi_end);


  /* load midi devices */
  ags_midi_dialog_load_sequencers(midi_dialog);

  /* find device */
  found_device = FALSE;
  
  if(midi_device != NULL){
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(midi_dialog->midi_device));
    i = 0;

    if(model != NULL &&
       gtk_tree_model_get_iter_first(model,
				     &iter)){
      do{
	gtk_tree_model_get(model,
			   &iter,
			   0, &str,
			   -1);
      
	if(!g_ascii_strcasecmp(midi_device,
			       str)){
	  found_device = TRUE;
	  break;
	}
      
	i++;
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }
  }

  if(found_device){
    gtk_combo_box_set_active(GTK_COMBO_BOX(midi_dialog->midi_device),
			     i);
  }else{
    gtk_combo_box_set_active(GTK_COMBO_BOX(midi_dialog->midi_device),
			     0);
  }
}

void
ags_midi_dialog_load_sequencers(AgsMidiDialog *midi_dialog)
{
  AgsWindow *window;
  
  GObject *sequencer;
  GtkListStore *model;
  AgsAudio *audio;

  AgsMutexManager *mutex_manager;

  GType sequencer_type;
  
  GtkTreeIter iter;
  GList *card_id, *card_name;
  GList *list;
  gchar *backend;

  pthread_mutex_t *application_mutex;

  gdk_threads_enter();
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) midi_dialog->machine,
						 AGS_TYPE_WINDOW);

  /* application context and mutex manager */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* clear model */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(midi_dialog->midi_device))));

  /* backend */
  sequencer = NULL;

  backend = gtk_combo_box_text_get_active_text(midi_dialog->backend);

  /* find sequencer */
  pthread_mutex_lock(application_mutex);

  list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(window->application_context));

  if(!g_ascii_strncasecmp("alsa\0",
			  backend,
			  4)){
    sequencer_type = AGS_TYPE_MIDIIN;
  }else if(!g_ascii_strncasecmp("jack\0",
				backend,
				4)){
    sequencer_type = AGS_TYPE_JACK_MIDIIN;
  }else{
    sequencer_type = G_TYPE_NONE;
  }

  while(list != NULL){
    if(g_type_is_a(G_OBJECT_TYPE(list->data),
		   sequencer_type)){
      sequencer = list->data;
      
      break;
    }else if(g_type_is_a(G_OBJECT_TYPE(list->data),
			 sequencer_type)){
      sequencer = list->data;
	
      break;
    }
    
    list = list->next;
  }
    
  pthread_mutex_unlock(application_mutex);

  /* tree model */
  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
  
  /* null device */
  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL\0",
		     1, "(null)\0",
		     -1);

  /* load sequencer */
  if(sequencer != NULL){
    ags_sequencer_list_cards(AGS_SEQUENCER(sequencer),
			     &card_id, &card_name);

    while(card_id != NULL){
      gtk_list_store_append(model, &iter);
      gtk_list_store_set(model, &iter,
			 0, card_id->data,
			 1, card_name->data,
			 -1);
    
      card_id = card_id->next;
      card_name = card_name->next;
    }

    gtk_combo_box_set_model(GTK_COMBO_BOX(midi_dialog->midi_device),
			    GTK_TREE_MODEL(model));
  
    //    g_list_free(card_id);
    //    g_list_free(card_name);
  }

  gdk_threads_leave();
}

void
ags_midi_dialog_show_all(GtkWidget *widget)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = (AgsMidiDialog *) widget;
  
  if((AGS_MIDI_DIALOG_IO_OPTIONS & (midi_dialog->flags)) != 0){
    gtk_widget_show_all((GtkWidget *) midi_dialog->io_options);
  }

  if((AGS_MIDI_DIALOG_MAPPING & (midi_dialog->flags)) != 0){
    gtk_widget_show_all((GtkWidget *) midi_dialog->mapping);
  }

  if((AGS_MIDI_DIALOG_DEVICE & (midi_dialog->flags)) != 0){
    gtk_widget_show_all((GtkWidget *) midi_dialog->device);
  }

  gtk_widget_show(widget);
  gtk_widget_show((GtkWidget *) midi_dialog->apply);
  gtk_widget_show((GtkWidget *) midi_dialog->ok);
  gtk_widget_show((GtkWidget *) midi_dialog->cancel);
}

/**
 * ags_midi_dialog_new:
 * @machine: the assigned machine.
 *
 * Creates an #AgsMidiDialog
 *
 * Returns: a new #AgsMidiDialog
 *
 * Since: 0.7.0
 */
AgsMidiDialog*
ags_midi_dialog_new(AgsMachine *machine)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = (AgsMidiDialog *) g_object_new(AGS_TYPE_MIDI_DIALOG,
					       "machine\0", machine,
					       NULL);

  return(midi_dialog);
}
