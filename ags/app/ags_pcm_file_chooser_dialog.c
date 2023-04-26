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

#include <ags/app/ags_pcm_file_chooser_dialog.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_pcm_file_chooser_dialog_class_init(AgsPCMFileChooserDialogClass *pcm_file_chooser_dialog);
void ags_pcm_file_chooser_dialog_init(AgsPCMFileChooserDialog *pcm_file_chooser_dialog);

void ags_pcm_file_chooser_dialog_show(GtkWidget *widget);

/**
 * SECTION:ags_pcm_file_chooser_dialog
 * @short_description: The PCM file chooser dialog
 * @title: AgsPCMFileChooserDialog
 * @section_id:
 * @include: ags/app/ags_pcm_file_chooser_dialog.h
 *
 * #AgsPCMFileChooserDialog lets you open PCM audio files and allows you
 * to select audio channel.
 */

static gpointer ags_pcm_file_chooser_dialog_parent_class = NULL;

GType
ags_pcm_file_chooser_dialog_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pcm_file_chooser_dialog = 0;

    static const GTypeInfo ags_pcm_file_chooser_dialog_info = {
      sizeof (AgsPCMFileChooserDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pcm_file_chooser_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPCMFileChooserDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pcm_file_chooser_dialog_init,
    };

    ags_type_pcm_file_chooser_dialog = g_type_register_static(GTK_TYPE_DIALOG,
							      "AgsPCMFileChooserDialog", &ags_pcm_file_chooser_dialog_info,
							      0);
    
    g_once_init_leave(&g_define_type_id__volatile, ags_type_pcm_file_chooser_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_pcm_file_chooser_dialog_class_init(AgsPCMFileChooserDialogClass *pcm_file_chooser_dialog)
{
  GtkWidgetClass *widget;
  
  ags_pcm_file_chooser_dialog_parent_class = g_type_class_peek_parent(pcm_file_chooser_dialog);

  widget = (GtkWidgetClass *) pcm_file_chooser_dialog;

  widget->show = ags_pcm_file_chooser_dialog_show;
}

void
ags_pcm_file_chooser_dialog_init(AgsPCMFileChooserDialog *pcm_file_chooser_dialog)
{
  GtkBox *content_area;
  GtkBox *vbox;
  GtkGrid *grid;

  gchar *str;

  /* file chooser */  
  gtk_dialog_add_buttons((GtkDialog *) pcm_file_chooser_dialog,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 i18n("_Open"), GTK_RESPONSE_ACCEPT,
			 NULL);

  pcm_file_chooser_dialog->flags = AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_AUDIO_CHANNEL;
  
  content_area = (GtkBox *) gtk_dialog_get_content_area((GtkDialog *) pcm_file_chooser_dialog);
  
  gtk_widget_set_vexpand((GtkWidget *) content_area,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) content_area,
			 TRUE);

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);
  
  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 TRUE);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);

  gtk_box_append(content_area,
		 (GtkWidget *) vbox);

  pcm_file_chooser_dialog->file_chooser = (GtkFileChooserWidget *) gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);

  gtk_widget_set_vexpand((GtkWidget *) pcm_file_chooser_dialog->file_chooser,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) pcm_file_chooser_dialog->file_chooser,
			 TRUE);

  gtk_widget_set_valign((GtkWidget *) pcm_file_chooser_dialog->file_chooser,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pcm_file_chooser_dialog->file_chooser,
			GTK_ALIGN_FILL);
  
  gtk_box_append(vbox,
		 (GtkWidget *) pcm_file_chooser_dialog->file_chooser);

  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_vexpand((GtkWidget *) grid,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) grid,
			 FALSE);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) grid);

  /* audio channel */
  str = g_strdup_printf("%s: ",
			i18n("audio channel"));
  
  pcm_file_chooser_dialog->audio_channel_label = (GtkLabel *) gtk_label_new(str);

  gtk_widget_set_halign((GtkWidget *) pcm_file_chooser_dialog->audio_channel_label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_chooser_dialog->audio_channel_label,
		  0, 0,
		  1, 1);

  g_free(str);
  
  pcm_file_chooser_dialog->audio_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											    256.0,
											    1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_chooser_dialog->audio_channel,
		  1, 0,
		  1, 1);

  /* new channel */
  pcm_file_chooser_dialog->new_channel = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("open in new channel"));

  gtk_check_button_set_active(pcm_file_chooser_dialog->new_channel,
			      TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_chooser_dialog->new_channel,
		  0, 1,
		  1, 1);

  /* existing channel */
  pcm_file_chooser_dialog->existing_channel = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("override existing links"));

  gtk_check_button_set_active(pcm_file_chooser_dialog->existing_channel,
			      TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_chooser_dialog->existing_channel,
		  0, 2,
		  1, 1);
}

void
ags_pcm_file_chooser_dialog_show(GtkWidget *widget)
{
  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;

  pcm_file_chooser_dialog = AGS_PCM_FILE_CHOOSER_DIALOG(widget);

  /* hide unneeded */
  if((AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_AUDIO_CHANNEL & (pcm_file_chooser_dialog->flags)) == 0){
    gtk_widget_set_visible((GtkWidget *) pcm_file_chooser_dialog->audio_channel_label,
			   FALSE);
    gtk_widget_set_visible((GtkWidget *) pcm_file_chooser_dialog->audio_channel,
			   FALSE);
  }

  if((AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_NEW_CHANNEL & (pcm_file_chooser_dialog->flags)) == 0){
    gtk_widget_set_visible((GtkWidget *) pcm_file_chooser_dialog->new_channel,
			   FALSE);
  }
  
  if((AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_EXISTING_CHANNEL & (pcm_file_chooser_dialog->flags)) == 0){
    gtk_widget_set_visible((GtkWidget *) pcm_file_chooser_dialog->existing_channel,
			   FALSE);
  }
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_pcm_file_chooser_dialog_parent_class)->show(widget);
}

/**
 * ags_pcm_file_chooser_dialog_new:
 * @title: the title
 * @transient_for: the transient for window
 * 
 * Create a new instance of #AgsPCMFileChooserDialog
 * 
 * Returns: the #AgsPCMFileChooserDialog
 * 
 * Since: 4.0.0
 */
AgsPCMFileChooserDialog*
ags_pcm_file_chooser_dialog_new(gchar *title,
				GtkWindow *transient_for)
{
  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;

  pcm_file_chooser_dialog = (AgsPCMFileChooserDialog *) g_object_new(AGS_TYPE_PCM_FILE_CHOOSER_DIALOG,
								     "title", title,
								     "transient-for", transient_for,
								     NULL);

  return(pcm_file_chooser_dialog);
}
