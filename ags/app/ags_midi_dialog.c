/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/ags_midi_dialog.h>
#include <ags/app/ags_midi_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/i18n.h>

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
void ags_midi_dialog_dispose(GObject *gobject);

gboolean ags_midi_dialog_is_connected(AgsConnectable *connectable);
void ags_midi_dialog_connect(AgsConnectable *connectable);
void ags_midi_dialog_disconnect(AgsConnectable *connectable);

void ags_midi_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_dialog_apply(AgsApplicable *applicable);
void ags_midi_dialog_reset(AgsApplicable *applicable);

void ags_midi_dialog_show(GtkWidget *widget);

void ags_midi_dialog_activate_button_callback(GtkButton *activate_button,
					      AgsMidiDialog *midi_dialog);

gboolean ags_midi_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
					      guint keyval,
					      guint keycode,
					      GdkModifierType state,
					      AgsMidiDialog *midi_dialog);
void ags_midi_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					   guint keyval,
					   guint keycode,
					   GdkModifierType state,
					   AgsMidiDialog *midi_dialog);
gboolean ags_midi_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
					    GdkModifierType keyval,
					    AgsMidiDialog *midi_dialog);

void ags_midi_dialog_real_response(AgsMidiDialog *midi_dialog,
				   gint response_id);

