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

#include <ags/widget/ags_input_dialog.h>

#include <ags/i18n.h>

void ags_input_dialog_class_init(AgsInputDialogClass *input_dialog);
void ags_input_dialog_init(AgsInputDialog *input_dialog);

void ags_input_dialog_clicked_callback(GtkButton *button,
				       AgsInputDialog *input_dialog);

/**
 * SECTION:ags_input_dialog
 * @short_description: edit input settings
 * @title: AgsInputDialog
 * @section_id:
 * @include: ags/widget/ags_input_dialog.h
 *
 * #AgsInputDialog is a dialog widget to edit input settings.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

static gpointer ags_input_dialog_parent_class = NULL;
static guint input_dialog_signals[LAST_SIGNAL];

GType
ags_input_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_input_dialog = 0;

    static const GTypeInfo ags_input_dialog_info = {
      sizeof (AgsInputDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_input_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInputDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_input_dialog_init,
    };

    ags_type_input_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						   "AgsInputDialog", &ags_input_dialog_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_input_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_input_dialog_class_init(AgsInputDialogClass *input_dialog)
{
  ags_input_dialog_parent_class = g_type_class_peek_parent(input_dialog);

  /* AgsInputDialogClass */
  input_dialog->response = NULL;

  /* signals */
  /**
   * AgsInputDialog::response:
   * @input_dialog: the #AgsInputDialog
   * @response: the response
   *
   * The ::response signal notifies adjustment value changed.
   *
   * Since: 6.6.0
   */
  input_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(input_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsInputDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_input_dialog_init(AgsInputDialog *input_dialog)
{
  GtkBox *button_box;
  
  input_dialog->flags = 0;

  input_dialog->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,
				   6);
  gtk_window_set_child(input_dialog,
		       input_dialog->vbox);

  /* input */
  input_dialog->input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					6);
  gtk_box_append(input_dialog->vbox,
		 input_dialog->input_box);

  input_dialog->text = NULL;

  input_dialog->string_input = NULL;
  
  input_dialog->spin_button_label = NULL;
  input_dialog->spin_button_input = NULL;

  /* button */
  button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
			   6);

  gtk_widget_set_halign(button_box,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand(button_box,
			 TRUE);
  
  gtk_box_append(input_dialog->vbox,
		 button_box);
  
  input_dialog->ok = gtk_button_new_with_mnemonic(i18n("_OK"));

  gtk_widget_set_hexpand(input_dialog->ok,
			 TRUE);

  gtk_widget_set_halign(input_dialog->ok,
			GTK_ALIGN_END);

  gtk_box_append(button_box,
		 input_dialog->ok);

  g_signal_connect(input_dialog->ok, "clicked",
		   G_CALLBACK(ags_input_dialog_clicked_callback), input_dialog);
  
  input_dialog->cancel = gtk_button_new_with_mnemonic(i18n("_Cancel"));

  gtk_widget_set_hexpand(input_dialog->cancel,
			 FALSE);

  gtk_widget_set_halign(input_dialog->cancel,
			GTK_ALIGN_END);

  gtk_box_append(button_box,
		 input_dialog->cancel);

  g_signal_connect(input_dialog->cancel, "clicked",
		   G_CALLBACK(ags_input_dialog_clicked_callback), input_dialog);
}

void
ags_input_dialog_clicked_callback(GtkButton *button,
				  AgsInputDialog *input_dialog)
{
  if(button == input_dialog->ok){
    ags_input_dialog_response(input_dialog,
			      GTK_RESPONSE_ACCEPT);
  }else{
    ags_input_dialog_response(input_dialog,
			      GTK_RESPONSE_REJECT);
  }
}

/**
 * ags_input_dialog_test_flags:
 * @input_dialog: the #AgsInputDialog
 * @flags: the flags
 * 
 * Test flags to be set.
 * 
 * Returns: %TRUE if flags is set, otherwise %FALSE
 * 
 * Since: 4.0.0
 */
gboolean
ags_input_dialog_test_flags(AgsInputDialog *input_dialog,
			    guint flags)
{
  guint retval;
  
  g_return_val_if_fail(AGS_IS_INPUT_DIALOG(input_dialog),
		       FALSE);

  retval = ((flags & (input_dialog->flags)) != 0) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_input_dialog_set_flags:
 * @input_dialog: the #AgsInputDialog
 * @flags: the flags
 * 
 * Set @flags.
 * 
 * Since: 4.0.0
 */
void
ags_input_dialog_set_flags(AgsInputDialog *input_dialog,
			   guint flags)
{
  g_return_if_fail(AGS_IS_INPUT_DIALOG(input_dialog));

  if((AGS_INPUT_DIALOG_SHOW_STRING_INPUT & (flags)) != 0 &&
     input_dialog->string_input == NULL){
    input_dialog->string_input = (GtkEntry *) gtk_entry_new();
    gtk_box_append(input_dialog->input_box,
		   (GtkWidget *) input_dialog->string_input);

    gtk_widget_show((GtkWidget *) input_dialog->string_input);
  }else if((AGS_INPUT_DIALOG_SHOW_SPIN_BUTTON_INPUT & (flags)) != 0 &&
	   input_dialog->spin_button_input == NULL){
    GtkBox *hbox;

    hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				  6);
    gtk_box_append(input_dialog->input_box,
		   (GtkWidget *) hbox);

    input_dialog->spin_button_label = (GtkLabel *) gtk_label_new(NULL);
    gtk_box_append(hbox,
		   (GtkWidget *) input_dialog->spin_button_label);
    
    input_dialog->spin_button_input = (GtkSpinButton *) gtk_spin_button_new(NULL,
									    1.0,
									    0);
    gtk_box_append(hbox,
		   (GtkWidget *) input_dialog->spin_button_input);

    gtk_widget_show((GtkWidget *) hbox);
  }
  
  input_dialog->flags |= flags;
}

