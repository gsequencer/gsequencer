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

#include <ags/X/editor/ags_sheet_edit_callbacks.h>

#include <ags/X/ags_sheet_editor.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <math.h>

#include <gdk/gdkkeysyms.h>

gboolean
ags_sheet_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsSheetEdit *sheet_edit)
{
  ags_sheet_edit_draw(sheet_edit);

  return(FALSE);
}

gboolean
ags_sheet_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsSheetEdit *sheet_edit)
{
  AgsSheetEditor *sheet_editor;
  AgsSheetToolbar *sheet_toolbar;

  sheet_editor = (AgsSheetEditor *) gtk_widget_get_ancestor(GTK_WIDGET(sheet_edit),
								  AGS_TYPE_SHEET_EDITOR);

  sheet_toolbar = sheet_editor->sheet_toolbar;

  gtk_widget_grab_focus((GtkWidget *) sheet_edit->drawing_area);

  //TODO:JK: implement me
  
  return(TRUE);
}

gboolean
ags_sheet_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsSheetEdit *sheet_edit)
{
  AgsSheetEditor *sheet_editor;
  AgsSheetToolbar *sheet_toolbar;

  sheet_editor = (AgsSheetEditor *) gtk_widget_get_ancestor(GTK_WIDGET(sheet_edit),
								  AGS_TYPE_SHEET_EDITOR);

  sheet_toolbar = sheet_editor->sheet_toolbar;

  //TODO:JK: implement me  

  return(FALSE);
}

gboolean
ags_sheet_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsSheetEdit *sheet_edit)
{
  AgsSheetEditor *sheet_editor;
  AgsSheetToolbar *sheet_toolbar;
  
  sheet_editor = (AgsSheetEditor *) gtk_widget_get_ancestor(GTK_WIDGET(sheet_edit),
								  AGS_TYPE_SHEET_EDITOR);

  sheet_toolbar = sheet_editor->sheet_toolbar;

  //TODO:JK: implement me

  return(FALSE);
}

gboolean
ags_sheet_edit_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsSheetEdit *sheet_edit)
{
  AgsSheetEditor *sheet_editor;
  AgsMachine *machine;

  gboolean retval;
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_KEY_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    retval = FALSE;
  }else{
    retval = TRUE;
  }

  sheet_editor = (AgsSheetEditor *) gtk_widget_get_ancestor(GTK_WIDGET(sheet_edit),
							    AGS_TYPE_SHEET_EDITOR);

  machine = sheet_editor->selected_machine;
  
  if(machine != NULL){
    switch(event->keyval){
    case GDK_KEY_Control_L:
      {
	sheet_edit->key_mask |= AGS_SHEET_EDIT_KEY_L_CONTROL;
      }
      break;
    case GDK_KEY_Control_R:
      {
	sheet_edit->key_mask |= AGS_SHEET_EDIT_KEY_R_CONTROL;
      }
      break;
    case GDK_KEY_Shift_L:
      {
	sheet_edit->key_mask |= AGS_SHEET_EDIT_KEY_L_SHIFT;
      }
      break;
    case GDK_KEY_Shift_R:
      {
	sheet_edit->key_mask |= AGS_SHEET_EDIT_KEY_R_SHIFT;
      }
      break;
    case GDK_KEY_a:
      {
	/* select all notes */
	if((AGS_SHEET_EDIT_KEY_L_CONTROL & (sheet_edit->key_mask)) != 0 || (AGS_SHEET_EDIT_KEY_R_CONTROL & (sheet_edit->key_mask)) != 0){
	  ags_sheet_editor_select_all(sheet_editor);
	}
      }
      break;
    case GDK_KEY_c:
      {
	/* copy notes */
	if((AGS_SHEET_EDIT_KEY_L_CONTROL & (sheet_edit->key_mask)) != 0 || (AGS_SHEET_EDIT_KEY_R_CONTROL & (sheet_edit->key_mask)) != 0){
	  ags_sheet_editor_copy(sheet_editor);
	}
      }
      break;
    case GDK_KEY_v:
      {
	/* paste notes */
	if((AGS_SHEET_EDIT_KEY_L_CONTROL & (sheet_edit->key_mask)) != 0 || (AGS_SHEET_EDIT_KEY_R_CONTROL & (sheet_edit->key_mask)) != 0){
	  ags_sheet_editor_paste(sheet_editor);
	}
      }
      break;
    case GDK_KEY_x:
      {
	/* cut notes */
	if((AGS_SHEET_EDIT_KEY_L_CONTROL & (sheet_edit->key_mask)) != 0 || (AGS_SHEET_EDIT_KEY_R_CONTROL & (sheet_edit->key_mask)) != 0){
	  ags_sheet_editor_cut(sheet_editor);
	}
      }
      break;
    }
  }

  return(retval);
}

gboolean
ags_sheet_edit_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsSheetEdit *sheet_edit)
{
  AgsSheetEditor *sheet_editor;
  AgsSheetToolbar *sheet_toolbar;
  AgsMachine *machine;

  gboolean retval;
  
  sheet_editor = (AgsSheetEditor *) gtk_widget_get_ancestor(GTK_WIDGET(sheet_edit),
								  AGS_TYPE_SHEET_EDITOR);

  sheet_toolbar = sheet_editor->sheet_toolbar;

  machine = sheet_editor->selected_machine;
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_KEY_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    retval = FALSE;
  }else{
    retval = TRUE;
  }

  //TODO:JK: implement me
  
  return(retval);
}