/**
 * SECTION:ags_midi_dialog
 * @short_description: Edit MIDI settings
 * @title: AgsMidiDialog
 * @section_id:
 * @include: ags/app/ags_midi_dialog.h
 *
 * #AgsMidiDialog is a composite widget to edit MIDI settings.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MACHINE,
};

static gpointer ags_midi_dialog_parent_class = NULL;

static guint midi_dialog_signals[LAST_SIGNAL];

GType
ags_midi_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_dialog = 0;

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

    ags_type_midi_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						  "AgsMidiDialog", &ags_midi_dialog_info,
						  0);

    g_type_add_interface_static(ags_type_midi_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_midi_dialog_class_init(AgsMidiDialogClass *midi_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_midi_dialog_parent_class = g_type_class_peek_parent(midi_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_dialog;

  gobject->set_property = ags_midi_dialog_set_property;
  gobject->get_property = ags_midi_dialog_get_property;

  gobject->dispose = ags_midi_dialog_dispose;
  
  /* properties */
  /**
   * AgsMachine:machine:
   *
   * The #AgsMachine to edit.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("machine",
				   i18n_pspec("assigned machine"),
				   i18n_pspec("The machine which this machine editor is assigned with"),
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) midi_dialog;

  widget->show = ags_midi_dialog_show;

  /* AgsMidiDialog */
  midi_dialog->response = ags_midi_dialog_real_response;
  
  /* signals */
  /**
   * AgsMidiDialog::response:
   * @midi_dialog: the #AgsMidiDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.10.0
   */
  midi_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(midi_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_midi_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_midi_dialog_is_connected;  
  connectable->connect = ags_midi_dialog_connect;
  connectable->disconnect = ags_midi_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
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
  GtkBox *vbox;
  GtkLabel *label;
  GtkGrid *grid;
  GtkBox *hbox;

  GtkEventController *event_controller;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();
  
  midi_dialog->flags = 0;
  midi_dialog->connectable_flags = 0;

  midi_dialog->version = AGS_MIDI_DIALOG_DEFAULT_VERSION;
  midi_dialog->build_id = AGS_MIDI_DIALOG_DEFAULT_BUILD_ID;

  midi_dialog->machine = NULL;

  gtk_window_set_title((GtkWindow *) midi_dialog,
		       i18n("MIDI connection"));

  gtk_window_set_deletable(GTK_WINDOW(midi_dialog),
			   TRUE);

  gtk_window_set_transient_for((GtkWindow *) midi_dialog,
			       (GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) midi_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_midi_dialog_key_pressed_callback), midi_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_midi_dialog_key_released_callback), midi_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_midi_dialog_modifiers_callback), midi_dialog);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) midi_dialog,
		       (GtkWidget *) vbox);

  gtk_widget_set_valign(vbox,
			GTK_ALIGN_START);

  gtk_widget_set_vexpand(vbox,
			 FALSE);
  
  /* connection */
  midi_dialog->io_options = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						   AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) midi_dialog->io_options);

  /* midi channel */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(midi_dialog->io_options,
		 (GtkWidget *) hbox);
  
  label = (GtkLabel *) gtk_label_new(i18n("midi channel"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);
  
  midi_dialog->midi_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 15.0, 1.0);
  gtk_box_append(hbox,
		 (GtkWidget *) midi_dialog->midi_channel);
  
  /* playback */
  midi_dialog->playback = NULL;
  //  midi_dialog->playback = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("playback"));
  //  gtk_box_append((GtkBox *) midi_dialog->io_options,
  //		     GTK_WIDGET(midi_dialog->playback));

  /* record */
  midi_dialog->record = NULL;
  //  midi_dialog->record = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("record"));
  //  gtk_box_append((GtkBox *) midi_dialog->io_options,
  //		     GTK_WIDGET(midi_dialog->record));

  /* mapping */
  midi_dialog->mapping = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 GTK_WIDGET(midi_dialog->mapping));

  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(midi_dialog->mapping,
		 (GtkWidget *) grid);
  
  /* audio start */
  label = (GtkLabel *) gtk_label_new(i18n("audio start mapping"));

  g_object_set(label,
	       "xalign", 0.0,
	       NULL);

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		   0, 0,
		   1, 1);
  
  midi_dialog->audio_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									      65535.0,
									      1.0);
  
  gtk_widget_set_halign((GtkWidget *) midi_dialog->audio_start,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) midi_dialog->audio_start,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) midi_dialog->audio_start,
		  1, 0,
		  1, 1);

  /* audio end */
  label = (GtkLabel *) gtk_label_new(i18n("audio end mapping"));

  g_object_set(label,
	       "xalign", 0.0,
	       NULL);

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);
  
  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		   0, 1,
		   1, 1);
  
  midi_dialog->audio_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									    65535.0,
									    1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) midi_dialog->audio_end,
		  1, 1,
		  1, 1);
  
  /* midi start */
  label = (GtkLabel *) gtk_label_new(i18n("midi start mapping"));

  g_object_set(label,
	       "xalign", 0.0,
	       NULL);

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 2,
		  1, 1);
  
  midi_dialog->midi_start = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									     128.0,
									     1.0);
  
  gtk_widget_set_halign((GtkWidget *) midi_dialog->midi_start,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) midi_dialog->midi_start,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) midi_dialog->midi_start,
		  1, 2,
		  1, 1);

  /* midi end */
  label = (GtkLabel *) gtk_label_new(i18n("midi end mapping"));

  g_object_set(label,
	       "xalign", 0.0,
	       NULL);

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  midi_dialog->midi_end = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									   128.0,
									   1.0);

  gtk_widget_set_halign((GtkWidget *) midi_dialog->midi_end,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) midi_dialog->midi_end,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(midi_dialog->midi_end),
		  1, 3,
		  1, 1);

  /* device */
  midi_dialog->device = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					       AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) midi_dialog->device);

  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_append((GtkBox *) midi_dialog->device,
		 (GtkWidget * ) grid);
  
  /* midi device */
  label = (GtkLabel *) gtk_label_new(i18n("midi device"));

  g_object_set(label,
	       "xalign", 0.0,
	       NULL);

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) label,
			 TRUE);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  midi_dialog->midi_device = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_widget_set_halign((GtkWidget *) midi_dialog->midi_device,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) midi_dialog->midi_device,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) midi_dialog->midi_device,
		  1, 1,
		  1, 1);

  /* buttons */
  midi_dialog->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
						    AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign(midi_dialog->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) midi_dialog->action_area);

  midi_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("ok"));

  gtk_box_append(midi_dialog->action_area,
		 (GtkWidget *) midi_dialog->activate_button);

  g_signal_connect(midi_dialog->activate_button, "clicked",
		   G_CALLBACK(ags_midi_dialog_activate_button_callback), midi_dialog);
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
    {
      g_value_set_object(value, midi_dialog->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_dialog_dispose(GObject *gobject)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(gobject);

  if(midi_dialog->machine != NULL){
    midi_dialog->machine->midi_dialog = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_midi_dialog_parent_class)->dispose(gobject);
}

gboolean
ags_midi_dialog_is_connected(AgsConnectable *connectable)
{
  AgsMidiDialog *midi_dialog;
  
  gboolean is_connected;
  
  midi_dialog = AGS_MIDI_DIALOG(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (midi_dialog->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_midi_dialog_connect(AgsConnectable *connectable)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  midi_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_midi_dialog_disconnect(AgsConnectable *connectable)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = AGS_MIDI_DIALOG(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  midi_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_midi_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_midi_dialog_apply(AgsApplicable *applicable)
{
  AgsMidiDialog *midi_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  GObject *sequencer;

  GtkTreeIter tree_iter;
  
  midi_dialog = AGS_MIDI_DIALOG(applicable);

  machine = midi_dialog->machine;

  /* audio and sequencer */
  audio = machine->audio;

  gtk_combo_box_get_active_iter(GTK_COMBO_BOX(midi_dialog->midi_device),
				&tree_iter);
  gtk_tree_model_get(gtk_combo_box_get_model(GTK_COMBO_BOX(midi_dialog->midi_device)),
		     &tree_iter,
		     1, &sequencer,
		     -1);
  
  /* set properties */
  g_object_set(audio,
	       "midi-channel", gtk_spin_button_get_value_as_int(midi_dialog->midi_channel),
	       "audio-start-mapping", gtk_spin_button_get_value_as_int(midi_dialog->audio_start),
	       "audio-end-mapping", gtk_spin_button_get_value_as_int(midi_dialog->audio_end),
	       "midi-start-mapping", gtk_spin_button_get_value_as_int(midi_dialog->midi_start),
	       "midi-end-mapping", gtk_spin_button_get_value_as_int(midi_dialog->midi_end),
	       "input-sequencer", sequencer,
	       NULL);
}

void
ags_midi_dialog_reset(AgsApplicable *applicable)
{
  AgsMidiDialog *midi_dialog;
  AgsMachine *machine;

  AgsAudio *audio;
  GObject *sequencer;
  GObject *current;

  GtkTreeModel *model;
  GtkTreeIter iter;

  guint midi_channel;
  guint audio_start, audio_end;
  guint midi_start, midi_end;
  guint i;
  gboolean found_device;

  midi_dialog = AGS_MIDI_DIALOG(applicable);

  machine = midi_dialog->machine;

  /* audio and sequencer */
  audio = machine->audio;

  /*  */
  g_object_get(audio,
	       "midi-channel", &midi_channel,
	       "audio-start-mapping", &audio_start,
	       "audio-end-mapping", &audio_end,
	       "midi-start-mapping", &midi_start,
	       "midi-end-mapping", &midi_end,
	       "input-sequencer", &sequencer,
	       NULL);
  
  /* mapping */
  gtk_spin_button_set_value(midi_dialog->midi_channel,
			    (gdouble) midi_channel);

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
  
  if(sequencer != NULL){
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(midi_dialog->midi_device));
    i = 0;

    if(model != NULL &&
       gtk_tree_model_get_iter_first(model,
				     &iter)){
      do{
	gtk_tree_model_get(model,
			   &iter,
			   1, &current,
			   -1);
      
	if(current == sequencer){
	  found_device = TRUE;
	  break;
	}
      
	i++;
      }while(gtk_tree_model_iter_next(model,
				      &iter));
    }

    g_object_unref(sequencer);
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
  GtkListStore *model;

  AgsApplicationContext *application_context;
  
  GtkTreeIter iter;
  
  GList *start_list, *list;

  /* application context and mutex manager */
  application_context = ags_application_context_get_instance();

  /* clear model */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(midi_dialog->midi_device))));

  /* tree model */
  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
  
  /* null device */
  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "NULL",
		     1, NULL,
		     -1);

  /* load sequencer */
  list =
    start_list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter,
		       0, ags_sequencer_get_device(AGS_SEQUENCER(list->data)),
		       1, list->data,
		       -1);
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  gtk_combo_box_set_model(GTK_COMBO_BOX(midi_dialog->midi_device),
			  GTK_TREE_MODEL(model));
}

