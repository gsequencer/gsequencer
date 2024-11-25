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

#include <ags/app/export/ags_wave_export_dialog.h>
#include <ags/app/export/ags_wave_export_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_navigation.h>

#include <ags/app/machine/ags_audiorec.h>

#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>

#include <math.h>

#include <ags/i18n.h>

void ags_wave_export_dialog_class_init(AgsWaveExportDialogClass *wave_export_dialog);
void ags_wave_export_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_export_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_wave_export_dialog_init(AgsWaveExportDialog *wave_export_dialog);
void ags_wave_export_dialog_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_wave_export_dialog_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_wave_export_dialog_finalize(GObject *gobject);

void ags_wave_export_dialog_close_request_callback(GtkWindow *window,
						   AgsWaveExportDialog *wave_export_dialog);

gboolean ags_wave_export_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
						     guint keyval,
						     guint keycode,
						     GdkModifierType state,
						     AgsWaveExportDialog *wave_export_dialog);
void ags_wave_export_dialog_key_released_callback(GtkEventControllerKey *event_controller,
						  guint keyval,
						  guint keycode,
						  GdkModifierType state,
						  AgsWaveExportDialog *wave_export_dialog);
gboolean ags_wave_export_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
						   GdkModifierType keyval,
						   AgsWaveExportDialog *wave_export_dialog);

gboolean ags_wave_export_dialog_is_connected(AgsConnectable *connectable);
void ags_wave_export_dialog_connect(AgsConnectable *connectable);
void ags_wave_export_dialog_disconnect(AgsConnectable *connectable);

void ags_wave_export_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_wave_export_dialog_apply(AgsApplicable *applicable);
void ags_wave_export_dialog_reset(AgsApplicable *applicable);

void ags_wave_export_dialog_show(GtkWidget *widget);

void ags_wave_export_dialog_real_response(AgsWaveExportDialog *wave_export_dialog,
					  gint response);

/**
 * SECTION:ags_wave_export_dialog
 * @short_description: The export dialog
 * @title: AgsWaveExportDialog
 * @section_id:
 * @include: ags/app/ags_wave_export_dialog.h
 *
 * #AgsWaveExportDialog lets you export to audio files. This widget
 * can contain mulitple #AgsExportSoundcard widgets.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MACHINE,
};

static gpointer ags_wave_export_dialog_parent_class = NULL;

static guint wave_export_dialog_signals[LAST_SIGNAL];

GType
ags_wave_export_dialog_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave_export_dialog = 0;

    static const GTypeInfo ags_wave_export_dialog_info = {
      sizeof (AgsWaveExportDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_export_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWaveExportDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_export_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wave_export_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_wave_export_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wave_export_dialog = g_type_register_static(GTK_TYPE_WINDOW,
							 "AgsWaveExportDialog", &ags_wave_export_dialog_info,
							 0);
    
    g_type_add_interface_static(ags_type_wave_export_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_wave_export_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave_export_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_wave_export_dialog_class_init(AgsWaveExportDialogClass *wave_export_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_wave_export_dialog_parent_class = g_type_class_peek_parent(wave_export_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) wave_export_dialog;

  gobject->set_property = ags_wave_export_dialog_set_property;
  gobject->get_property = ags_wave_export_dialog_get_property;

  gobject->finalize = ags_wave_export_dialog_finalize;

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
  widget = (GtkWidgetClass *) wave_export_dialog;

  widget->show = ags_wave_export_dialog_show;

  /* AgsWaveExportDialog */
  wave_export_dialog->response = ags_wave_export_dialog_real_response;
  
  /* signals */
  /**
   * AgsWaveExportDialog::response:
   * @wave_export_dialog: the #AgsWaveExportDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.6.0
   */
  wave_export_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(wave_export_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsWaveExportDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_wave_export_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_wave_export_dialog_is_connected;  
  connectable->connect = ags_wave_export_dialog_connect;
  connectable->disconnect = ags_wave_export_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_wave_export_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_wave_export_dialog_set_update;
  applicable->apply = ags_wave_export_dialog_apply;
  applicable->reset = ags_wave_export_dialog_reset;
}

