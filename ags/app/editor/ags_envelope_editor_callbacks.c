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

#include <ags/app/editor/ags_envelope_editor_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_input_dialog.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_envelope_dialog.h>

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

  if(envelope_editor->rename != NULL){
    return;
  }
  
  envelope_editor->rename =
    dialog = (GtkDialog *) ags_input_dialog_new(i18n("preset name"),
						(GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(envelope_editor),
										      AGS_TYPE_ENVELOPE_DIALOG));
  ags_input_dialog_set_flags((AgsInputDialog *) dialog,
			     AGS_INPUT_DIALOG_SHOW_STRING_INPUT);
  
  gtk_widget_show((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_envelope_editor_preset_rename_response_callback), (gpointer) envelope_editor);
}

void
ags_envelope_editor_preset_remove_callback(GtkWidget *button,
					   AgsEnvelopeEditor *envelope_editor)
{
  AgsEnvelopeDialog *envelope_dialog;

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) envelope_editor,
								  AGS_TYPE_ENVELOPE_DIALOG);
  
  /* remove preset */
  ags_envelope_editor_remove_preset(envelope_editor,
				    gtk_combo_box_get_active(GTK_COMBO_BOX(envelope_editor->preset)));

  /* load preset */
  ags_envelope_dialog_load_preset(envelope_dialog);
}

void
ags_envelope_editor_preset_rename_response_callback(GtkWidget *widget, gint response,
						    AgsEnvelopeEditor *envelope_editor)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsEnvelopeDialog *envelope_dialog;

    gchar *text;

    envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) envelope_editor,
								    AGS_TYPE_ENVELOPE_DIALOG);

    /* get name */
    text = gtk_editable_get_chars(GTK_EDITABLE(AGS_INPUT_DIALOG(widget)->string_input),
				  0,
				  -1);
    
    /* add preset */
    ags_envelope_editor_add_preset(envelope_editor,
				   text);
    
    /* load preset */
    ags_envelope_dialog_load_preset(envelope_dialog);
  }
  
  envelope_editor->rename = NULL;

  gtk_window_destroy((GtkWindow *) widget);
}

void
ags_envelope_editor_attack_x_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble attack_x;

  GValue value = G_VALUE_INIT;

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
  attack_x = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].real = attack_x;

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_attack_y_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble attack_y;

  GValue value = G_VALUE_INIT;

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
  attack_y = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = attack_y;

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_decay_x_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble decay_x;

  GValue value = G_VALUE_INIT;

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
  decay_x = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].real = decay_x;

  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_decay_y_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble decay_y;

  GValue value = G_VALUE_INIT;

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
  decay_y = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = decay_y;

  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_sustain_x_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble sustain_x;

  GValue value = G_VALUE_INIT;

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
  sustain_x = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].real = sustain_x;

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_sustain_y_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble sustain_y;

  GValue value = G_VALUE_INIT;

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
  sustain_y = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = sustain_y;

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_release_x_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble release_x;

  GValue value = G_VALUE_INIT;

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
  release_x = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].real = release_x;

  ags_preset_add_parameter(preset,
			   "release", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_release_y_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble release_y;

  GValue value = G_VALUE_INIT;

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
  release_y = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = release_y;
  
  ags_preset_add_parameter(preset,
			   "release", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}

void
ags_envelope_editor_ratio_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble ratio;

  GValue value = G_VALUE_INIT;

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
  ratio = ags_dial_get_value((AgsDial *) dial);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "ratio", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = ratio;

  ags_preset_add_parameter(preset,
			   "ratio", &value);

  /* plot */
  ags_envelope_editor_plot(envelope_editor);
}
