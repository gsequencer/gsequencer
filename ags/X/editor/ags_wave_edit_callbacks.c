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

#include <ags/X/editor/ags_wave_edit_callbacks.h>

#include <ags/X/ags_wave_editor.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <math.h>
#include <gdk/gdkkeysyms.h>

gboolean
ags_wave_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsWaveEdit *wave_edit)
{
  ags_wave_edit_reset_vscrollbar(wave_edit);
  ags_wave_edit_reset_hscrollbar(wave_edit);
  
  ags_wave_edit_draw(wave_edit);
  
  return(TRUE);
}

gboolean
ags_wave_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsWaveEdit *wave_edit)
{
  ags_wave_edit_reset_vscrollbar(wave_edit);
  ags_wave_edit_reset_hscrollbar(wave_edit);
  
  ags_wave_edit_draw(wave_edit);
  
  return(TRUE);
}

gboolean
ags_wave_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsWaveEdit *wave_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_wave_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsWaveEdit *wave_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_wave_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsWaveEdit *wave_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_wave_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsWaveEdit *wave_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_wave_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsWaveEdit *wave_edit)
{
  //TODO:JK: implement me
}

void
ags_wave_edit_vscrollbar_value_changed(GtkRange *range, AgsWaveEdit *wave_edit)
{
  AgsWaveEditor *wave_editor;

  wave_editor = gtk_widget_get_ancestor(wave_edit,
					AGS_TYPE_WAVE_EDITOR);

  /* queue draw */
  gtk_widget_queue_draw(wave_edit->drawing_area);
}

void
ags_wave_edit_hscrollbar_value_changed(GtkRange *range, AgsWaveEdit *wave_edit)
{
  gdouble value;

  value = GTK_RANGE(wave_edit->hscrollbar)->adjustment->value / 64.0;
  gtk_adjustment_set_value(wave_edit->ruler->adjustment,
			   value);
  gtk_widget_queue_draw(wave_edit->ruler);
  
  /* queue draw */
  gtk_widget_queue_draw(wave_edit->drawing_area);
}