void
ags_wave_export_dialog_init(AgsWaveExportDialog *wave_export_dialog)
{  
  GtkBox *vbox;
  GtkBox *hbox;
  GtkBox *action_area;
  GtkLabel *label;

  GtkEventController *event_controller;

  wave_export_dialog->flags = 0;
  wave_export_dialog->connectable_flags = 0;
  
  g_object_set(wave_export_dialog,
	       "title", i18n("audio fast export"),
	       NULL);

  g_signal_connect(wave_export_dialog, "close-request",
		   G_CALLBACK(ags_wave_export_dialog_close_request_callback), wave_export_dialog);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) wave_export_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_wave_export_dialog_key_pressed_callback), wave_export_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_wave_export_dialog_key_released_callback), wave_export_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_wave_export_dialog_modifiers_callback), wave_export_dialog);

  /* vbox */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_START);
  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 FALSE);
  
  gtk_box_set_spacing((GtkBox *) vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) wave_export_dialog,
		       (GtkWidget *) vbox);

  wave_export_dialog->machine = NULL;

  /* start tact */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);
  
  label = (GtkLabel *) gtk_label_new(i18n("start tact"));
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) label);
  
  wave_export_dialog->start_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, (gdouble) AGS_NAVIGATION_MAX_POSITION_TACT, 1.0);
  gtk_spin_button_set_digits(wave_export_dialog->start_tact,
			     3);
  gtk_spin_button_set_value(wave_export_dialog->start_tact,
			    0.0);
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) wave_export_dialog->start_tact);
  
  /* end tact */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);
  
  label = (GtkLabel *) gtk_label_new(i18n("end tact"));
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) label);
  
  wave_export_dialog->end_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, (gdouble) AGS_NAVIGATION_MAX_POSITION_TACT / 16.0, 1.0);
  gtk_spin_button_set_digits(wave_export_dialog->end_tact,
			     3);
  gtk_spin_button_set_value(wave_export_dialog->end_tact,
			    4.0);
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) wave_export_dialog->end_tact);
  
  /* duration */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  wave_export_dialog->duration = (GtkLabel *) gtk_label_new("0000:00.000");
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) wave_export_dialog->duration);

  /* output format */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("output format"));
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  wave_export_dialog->output_format = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_WAV);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_FLAC);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_AIFF);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_OGG);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_MP3);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_AAC);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_MP4);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_MKV);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_WEBM);
  gtk_combo_box_text_append_text(wave_export_dialog->output_format,
				 AGS_WAVE_EXPORT_DIALOG_FORMAT_MPEG);

  gtk_combo_box_set_active((GtkComboBox *) wave_export_dialog->output_format,
			   0);

  gtk_box_append(hbox,
		 (GtkWidget *) wave_export_dialog->output_format);

  /* filename */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("file"));
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  wave_export_dialog->filename = (GtkEntry *) gtk_entry_new();
  gtk_editable_set_text(GTK_EDITABLE(wave_export_dialog->filename),
			"out.wav");
  gtk_box_append(hbox,
		 (GtkWidget *) wave_export_dialog->filename);

  wave_export_dialog->file_open_button = (GtkButton *) gtk_button_new_with_label(i18n("open"));
  gtk_box_append(hbox,
		 (GtkWidget *) wave_export_dialog->file_open_button);

  /* GtkButton action-area  */
  action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_halign((GtkWidget *) action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) action_area);

  wave_export_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("export"));
  gtk_box_append(action_area,
		 (GtkWidget *) wave_export_dialog->activate_button);
}

