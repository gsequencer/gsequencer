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

#include <ags/app/editor/ags_sheet_edit.h>
#include <ags/app/editor/ags_sheet_edit_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_composite_editor.h>

#include <gdk/gdkkeysyms.h>
#include <cairo.h>
#include <cairo-ps.h>

#include <math.h>

void ags_sheet_edit_class_init(AgsSheetEditClass *sheet_edit);
void ags_sheet_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sheet_edit_init(AgsSheetEdit *sheet_edit);
void ags_sheet_edit_finalize(GObject *gobject);

void ags_sheet_edit_connect(AgsConnectable *connectable);
void ags_sheet_edit_disconnect(AgsConnectable *connectable);

void ags_sheet_edit_show(GtkWidget *widget);

gboolean ags_sheet_edit_auto_scroll_timeout(GtkWidget *widget);

/**
 * SECTION:ags_sheet_edit
 * @short_description: edit notes
 * @title: AgsSheetEdit
 * @section_id:
 * @include: ags/app/editor/ags_sheet_edit.h
 *
 * The #AgsSheetEdit lets you edit notes.
 */

enum{
  PROP_0,
};

static gpointer ags_sheet_edit_parent_class = NULL;

GHashTable *ags_sheet_edit_auto_scroll = NULL;

GType
ags_sheet_edit_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sheet_edit = 0;

    static const GTypeInfo ags_sheet_edit_info = {
      sizeof (AgsSheetEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sheet_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSheetEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sheet_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sheet_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sheet_edit = g_type_register_static(GTK_TYPE_GRID,
						 "AgsSheetEdit", &ags_sheet_edit_info,
						 0);
    
    g_type_add_interface_static(ags_type_sheet_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sheet_edit);
  }

  return g_define_type_id__volatile;
}

void
ags_sheet_edit_class_init(AgsSheetEditClass *sheet_edit)
{
  GtkWidgetClass *widget;
  
  GObjectClass *gobject;
  
  ags_sheet_edit_parent_class = g_type_class_peek_parent(sheet_edit);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(sheet_edit);

  gobject->finalize = ags_sheet_edit_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) sheet_edit;

  widget->show = ags_sheet_edit_show;
}

void
ags_sheet_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_sheet_edit_connect;
  connectable->disconnect = ags_sheet_edit_disconnect;
}

void
ags_sheet_edit_init(AgsSheetEdit *sheet_edit)
{
  GtkAdjustment *adjustment;

  sheet_edit->flags = 0;
  sheet_edit->connectable_flags = 0;
  sheet_edit->mode = AGS_SHEET_EDIT_NO_EDIT_MODE;

  sheet_edit->button_mask = 0;
  sheet_edit->key_mask = 0;

  sheet_edit->note_offset = 0;
  sheet_edit->note_offset_absolute = 0;

  sheet_edit->cursor_position_x = AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_X;
  sheet_edit->cursor_position_y = AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_Y;

  sheet_edit->selection_x0 = 0;
  sheet_edit->selection_x1 = 0;
  sheet_edit->selection_y0 = 0;
  sheet_edit->selection_y1 = 0;

  sheet_edit->paper_name = g_strdup(AGS_SHEET_EDIT_DEFAULT_PAPER_NAME);

  sheet_edit->notation_x0 = 0;
  sheet_edit->notation_x1 = 0;

  sheet_edit->utf8_tablature_line = NULL;
  sheet_edit->utf8_tablature_note = NULL;

  sheet_edit->ps_surface = cairo_ps_surface_create(NULL,
						   AGS_SHEET_EDIT_DEFAULT_WIDTH, AGS_SHEET_EDIT_DEFAULT_HEIGHT);
  
  sheet_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_can_focus((GtkWidget *) sheet_edit->drawing_area,
			   TRUE);
  gtk_grid_attach(GTK_GRID(sheet_edit),
		  (GtkWidget *) sheet_edit->drawing_area,
		  0, 0,
		  1, 1);
    
  /* auto-scroll */
  if(ags_sheet_edit_auto_scroll == NULL){
    ags_sheet_edit_auto_scroll = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							  NULL,
							  NULL);
  }

  g_hash_table_insert(ags_sheet_edit_auto_scroll,
		      sheet_edit, ags_sheet_edit_auto_scroll_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_sheet_edit_auto_scroll_timeout, (gpointer) sheet_edit);
}

void
ags_sheet_edit_finalize(GObject *gobject)
{
  AgsSheetEdit *sheet_edit;
  
  sheet_edit = AGS_SHEET_EDIT(gobject);

  g_free(sheet_edit->utf8_tablature_line);
  g_free(sheet_edit->utf8_tablature_note);

  cairo_surface_destroy(sheet_edit->ps_surface);
  
  /* remove auto scroll */
  g_hash_table_remove(ags_sheet_edit_auto_scroll,
		      sheet_edit);

  /* call parent */
  G_OBJECT_CLASS(ags_sheet_edit_parent_class)->finalize(gobject);
}

void
ags_sheet_edit_connect(AgsConnectable *connectable)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (sheet_edit->connectable_flags)) != 0){
    return;
  }

  sheet_edit->connectable_flags |= AGS_CONNECTABLE_CONNECTED;  

  //TODO:JK: implement me
}

void
ags_sheet_edit_disconnect(AgsConnectable *connectable)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (sheet_edit->connectable_flags)) == 0){
    return;
  }

  sheet_edit->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  //TODO:JK: implement me
}

void
ags_sheet_edit_show(GtkWidget *widget)
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = AGS_SHEET_EDIT(widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_sheet_edit_parent_class)->show(widget);
}

gboolean
ags_sheet_edit_auto_scroll_timeout(GtkWidget *widget)
{
  if(g_hash_table_lookup(ags_sheet_edit_auto_scroll,
			 widget) != NULL){
    AgsCompositeEditor *composite_editor;
    AgsSheetEdit *sheet_edit;

    GObject *output_soundcard;
    
    double x;
    
    sheet_edit = AGS_SHEET_EDIT(widget);

    if((AGS_SHEET_EDIT_AUTO_SCROLL & (sheet_edit->flags)) == 0){
      return(TRUE);
    }
    
    composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) sheet_edit,
								      AGS_TYPE_COMPOSITE_EDITOR);
    
    if(composite_editor->selected_machine == NULL){
      return(TRUE);
    }

    //TODO:JK: implement me
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_sheet_edit_new:
 *
 * Create a new #AgsSheetEdit.
 *
 * Returns: a new #AgsSheetEdit
 * 
 * Since: 3.0.0
 */
AgsSheetEdit*
ags_sheet_edit_new()
{
  AgsSheetEdit *sheet_edit;

  sheet_edit = (AgsSheetEdit *) g_object_new(AGS_TYPE_SHEET_EDIT,
					     NULL);

  return(sheet_edit);
}
