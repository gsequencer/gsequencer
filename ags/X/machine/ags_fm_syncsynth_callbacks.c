/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_fm_syncsynth_callbacks.h>
#include <ags/X/machine/ags_fm_oscillator.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>

#include <math.h>

void
ags_fm_syncsynth_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsFMSyncsynth *fm_syncsynth)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_FM_SYNCSYNTH)->counter);

  g_object_set(AGS_MACHINE(fm_syncsynth),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_FM_SYNCSYNTH);

  g_free(str);
}

void
ags_fm_syncsynth_samplerate_changed_callback(AgsMachine *machine,
					     guint samplerate, guint old_samplerate,
					     gpointer user_data)
{
  GList *start_list, *list;

  list = 
    start_list = gtk_container_get_children(AGS_FM_SYNCSYNTH(machine)->fm_oscillator);

  while(list != NULL){
    AgsFMOscillator *fm_oscillator;

    GList *start_child, *child;
    
    guint i;

    start_child = gtk_container_get_children(list->data);

    child = ags_list_util_find_type(start_child,
				    AGS_TYPE_FM_OSCILLATOR);

    if(child != NULL){
      fm_oscillator = child->data;
  
      gtk_spin_button_set_value(fm_oscillator->attack,
				samplerate * (gtk_spin_button_get_value(fm_oscillator->attack) / old_samplerate));

      gtk_spin_button_set_value(fm_oscillator->frame_count,
				samplerate * (gtk_spin_button_get_value(fm_oscillator->frame_count) / old_samplerate));
  
      gtk_spin_button_set_value(fm_oscillator->phase,
				samplerate * (gtk_spin_button_get_value(fm_oscillator->phase) / old_samplerate));

      for(i = 0; i < fm_oscillator->sync_point_count; i++){
	gtk_spin_button_set_value(fm_oscillator->sync_point[i * 2],
				  samplerate * (gtk_spin_button_get_value(fm_oscillator->sync_point[i * 2]) / old_samplerate));
			      
	gtk_spin_button_set_value(fm_oscillator->sync_point[i * 2 + 1],
				  samplerate * (gtk_spin_button_get_value(fm_oscillator->sync_point[i * 2 + 1]) / old_samplerate));
      }
    }

    g_list_free(start_child);
    
    list = list->next;
  }

  g_list_free(start_list);
  
  gtk_spin_button_set_value(AGS_FM_SYNCSYNTH(machine)->loop_start,
			    samplerate * (gtk_spin_button_get_value(AGS_FM_SYNCSYNTH(machine)->loop_start) / old_samplerate));

  gtk_spin_button_set_value(AGS_FM_SYNCSYNTH(machine)->loop_end,
			    samplerate * (gtk_spin_button_get_value(AGS_FM_SYNCSYNTH(machine)->loop_end) / old_samplerate));
}

void
ags_fm_syncsynth_auto_update_callback(GtkToggleButton *toggle, AgsFMSyncsynth *fm_syncsynth)
{
  if(gtk_toggle_button_get_active(toggle)){
    fm_syncsynth->flags |= AGS_FM_SYNCSYNTH_AUTO_UPDATE;
  }else{
    fm_syncsynth->flags &= (~AGS_FM_SYNCSYNTH_AUTO_UPDATE);
  }
}

void
ags_fm_syncsynth_add_callback(GtkButton *button, AgsFMSyncsynth *fm_syncsynth)
{
  AgsFMOscillator *fm_oscillator;

  fm_oscillator = ags_fm_oscillator_new();
  
  ags_fm_syncsynth_add_fm_oscillator(fm_syncsynth,
				     fm_oscillator);
  
  ags_connectable_connect(AGS_CONNECTABLE(fm_oscillator));
  
  g_signal_connect((GObject *) fm_oscillator, "control-changed",
		   G_CALLBACK(ags_fm_syncsynth_fm_oscillator_control_changed_callback), (gpointer) fm_syncsynth);

}

void
ags_fm_syncsynth_remove_callback(GtkButton *button, AgsFMSyncsynth *fm_syncsynth)
{
  GList *list, *list_start;
  GList *child_start;

  guint nth;
  
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(fm_syncsynth->fm_oscillator));

  nth = 0;
  
  while(list != NULL){
    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(child_start->data))){
      ags_fm_syncsynth_remove_fm_oscillator(fm_syncsynth,
					    nth);
    }else{
      nth++;
    }
    
    g_list_free(child_start);
    
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_fm_syncsynth_update_callback(GtkButton *button, AgsFMSyncsynth *fm_syncsynth)
{
  ags_fm_syncsynth_update(fm_syncsynth);
}

void
ags_fm_syncsynth_fm_oscillator_control_changed_callback(AgsFMOscillator *fm_oscillator,
							AgsFMSyncsynth *fm_syncsynth)
{
  ags_fm_syncsynth_reset_loop(fm_syncsynth);

  if((AGS_FM_SYNCSYNTH_AUTO_UPDATE & (fm_syncsynth->flags)) != 0){
    ags_fm_syncsynth_update(fm_syncsynth);
  }
}

void
ags_fm_syncsynth_lower_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth)
{
  //TODO:JK: implement me
}

void
ags_fm_syncsynth_loop_start_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth)
{
  //TODO:JK: implement me
}

void
ags_fm_syncsynth_loop_end_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth)
{
  //TODO:JK: implement me
}