void
ags_wave_export_dialog_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsWaveExportDialog *wave_export_dialog;

  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      AgsMachine *machine;

      machine = (AgsMachine *) g_value_get_object(value);

      if(machine == wave_export_dialog->machine){
	return;
      }

      if(wave_export_dialog->machine != NULL){
	g_object_unref(wave_export_dialog->machine);
      }

      if(machine != NULL){
	g_object_ref(machine);
      }

      wave_export_dialog->machine = machine;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_export_dialog_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsWaveExportDialog *wave_export_dialog;

  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      g_value_set_object(value, wave_export_dialog->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_export_dialog_finalize(GObject *gobject)
{
  AgsWaveExportDialog *wave_export_dialog;

  wave_export_dialog = (AgsWaveExportDialog *) gobject;

  if(wave_export_dialog->machine != NULL){
    wave_export_dialog->machine->wave_export_dialog = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_wave_export_dialog_parent_class)->finalize(gobject);
}

gboolean
ags_wave_export_dialog_is_connected(AgsConnectable *connectable)
{
  AgsWaveExportDialog *wave_export_dialog;
  
  gboolean is_connected;
  
  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (wave_export_dialog->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_wave_export_dialog_connect(AgsConnectable *connectable)
{
  AgsWaveExportDialog *wave_export_dialog;

  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  wave_export_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_signal_connect(G_OBJECT(wave_export_dialog->file_open_button), "clicked",
		   G_CALLBACK(ags_wave_export_dialog_file_open_button_callback), wave_export_dialog);
  
  g_signal_connect_after(G_OBJECT(wave_export_dialog->start_tact), "value-changed",
			 G_CALLBACK(ags_wave_export_dialog_start_tact_callback), wave_export_dialog);

  g_signal_connect_after(G_OBJECT(wave_export_dialog->end_tact), "value-changed",
			 G_CALLBACK(ags_wave_export_dialog_end_tact_callback), wave_export_dialog);

  g_signal_connect(G_OBJECT(wave_export_dialog->activate_button), "clicked",
		   G_CALLBACK(ags_wave_export_dialog_save_as_callback), wave_export_dialog);
}

void
ags_wave_export_dialog_disconnect(AgsConnectable *connectable)
{
  AgsWaveExportDialog *wave_export_dialog;

  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  wave_export_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(G_OBJECT(wave_export_dialog->file_open_button),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_export_dialog_file_open_button_callback),
		      wave_export_dialog,
		      NULL);
  
  g_object_disconnect(G_OBJECT(wave_export_dialog->start_tact),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_wave_export_dialog_start_tact_callback),
		      wave_export_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_export_dialog->end_tact),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_wave_export_dialog_end_tact_callback),
		      wave_export_dialog,
		      NULL);
  
  g_object_disconnect(G_OBJECT(wave_export_dialog->activate_button),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_export_dialog_save_as_callback),
		      wave_export_dialog,
		      NULL);
}

void
ags_wave_export_dialog_close_request_callback(GtkWindow *window,
					      AgsWaveExportDialog *wave_export_dialog)
{
  ags_wave_export_dialog_response(wave_export_dialog,
				  GTK_RESPONSE_CANCEL);
}

void
ags_wave_export_dialog_activate_button_callback(GtkButton *activate_button,
						AgsWaveExportDialog *wave_export_dialog)
{
  ags_wave_export_dialog_response(wave_export_dialog,
				  GTK_RESPONSE_ACCEPT);
}

gboolean
ags_wave_export_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
					    guint keyval,
					    guint keycode,
					    GdkModifierType state,
					    AgsWaveExportDialog *wave_export_dialog)
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
ags_wave_export_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					     guint keyval,
					     guint keycode,
					     GdkModifierType state,
					     AgsWaveExportDialog *wave_export_dialog)
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
	ags_wave_export_dialog_response(wave_export_dialog,
					GTK_RESPONSE_CANCEL);	
      }
      break;
    }
  }
}

gboolean
ags_wave_export_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
					  GdkModifierType keyval,
					  AgsWaveExportDialog *wave_export_dialog)
{
  return(FALSE);
}

