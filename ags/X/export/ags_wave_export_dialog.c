/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/export/ags_wave_export_dialog.h>
#include <ags/X/export/ags_wave_export_dialog_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_notation_editor.h>

#include <ags/X/machine/ags_audiorec.h>

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

void ags_wave_export_dialog_connect(AgsConnectable *connectable);
void ags_wave_export_dialog_disconnect(AgsConnectable *connectable);

void ags_wave_export_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_wave_export_dialog_apply(AgsApplicable *applicable);
void ags_wave_export_dialog_reset(AgsApplicable *applicable);

void ags_wave_export_dialog_show(GtkWidget *widget);

/**
 * SECTION:ags_wave_export_dialog
 * @short_description: The export dialog
 * @title: AgsWaveExportDialog
 * @section_id:
 * @include: ags/X/ags_wave_export_dialog.h
 *
 * #AgsWaveExportDialog lets you export to audio files. This widget
 * can contain mulitple #AgsExportSoundcard widgets.
 */

enum{
  PROP_0,
  PROP_MACHINE,
};

static gpointer ags_wave_export_dialog_parent_class = NULL;

GType
ags_wave_export_dialog_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

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

    ags_type_wave_export_dialog = g_type_register_static(GTK_TYPE_DIALOG,
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
}

void
ags_wave_export_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_wave_export_dialog_connect;
  connectable->disconnect = ags_wave_export_dialog_disconnect;
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
  GtkHBox *hbox;
  GtkLabel *label;

  wave_export_dialog->flags = 0;

  g_object_set(wave_export_dialog,
	       "title", i18n("Audio fast export"),
	       NULL);

  wave_export_dialog->machine = NULL;

  /* filename */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(wave_export_dialog)),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("file"));
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     TRUE, TRUE,
		     0);

  wave_export_dialog->filename = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(wave_export_dialog->filename,
		     "out.wav");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(wave_export_dialog->filename),
		     TRUE, TRUE,
		     0);

  wave_export_dialog->file_chooser_button = (GtkButton *) gtk_button_new_with_label(i18n("open"));
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(wave_export_dialog->file_chooser_button),
		     TRUE, TRUE,
		     0);

  /* start tact */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(wave_export_dialog)),
		     GTK_WIDGET(hbox),
  		     FALSE, FALSE,
  		     0);
  
  label = (GtkLabel *) gtk_label_new(i18n("start tact"));
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
  		     TRUE, TRUE,
  		     0);
  
  wave_export_dialog->start_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, (gdouble) AGS_NOTATION_EDITOR_MAX_CONTROLS, 1.0);
  gtk_spin_button_set_digits(wave_export_dialog->start_tact,
			     3);
  gtk_spin_button_set_value(wave_export_dialog->start_tact,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(wave_export_dialog->start_tact),
  		     FALSE, FALSE,
  		     0);
  
  /* end tact */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(wave_export_dialog)),
		     GTK_WIDGET(hbox),
  		     FALSE, FALSE,
  		     0);
  
  label = (GtkLabel *) gtk_label_new(i18n("end tact"));
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(label),
  		     TRUE, TRUE,
  		     0);
  
  wave_export_dialog->end_tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, (gdouble) AGS_NOTATION_EDITOR_MAX_CONTROLS / 16.0, 1.0);
  gtk_spin_button_set_digits(wave_export_dialog->end_tact,
			     3);
  gtk_spin_button_set_value(wave_export_dialog->end_tact,
			    4.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(wave_export_dialog->end_tact),
  		     FALSE, FALSE,
  		     0);
  
  /* duration */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(wave_export_dialog)),
		     GTK_WIDGET(hbox),
  		     FALSE, FALSE,
  		     0);

  wave_export_dialog->duration = (GtkLabel *) gtk_label_new("0000:00.000");
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(wave_export_dialog->duration),
		     FALSE, FALSE,
		     0);

  /* output format */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(wave_export_dialog)),
		     GTK_WIDGET(hbox),
  		     FALSE, FALSE,
  		     0);

  label = (GtkLabel *) gtk_label_new(i18n("output format"));
  g_object_set(G_OBJECT(label),
	       "xalign", 0.0,
	       NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     TRUE, TRUE,
		     0);

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

  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(wave_export_dialog->output_format),
		     FALSE, FALSE,
		     0);

  /* GtkButton's in GtkDialog->action_area  */
  wave_export_dialog->apply = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_APPLY);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_action_area(GTK_DIALOG(wave_export_dialog)),
		     (GtkWidget *) wave_export_dialog->apply,
		     FALSE, FALSE,
		     0);
  
  wave_export_dialog->ok = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OK);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_action_area(GTK_DIALOG(wave_export_dialog)),
		     (GtkWidget *) wave_export_dialog->ok,
		     FALSE, FALSE,
		     0);
  
  wave_export_dialog->cancel = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_CANCEL);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_action_area(GTK_DIALOG(wave_export_dialog)),
		     (GtkWidget *) wave_export_dialog->cancel,
		     FALSE, FALSE,
		     0);
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
  
  G_OBJECT_CLASS(ags_wave_export_dialog_parent_class)->finalize(gobject);
}

