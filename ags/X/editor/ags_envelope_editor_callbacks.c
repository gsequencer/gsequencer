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

#include <ags/X/editor/ags_envelope_editor_callbacks.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_preset.h>

#include <ags/X/editor/ags_envelope_dialog.h>

#include <ags/i18n.h>

void
ags_envelope_editor_preset_callback(GtkWidget *combo_box,
				    AgsEnvelopeEditor *envelope_editor)
{
  ags_envelope_editor_reset_control(envelope_editor);
}

void
ags_envelope_editor_preset_add_callback(GtkWidget *button,
					AgsEnvelopeEditor *envelope_editor)
{
  GtkDialog *dialog;
  GtkEntry *entry;

  if(envelope_editor->rename != NULL){
    return;
  }
  
  envelope_editor->rename =
    dialog = (GtkDialog *) gtk_dialog_new_with_buttons(i18n("preset name"),
						       (GtkWindow *) gtk_widget_get_toplevel(GTK_WIDGET(envelope_editor)),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_STOCK_OK,
						       GTK_RESPONSE_ACCEPT,
						       GTK_STOCK_CANCEL,
						       GTK_RESPONSE_REJECT,
						       NULL);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) dialog->vbox,
		     (GtkWidget *) entry,
		     FALSE, FALSE,
		     0);

  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_envelope_editor_preset_rename_response_callback), (gpointer) envelope_editor);
}

void
ags_envelope_editor_preset_remove_callback(GtkWidget *button,
					   AgsEnvelopeEditor *envelope_editor)
{
  /* remove preset */
  ags_envelope_editor_remove_preset(envelope_editor,
				    gtk_combo_box_get_active(envelope_editor->preset));
}

void
ags_envelope_editor_preset_rename_response_callback(GtkWidget *widget, gint response,
						    AgsEnvelopeEditor *envelope_editor)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *text;

    /* get name */
    text = gtk_editable_get_chars(GTK_EDITABLE(gtk_container_get_children((GtkContainer *) GTK_DIALOG(widget)->vbox)->data),
				  0, -1);
    
    /* add preset */
    ags_envelope_editor_add_preset(envelope_editor,
				   text);
    
    /* load preset */
    ags_envelope_editor_load_preset(envelope_editor);    
  }
  
  envelope_editor->rename = NULL;
  gtk_widget_destroy(widget);

  return(0);
}

void
ags_envelope_editor_attack_x_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble attack_x;

  GValue value = {0,};

  GError *error;

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  attack_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  attack_x + val[0][1]);

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_attack_y_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble attack_y;

  GValue value = {0,};

  GError *error;

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  attack_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  val[0][1] + attack_y * I);

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_decay_x_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble decay_x;

  GValue value = {0,};

  GError *error;  

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  decay_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  decay_x + val[0][1]);

  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_decay_y_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble decay_y;

  GValue value = {0,};

  GError *error;  

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  decay_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  val[0][1] + decay_y * I);

  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_sustain_x_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble sustain_x;

  GValue value = {0,};

  GError *error;  

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  sustain_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  sustain_x + val[0][1]);

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_sustain_y_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble sustain_y;

  GValue value = {0,};

  GError *error;  

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  sustain_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  val[0][1] + sustain_y * I);

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_release_x_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble release_x;

  GValue value = {0,};

  GError *error;  

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  release_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  release_x + val[0][1]);

  ags_preset_add_parameter(preset,
			   "release", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_release_y_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble release_y;

  GValue value = {0,};

  GError *error;  

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  release_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  val[0][1] + release_y * I);

  ags_preset_add_parameter(preset,
			   "release", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_ratio_callback(GtkWidget *range, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble ratio;

  GValue value = {0,};

  GError *error;  

  if((AGS_ENVELOPE_EDITOR_NO_UPDATE & (envelope_editor->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_envelope_editor_get_active_preset(envelope_editor);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  ratio = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "ratio", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  ratio * I);

  ags_preset_add_parameter(preset,
			   "ratio", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}
