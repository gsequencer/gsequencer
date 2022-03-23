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

#include <ags/app/ags_input_dialog.h>

#include <ags/i18n.h>

void ags_input_dialog_class_init(AgsInputDialogClass *input_dialog);
void ags_input_dialog_init(AgsInputDialog *input_dialog);

/**
 * SECTION:ags_input_dialog
 * @short_description: Edit INPUT settings
 * @title: AgsInputDialog
 * @section_id:
 * @include: ags/app/ags_input_dialog.h
 *
 * #AgsInputDialog is a composite widget to edit INPUT settings.
 */

static gpointer ags_input_dialog_parent_class = NULL;

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

    ags_type_input_dialog = g_type_register_static(GTK_TYPE_DIALOG,
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
}

void
ags_input_dialog_init(AgsInputDialog *input_dialog)
{
  input_dialog->flags = 0;

  input_dialog->string_input = NULL;

  gtk_dialog_add_buttons(input_dialog,
			 i18n("_OK"),
			 GTK_RESPONSE_ACCEPT,
			 i18n("_Cancel"),
			 GTK_RESPONSE_REJECT,
			 NULL);
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
    input_dialog->string_input = gtk_entry_new();
    gtk_box_append(gtk_dialog_get_content_area(input_dialog),
		   input_dialog->string_input);

    gtk_widget_show(input_dialog->string_input);
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
    gtk_box_remove(gtk_dialog_get_content_area(input_dialog),
		   input_dialog->string_input);

    g_object_run_dispose(input_dialog->string_input);
    g_object_unref(input_dialog->string_input);
    
    input_dialog->string_input = NULL;
  }
  
  input_dialog->flags &= (~flags);
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
