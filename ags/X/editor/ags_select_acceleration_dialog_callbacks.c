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

#include <ags/X/editor/ags_select_acceleration_dialog_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_machine.h>

void
ags_select_acceleration_dialog_response_callback(GtkWidget *dialog, gint response,
						 AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  gboolean hide_dialog;

  hide_dialog = TRUE;
  
  switch(response){
  case GTK_RESPONSE_APPLY:
    {
      hide_dialog = FALSE;
    }
  case GTK_RESPONSE_OK:
    {
      ags_applicable_apply(AGS_APPLICABLE(select_acceleration_dialog));
    }
  case GTK_RESPONSE_CANCEL:
    {
      if(hide_dialog){
	gtk_widget_hide(select_acceleration_dialog);
      }
    }
  }
}

void
ags_select_acceleration_dialog_add_callback(GtkWidget *button,
					    AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  AgsWindow *window;
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;

  GtkHBox *hbox;
  GtkComboBoxText *combo_box;
  GtkButton *remove;

  AgsAudio *audio;
  
  AgsMutexManager *mutex_manager;

  gchar **specifier;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;


  window = select_acceleration_dialog->main_window;
  automation_editor = window->automation_window->automation_editor;

  machine = automation_editor->selected_machine;

  audio = machine->audio;
  
  /* get mutex manager and application mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* select automation */
  hbox = (GtkVBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) select_acceleration_dialog->port,
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  /* automation combo box */
  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     GTK_WIDGET(combo_box),
		     FALSE, FALSE,
		     0);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /*  */  
  pthread_mutex_lock(audio_mutex);

  specifier = ags_automation_get_specifier_unique(audio->automation);

  pthread_mutex_unlock(audio_mutex);

  for(; *specifier != NULL; specifier++){
    gtk_combo_box_text_append_text(select_acceleration_dialog->port,
				   g_strdup(*specifier));
  }
  
  /* remove button */
  remove = (GtkCheckButton *) gtk_button_new_from_stock(GTK_STOCK_REMOVE);
  g_signal_connect(remove, "clicked",
		   G_CALLBACK(ags_select_acceleration_dialog_remove_callback), select_acceleration_dialog);
  gtk_box_pack_start((GtkBox *) hbox,
		     remove,
		     FALSE, FALSE,
		     0);
}

void
ags_select_acceleration_dialog_remove_callback(GtkWidget *button,
					       AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  gtk_widget_destroy(button->parent);
}
