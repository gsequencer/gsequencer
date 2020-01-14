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

#include <ags/X/machine/ags_syncsynth_callbacks.h>
#include <ags/X/machine/ags_oscillator.h>

#include <ags/X/ags_window.h>

#include <math.h>

void
ags_syncsynth_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsSyncsynth *syncsynth)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_SYNCSYNTH)->counter);

  g_object_set(AGS_MACHINE(syncsynth),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_SYNCSYNTH);

  g_free(str);
}

void
ags_syncsynth_samplerate_changed_callback(AgsMachine *machine,
					  guint samplerate, guint old_samplerate,
					  gpointer user_data)
{
  GList *start_list, *list;

  list = 
    start_list = gtk_container_get_children(AGS_SYNCSYNTH(machine)->oscillator);

  while(list != NULL){
    AgsOscillator *oscillator;

    GList *start_child, *child;
    
    guint i;

    start_child = gtk_container_get_children(list->data);

    child = ags_list_util_find_type(start_child,
				    AGS_TYPE_OSCILLATOR);

    if(child != NULL){
      oscillator = child->data;
  
      gtk_spin_button_set_value(oscillator->attack,
				samplerate * (gtk_spin_button_get_value(oscillator->attack) / old_samplerate));

      gtk_spin_button_set_value(oscillator->frame_count,
				samplerate * (gtk_spin_button_get_value(oscillator->frame_count) / old_samplerate));
  
      gtk_spin_button_set_value(oscillator->phase,
				samplerate * (gtk_spin_button_get_value(oscillator->phase) / old_samplerate));

      for(i = 0; i < oscillator->sync_point_count; i++){
	gtk_spin_button_set_value(oscillator->sync_point[i * 2],
				  samplerate * (gtk_spin_button_get_value(oscillator->sync_point[i * 2]) / old_samplerate));
			      
	gtk_spin_button_set_value(oscillator->sync_point[i * 2 + 1],
				  samplerate * (gtk_spin_button_get_value(oscillator->sync_point[i * 2 + 1]) / old_samplerate));
      }
    }

    g_list_free(start_child);
    
    list = list->next;
  }

  g_list_free(start_list);
  
  gtk_spin_button_set_value(AGS_SYNCSYNTH(machine)->loop_start,
			    samplerate * (gtk_spin_button_get_value(AGS_SYNCSYNTH(machine)->loop_start) / old_samplerate));

  gtk_spin_button_set_value(AGS_SYNCSYNTH(machine)->loop_end,
			    samplerate * (gtk_spin_button_get_value(AGS_SYNCSYNTH(machine)->loop_end) / old_samplerate));
}

void
ags_syncsynth_auto_update_callback(GtkToggleButton *toggle, AgsSyncsynth *syncsynth)
{
  if(gtk_toggle_button_get_active(toggle)){
    syncsynth->flags |= AGS_SYNCSYNTH_AUTO_UPDATE;
  }else{
    syncsynth->flags &= (~AGS_SYNCSYNTH_AUTO_UPDATE);
  }
}

void
ags_syncsynth_add_callback(GtkButton *button, AgsSyncsynth *syncsynth)
{
  AgsOscillator *oscillator;

  oscillator = ags_oscillator_new();
  
  ags_syncsynth_add_oscillator(syncsynth,
			       oscillator);
  
  ags_connectable_connect(AGS_CONNECTABLE(oscillator));
  
  g_signal_connect((GObject *) oscillator, "control-changed",
		   G_CALLBACK(ags_syncsynth_oscillator_control_changed_callback), (gpointer) syncsynth);

}

void
ags_syncsynth_remove_callback(GtkButton *button, AgsSyncsynth *syncsynth)
{
  GList *list, *list_start;
  GList *child_start;

  guint nth;
  
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(syncsynth->oscillator));

  nth = 0;
  
  while(list != NULL){
    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(child_start->data))){
      ags_syncsynth_remove_oscillator(syncsynth,
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
ags_syncsynth_update_callback(GtkButton *button, AgsSyncsynth *syncsynth)
{
  ags_syncsynth_update(syncsynth);
}

void
ags_syncsynth_oscillator_control_changed_callback(AgsOscillator *oscillator,
						  AgsSyncsynth *syncsynth)
{
  ags_syncsynth_reset_loop(syncsynth);

  if((AGS_SYNCSYNTH_AUTO_UPDATE & (syncsynth->flags)) != 0){
    ags_syncsynth_update(syncsynth);
  }
}

void
ags_syncsynth_lower_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth)
{
  //TODO:JK: implement me
}

void
ags_syncsynth_loop_start_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth)
{
  //TODO:JK: implement me
}

void
ags_syncsynth_loop_end_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth)
{
  //TODO:JK: implement me
}