void
ags_midi_dialog_show(GtkWidget *widget)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = (AgsMidiDialog *) widget;
  
  if((AGS_MIDI_DIALOG_IO_OPTIONS & (midi_dialog->flags)) != 0){
    gtk_widget_show((GtkWidget *) midi_dialog->io_options);
  }

  if((AGS_MIDI_DIALOG_MAPPING & (midi_dialog->flags)) != 0){
    gtk_widget_show((GtkWidget *) midi_dialog->mapping);
  }

  if((AGS_MIDI_DIALOG_DEVICE & (midi_dialog->flags)) != 0){
    gtk_widget_show((GtkWidget *) midi_dialog->device);
  }

  GTK_WIDGET_CLASS(ags_midi_dialog_parent_class)->show(widget);
}

void
ags_midi_dialog_activate_button_callback(GtkButton *activate_button,
					 AgsMidiDialog *midi_dialog)
{
  ags_midi_dialog_response(midi_dialog,
			   GTK_RESPONSE_ACCEPT);
}

gboolean
ags_midi_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
				     guint keyval,
				     guint keycode,
				     GdkModifierType state,
				     AgsMidiDialog *midi_dialog)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_midi_dialog_key_released_callback(GtkEventControllerKey *event_controller,
				      guint keyval,
				      guint keycode,
				      GdkModifierType state,
				      AgsMidiDialog *midi_dialog)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	ags_midi_dialog_response(midi_dialog,
				 GTK_RESPONSE_CLOSE);	
      }
      break;
    }
  }
}

