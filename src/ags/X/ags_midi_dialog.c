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

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>
#include <ags/object/ags_sequencer.h>

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
  GtkTable *table;
  GtkLabel *label;
  GtkHBox *hbox;
  
  gtk_window_set_title((GtkDialog *) midi_dialog, g_strdup("MIDI connection\0"));

  midi_dialog->flags = 0;

  midi_dialog->version = AGS_MIDI_DIALOG_DEFAULT_VERSION;
  midi_dialog->build_id = AGS_MIDI_DIALOG_DEFAULT_BUILD_ID;

  midi_dialog->machine = NULL;

  /* connection */
  table = (GtkTable *) gtk_table_new(6, 2,
				     FALSE);
  gtk_box_pack_start(GTK_DIALOG(midi_dialog)->vbox,
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  midi_dialog->playback = gtk_check_button_new_with_label("playback\0");
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->playback),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  midi_dialog->record = gtk_check_button_new_with_label("record\0");
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->record),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  /* audio start */
  label = (GtkLabel *) gtk_label_new("audio mapping\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  midi_dialog->audio_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									      65535.0,
									      1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->audio_start),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* midi start */
  label = (GtkLabel *) gtk_label_new("midi mapping\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
    gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  midi_dialog->midi_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									      128.0,
									      1.0);
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->midi_start),
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* backend */
  label = (GtkLabel *) gtk_label_new("midi backend\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   5, 6,
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
		   5, 6,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_combo_box_set_active(midi_dialog->backend,
			   0);
  
  /* midi device */
  label = (GtkLabel *) gtk_label_new("midi device\0");
  g_object_set(label,
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   6, 7,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  midi_dialog->midi_device = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(midi_dialog->midi_device),
		   1, 2,
		   6, 7,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* connection */  
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_table_attach(table,
		   GTK_WIDGET(hbox),
		   0, 2,
		   8, 9,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  midi_dialog->connection_name = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(midi_dialog->connection_name),
		     TRUE, TRUE,
		     0);

  midi_dialog->add = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(midi_dialog->add),
		     FALSE, FALSE,
		     0);

  midi_dialog->remove = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(midi_dialog->remove),
		     FALSE, FALSE,
		     0);

  /* insensitive for alsa */
  gtk_widget_set_sensitive(midi_dialog->connection_name,
			   FALSE);
  gtk_widget_set_sensitive(midi_dialog->add,
			   FALSE);
  gtk_widget_set_sensitive(midi_dialog->remove,
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

  g_signal_connect((GObject *) midi_dialog->backend, "changed\0",
		   G_CALLBACK(ags_midi_dialog_backend_changed_callback), (gpointer) midi_dialog);

  /* connection */
  g_signal_connect((GObject *) midi_dialog->add, "clicked\0",
		   G_CALLBACK(ags_midi_dialog_add_callback), (gpointer) midi_dialog);

  g_signal_connect((GObject *) midi_dialog->remove, "clicked\0",
		   G_CALLBACK(ags_midi_dialog_remove_callback), (gpointer) midi_dialog);

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

  //TODO:JK: implement me
}

void
ags_midi_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(applicable);

  //TODO:JK: implement me
}

void
ags_midi_dialog_apply(AgsApplicable *applicable)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(applicable);

  //TODO:JK: implement me
}

void
ags_midi_dialog_reset(AgsApplicable *applicable)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(applicable);
  
  //TODO:JK: implement me
}

void
ags_midi_dialog_load_sequencers(AgsMidiDialog *midi_dialog)
{
  AgsWindow *window;
  
  GObject *sequencer;
  GtkListStore *model;
  AgsAudio *audio;

  GType find_type;
  
  GtkTreeIter iter;
  GList *card_id, *card_name;
  GList *list;
  gchar *str;

  window = gtk_widget_get_ancestor(midi_dialog->machine,
				   AGS_TYPE_WINDOW);
  
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(midi_dialog->midi_device)));

  /* find sequencer */
  sequencer = NULL;
  
  list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(window->application_context));
  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX(midi_dialog->backend));

  if(!g_ascii_strncasecmp("alsa\0",
			  str,
			  4)){
    find_type = AGS_TYPE_MIDIIN;
  }else if(!g_ascii_strncasecmp("jack\0",
				str,
				4)){
    find_type = AGS_TYPE_JACK_MIDIIN;
  }else{
    find_type = G_TYPE_NONE;
  }

  while(list != NULL){
    if(g_type_is_a(G_OBJECT_TYPE(list->data),
		   find_type)){
      sequencer = list->data;
      
      break;
    }else if(g_type_is_a(G_OBJECT_TYPE(list->data),
			 find_type)){
      sequencer = list->data;
	
      break;
    }
    
    list = list->next;
  }
  
  /* load sequencer */
  if(sequencer != NULL){
    ags_sequencer_list_cards(AGS_SEQUENCER(sequencer),
			     &card_id, &card_name);

    model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
  
    while(card_id != NULL){
      gtk_list_store_append(model, &iter);
      gtk_list_store_set(model, &iter,
			 0, card_id->data,
			 1, card_name->data,
			 -1);
    
      card_id = card_id->next;
      card_name = card_name->next;
    }

    gtk_combo_box_set_model(midi_dialog->midi_device,
			    GTK_TREE_MODEL(model));
  
    g_list_free(card_id);
    g_list_free(card_name);
  }
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
