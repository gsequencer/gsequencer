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

#include <ags/X/machine/ags_equalizer10_callbacks.h>

#include <ags/X/ags_window.h>

void
ags_equalizer10_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsEqualizer10 *equalizer10)
{
  AgsWindow *window;

  gchar *str;
  
  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) equalizer10, AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_EQUALIZER10)->counter);

  g_object_set(AGS_MACHINE(equalizer10),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_EQUALIZER10);
  g_free(str);
}

void
ags_equalizer10_peak_28hz_callback(GtkRange *range,
				   AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_28hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_28hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_56hz_callback(GtkRange *range,
				   AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_56hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_56hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_112hz_callback(GtkRange *range,
				    AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_112hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_112hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_224hz_callback(GtkRange *range,
				    AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_224hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_224hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_448hz_callback(GtkRange *range,
				    AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_448hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_448hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_896hz_callback(GtkRange *range,
				    AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_896hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_896hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_1792hz_callback(GtkRange *range,
				     AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_1792hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_1792hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_3584hz_callback(GtkRange *range,
				     AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_3584hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_3584hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_7168hz_callback(GtkRange *range,
				     AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_7168hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_7168hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_14336hz_callback(GtkRange *range,
				      AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_14336hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_14336hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_pressure_callback(GtkRange *range,
				  AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->pressure_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->pressure_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}