void
ags_wave_export_dialog_connect(AgsConnectable *connectable)
{
  AgsWaveExportDialog *wave_export_dialog;

  GList *list, *list_start;

  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(connectable);

  if((AGS_WAVE_EXPORT_DIALOG_CONNECTED & (wave_export_dialog->flags)) != 0){
    return;
  }

  wave_export_dialog->flags |= AGS_WAVE_EXPORT_DIALOG_CONNECTED;

  g_signal_connect((GObject *) wave_export_dialog, "delete-event",
		   G_CALLBACK(ags_wave_export_dialog_delete_event), (gpointer) wave_export_dialog);
  
  g_signal_connect(G_OBJECT(wave_export_dialog->file_chooser_button), "clicked",
		   G_CALLBACK(ags_wave_export_dialog_file_chooser_button_callback), wave_export_dialog);
  
  g_signal_connect_after(G_OBJECT(wave_export_dialog->start_tact), "value-changed",
			 G_CALLBACK(ags_wave_export_dialog_start_tact_callback), wave_export_dialog);

  g_signal_connect_after(G_OBJECT(wave_export_dialog->end_tact), "value-changed",
			 G_CALLBACK(ags_wave_export_dialog_end_tact_callback), wave_export_dialog);

  g_signal_connect(G_OBJECT(wave_export_dialog->apply), "clicked",
		   G_CALLBACK(ags_wave_export_dialog_apply_callback), wave_export_dialog);

  g_signal_connect(G_OBJECT(wave_export_dialog->ok), "clicked",
		   G_CALLBACK(ags_wave_export_dialog_ok_callback), wave_export_dialog);

  g_signal_connect(G_OBJECT(wave_export_dialog->cancel), "clicked",
		   G_CALLBACK(ags_wave_export_dialog_cancel_callback), wave_export_dialog);
}

void
ags_wave_export_dialog_disconnect(AgsConnectable *connectable)
{
  AgsWaveExportDialog *wave_export_dialog;

  GList *list, *list_start;

  wave_export_dialog = AGS_WAVE_EXPORT_DIALOG(connectable);

  if((AGS_WAVE_EXPORT_DIALOG_CONNECTED & (wave_export_dialog->flags)) == 0){
    return;
  }

  wave_export_dialog->flags &= (~AGS_WAVE_EXPORT_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(wave_export_dialog->file_chooser_button),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_export_dialog_file_chooser_button_callback),
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
  
  g_object_disconnect(G_OBJECT(wave_export_dialog->apply),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_export_dialog_apply_callback),
		      wave_export_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_export_dialog->ok),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_export_dialog_ok_callback),
		      wave_export_dialog,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_export_dialog->cancel),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_export_dialog_cancel_callback),
		      wave_export_dialog,
		      NULL);
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
  
  filename = gtk_entry_get_text(wave_export_dialog->filename);
  
  start_tact = gtk_spin_button_get_value(wave_export_dialog->start_tact);
  end_tact = gtk_spin_button_get_value(wave_export_dialog->end_tact);

  offset = 16.0 * (end_tact - start_tact);

  default_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

  start_frame = ((16.0 * start_tact) / (16.0 * delay_factor * bpm / 60.0)) * samplerate;
  end_frame = ((16.0 * end_tact) / (16.0 * delay_factor * bpm / 60.0)) * samplerate + buffer_size;  

  if(AGS_IS_AUDIOREC(machine)){
    if(filename == NULL){
      filename = gtk_entry_get_text(AGS_AUDIOREC(machine)->filename);
    }
    
    ags_audiorec_fast_export(machine,
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
ags_wave_export_dialog_new(AgsMachine *machine)
{
  AgsWaveExportDialog *wave_export_dialog;

  wave_export_dialog = (AgsWaveExportDialog *) g_object_new(AGS_TYPE_WAVE_EXPORT_DIALOG,
							    "machine", machine,
							    NULL);

  return(wave_export_dialog);
}