gboolean
ags_midi_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
				   GdkModifierType keyval,
				   AgsMidiDialog *midi_dialog)
{
  return(FALSE);
}

void
ags_midi_dialog_real_response(AgsMidiDialog *midi_dialog,
			      gint response_id)
{
  gtk_window_destroy((GtkWindow *) midi_dialog);
}

/**
 * ags_midi_dialog_response:
 * @midi_dialog: the #AgsMidiDialog
 * @response: the response
 *
 * Response @midi_dialog due to user action.
 * 
 * Since: 6.10.0
 */
void
ags_midi_dialog_response(AgsMidiDialog *midi_dialog,
			 gint response)
{
  g_return_if_fail(AGS_IS_MIDI_DIALOG(midi_dialog));
  
  g_object_ref((GObject *) midi_dialog);
  g_signal_emit(G_OBJECT(midi_dialog),
		midi_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) midi_dialog);
}

/**
 * ags_midi_dialog_new:
 * @machine: the assigned machine.
 *
 * Creates an #AgsMidiDialog
 *
 * Returns: a new #AgsMidiDialog
 *
 * Since: 3.0.0
 */
AgsMidiDialog*
ags_midi_dialog_new(AgsMachine *machine)
{
  AgsMidiDialog *midi_dialog;

  midi_dialog = (AgsMidiDialog *) g_object_new(AGS_TYPE_MIDI_DIALOG,
					       "machine", machine,
					       NULL);

  return(midi_dialog);
}