/**
 * ags_input_dialog_unset_flags:
 * @input_dialog: the #AgsInputDialog
 * @flags: the flags
 * 
 * Unset @flags.
 * 
 * Since: 4.0.0
 */
void
ags_input_dialog_unset_flags(AgsInputDialog *input_dialog,
			     guint flags)
{
  g_return_if_fail(AGS_IS_INPUT_DIALOG(input_dialog));

  if((AGS_INPUT_DIALOG_SHOW_STRING_INPUT & (flags)) != 0 &&
     input_dialog->string_input != NULL){
    gtk_box_remove((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) input_dialog),
		   (GtkWidget *) input_dialog->string_input);

    g_object_run_dispose(G_OBJECT(input_dialog->string_input));
    
    input_dialog->string_input = NULL;
  }
  
  input_dialog->flags &= (~flags);
}

/**
 * ags_input_dialog_set_text:
 * @input_dialog: the #AgsInputDialog
 * @text: the text
 * 
 * Set informal text.
 * 
 * Since: 6.6.0
 */
void
ags_input_dialog_set_text(AgsInputDialog *input_dialog,
			  gchar *text)
{
  g_return_if_fail(AGS_IS_INPUT_DIALOG(input_dialog));

  if(input_dialog->text == NULL){
    input_dialog->text = gtk_label_new(text);
    gtk_box_prepend(input_dialog->input_box,
		    input_dialog->text);
  }else{
    gtk_label_set_text(input_dialog->text,
		       text);
  }
}

/**
 * ags_input_dialog_set_spin_button_label:
 * @input_dialog: the #AgsInputDialog
 * @label: the spin button label
 * 
 * Set label of spin button.
 * 
 * Since: 6.6.0
 */
void
ags_input_dialog_set_spin_button_label(AgsInputDialog *input_dialog,
				       gchar *label)
{
  g_return_if_fail(AGS_IS_INPUT_DIALOG(input_dialog));

  if(input_dialog->spin_button_label != NULL){
    gtk_label_set_text(input_dialog->spin_button_label,
		       label);
  }
}

/**
 * ags_input_dialog_get_entry:
 * @input_dialog: the #AgsInputDialog
 * 
 * The #GtkEntry to get input from.
 *
 * Returns: (transfer none): the entry widget
 * 
 * Since: 6.6.0
 */
GtkEntry*
ags_input_dialog_get_entry(AgsInputDialog *input_dialog)
{
  g_return_val_if_fail(AGS_IS_INPUT_DIALOG(input_dialog), NULL);

  return(input_dialog->string_input);
}

/**
 * ags_input_dialog_get_spin_button:
 * @input_dialog: the #AgsInputDialog
 * 
 * The #GtkSpinButton to get input from.
 *
 * Returns: (transfer none): the spin button widget
 * 
 * Since: 6.6.0
 */
GtkSpinButton*
ags_input_dialog_get_spin_button(AgsInputDialog *input_dialog)
{
  g_return_val_if_fail(AGS_IS_INPUT_DIALOG(input_dialog), NULL);

  return(input_dialog->spin_button_input);
}

/**
 * ags_input_dialog_response:
 * @input_dialog: the #AgsInputDialog
 * @response: the response
 * 
 * Emit signal  AgsInputDialog::response().
 * 
 * Since: 6.6.0
 */
void
ags_input_dialog_response(AgsInputDialog *input_dialog,
			  gint response)
{
  g_return_if_fail(AGS_IS_INPUT_DIALOG(input_dialog));

  g_object_ref((GObject *) input_dialog);
  g_signal_emit(G_OBJECT(input_dialog),
		input_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) input_dialog);
}

/**
 * ags_input_dialog_new:
 * @title: the title
 * @transient_for: the transient for
 *
 * Creates an #AgsInputDialog
 *
 * Returns: a new #AgsInputDialog
 *
 * Since: 4.0.0
 */
AgsInputDialog*
ags_input_dialog_new(gchar *title,
		     GtkWindow *transient_for)
{
  AgsInputDialog *input_dialog;

  input_dialog = (AgsInputDialog *) g_object_new(AGS_TYPE_INPUT_DIALOG,
						 "title", title,
						 "transient-for", transient_for,
						 NULL);

  return(input_dialog);
}