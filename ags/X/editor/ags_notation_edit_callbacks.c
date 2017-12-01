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

#include <ags/X/editor/ags_notation_edit_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

gboolean
ags_notation_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsNotationEdit *notation_edit)
{
  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);
  
  ags_notation_edit_draw(notation_edit);
  
  return(TRUE);
}

gboolean
ags_notation_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsNotationEdit *notation_edit)
{
  ags_notation_edit_reset_vscrollbar(notation_edit);
  ags_notation_edit_reset_hscrollbar(notation_edit);

  ags_notation_edit_draw(notation_edit);

  return(FALSE);
}

gboolean
ags_notation_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsNotationEdit *notation_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_notation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsNotationEdit *notation_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_notation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsNotationEdit *notation_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_notation_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsNotationEdit *notation_edit)
{
  //TODO:JK: implement me
}

gboolean
ags_notation_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsNotationEdit *notation_edit)
{
  //TODO:JK: implement me
}

void
ags_notation_edit_vscrollbar_value_changed(GtkRange *range, AgsNotationEdit *notation_edit)
{
  /* queue draw */
  gtk_widget_queue_draw(notation_edit->drawing_area);
}

void
ags_notation_edit_hscrollbar_value_changed(GtkRange *range, AgsNotationEdit *notation_edit)
{
  /* queue draw */
  gtk_widget_queue_draw(notation_edit->drawing_area);
}
