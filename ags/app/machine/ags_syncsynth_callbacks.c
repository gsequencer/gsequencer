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

#include <ags/app/machine/ags_syncsynth_callbacks.h>
#include <ags/app/machine/ags_oscillator.h>

#include <ags/app/ags_window.h>

#include <math.h>

void
ags_syncsynth_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsSyncsynth *syncsynth)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_ancestor(widget,
						 AGS_TYPE_WINDOW);

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
    start_list = ags_syncsynth_get_oscillator(AGS_SYNCSYNTH(machine));

  while(list != NULL){
    AgsOscillator *oscillator;
    
    guint i;

    oscillator = list->data;
  
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
  GList *start_list, *list;
  
  list =
    start_list = ags_syncsynth_get_oscillator(syncsynth);

  while(list != NULL){
    if(gtk_check_button_get_active(AGS_OSCILLATOR(list->data)->selector)){
      ags_syncsynth_remove_oscillator(syncsynth,
				      list->data);
    }
    
    list = list->next;
  }

  g_list_free(start_list);
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

  if(ags_syncsynth_test_flags(syncsynth, AGS_SYNCSYNTH_AUTO_UPDATE)){
    ags_syncsynth_update(syncsynth);
  }
}

void
ags_syncsynth_lower_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth)
{
  if(ags_syncsynth_test_flags(syncsynth, AGS_SYNCSYNTH_AUTO_UPDATE)){
    ags_syncsynth_update(syncsynth);
  }
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

void
ags_syncsynth_volume_callback(GtkRange *range, AgsSyncsynth *syncsynth)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(syncsynth)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_VOLUME_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "volume", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  volume);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}