void
ags_wave_export_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_wave_export_dialog_apply(AgsApplicable *applicable)
{
  AgsMachine *machine;
  AgsWaveExportDialog *wave_export_dialog;

  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  gchar *filename;
  
  gdouble bpm;
  gdouble delay, delay_factor;
  gdouble start_tact, end_tact;
  gdouble offset;
  guint default_offset;
  guint64 start_frame, end_frame;
  guint samplerate;
  guint buffer_size;
  
  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(applicable);

  application_context = ags_application_context_get_instance();

  machine = wave_export_dialog->machine;
  
  soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(soundcard));

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(soundcard));
  delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(soundcard));

  /* get some fields */
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  g_object_get(machine->audio,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,	       
	       NULL);
  
  filename = gtk_editable_get_text(GTK_EDITABLE(wave_export_dialog->filename));
  
  start_tact = gtk_spin_button_get_value(wave_export_dialog->start_tact);
  end_tact = gtk_spin_button_get_value(wave_export_dialog->end_tact);

  offset = 16.0 * (end_tact - start_tact);

  default_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

  start_frame = ((16.0 * start_tact) / (16.0 * delay_factor * bpm / 60.0)) * samplerate;
  end_frame = ((16.0 * end_tact) / (16.0 * delay_factor * bpm / 60.0)) * samplerate + buffer_size;  

  if(AGS_IS_AUDIOREC(machine)){
    if(filename == NULL){
      filename = gtk_editable_get_text(GTK_EDITABLE(AGS_AUDIOREC(machine)->filename));
    }
    
    ags_audiorec_fast_export((AgsAudiorec *) machine,
			     filename,
			     start_frame, end_frame);
  }
}

void
ags_wave_export_dialog_reset(AgsApplicable *applicable)
{
  AgsWaveExportDialog *wave_export_dialog;

  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(applicable);

  ags_wave_export_dialog_update_duration(wave_export_dialog);
}

void
ags_wave_export_dialog_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_wave_export_dialog_parent_class)->show(widget);
}

void
ags_wave_export_dialog_update_duration(AgsWaveExportDialog *wave_export_dialog)
{
  AgsApplicationContext *application_context;
  
  GObject *soundcard;

  gchar *str;
  
  gdouble bpm;
  gdouble delay, delay_factor;
  gdouble start_tact, end_tact;
  gdouble offset;
  
  if(!AGS_IS_WAVE_EXPORT_DIALOG(wave_export_dialog)){
    return;
  }

  application_context = ags_application_context_get_instance();

  soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(soundcard));

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(soundcard));
  delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(soundcard));
  
  start_tact = gtk_spin_button_get_value(wave_export_dialog->start_tact);
  end_tact = gtk_spin_button_get_value(wave_export_dialog->end_tact);
  
  offset = 16.0 * (end_tact - start_tact);

  str = ags_time_get_uptime_from_offset(offset,
					bpm,
					delay,
					delay_factor);

  gtk_label_set_label(wave_export_dialog->duration,
		      str);
  
  g_free(str);
}


void
ags_wave_export_dialog_real_response(AgsWaveExportDialog *wave_export_dialog,
				     gint response)
{  
  gtk_widget_set_visible((GtkWidget *) wave_export_dialog,
			 FALSE);
}

/**
 * ags_wave_export_dialog_response:
 * @wave_export_dialog: the #AgsWaveExportDialog
 * @response: the response
 *
 * Response @wave_export_dialog due to user action.
 * 
 * Since: 6.6.0
 */
void
ags_wave_export_dialog_response(AgsWaveExportDialog *wave_export_dialog,
				gint response)
{
  g_return_if_fail(AGS_IS_WAVE_EXPORT_DIALOG(wave_export_dialog));
  
  g_object_ref((GObject *) wave_export_dialog);
  g_signal_emit(G_OBJECT(wave_export_dialog),
		wave_export_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) wave_export_dialog);
}

/**
 * ags_wave_export_dialog_new:
 * @machine: the #AgsMachine
 * 
 * Create a new instance of #AgsWaveExportDialog
 * 
 * Returns: the #AgsWaveExportDialog
 * 
 * Since: 3.0.0
 */
AgsWaveExportDialog*
ags_wave_export_dialog_new(GtkWindow *transient_for,
			   AgsMachine *machine)
{
  AgsWaveExportDialog *wave_export_dialog;

  wave_export_dialog = (AgsWaveExportDialog *) g_object_new(AGS_TYPE_WAVE_EXPORT_DIALOG,
							    "transient-for", transient_for,
							    "machine", machine,
							    NULL);

  return(wave_export_dialog);
}
