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
#include <pango/pango.h>

#include <math.h>

void ags_sheet_edit_class_init(AgsSheetEditClass *sheet_edit);
void ags_sheet_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sheet_edit_init(AgsSheetEdit *sheet_edit);
void ags_sheet_edit_finalize(GObject *gobject);

void ags_sheet_edit_connect(AgsConnectable *connectable);
void ags_sheet_edit_disconnect(AgsConnectable *connectable);

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

  sheet_edit->page_orientation = GTK_PAGE_ORIENTATION_PORTRAIT;
  
  sheet_edit->notation_x0 = 0;
  sheet_edit->notation_x1 = 64;

  sheet_edit->utf8_tablature_line = NULL;
  sheet_edit->utf8_tablature_note = NULL;

  sheet_edit->ps_surface = cairo_ps_surface_create(NULL,
						   AGS_SHEET_EDIT_DEFAULT_WIDTH, AGS_SHEET_EDIT_DEFAULT_HEIGHT);
  
  sheet_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_can_focus((GtkWidget *) sheet_edit->drawing_area,
			   TRUE);
  gtk_widget_set_focusable((GtkWidget *) sheet_edit->drawing_area,
			   TRUE);

  gtk_widget_set_halign(sheet_edit->drawing_area,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(sheet_edit->drawing_area,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) sheet_edit->drawing_area,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) sheet_edit->drawing_area,
			 TRUE);

  gtk_grid_attach(GTK_GRID(sheet_edit),
		  (GtkWidget *) sheet_edit->drawing_area,
		  0, 0,
		  1, 1);

  sheet_edit->vscrollbar = NULL;
  sheet_edit->hscrollbar = NULL;

  sheet_edit->tablatur = NULL;
  
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
  
  /* drawing area */
  gtk_drawing_area_set_draw_func(sheet_edit->drawing_area,
				 ags_sheet_edit_draw_callback,
				 sheet_edit,
				 NULL);

  g_signal_connect_after((GObject *) sheet_edit->drawing_area, "resize",
			 G_CALLBACK(ags_sheet_edit_drawing_area_resize_callback), (gpointer) sheet_edit);

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

  /* drawing area */
  gtk_drawing_area_set_draw_func(sheet_edit->drawing_area,
				 NULL,
				 NULL,
				 NULL);
  
  g_object_disconnect((GObject *) sheet_edit->drawing_area,
		      "any_signal::resize",
		      G_CALLBACK(ags_sheet_edit_drawing_area_resize_callback),
		      (gpointer) sheet_edit,
		      NULL);

  //TODO:JK: implement me
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

#if 0
void
ags_sheet_edit_draw_tablature(AgsSheetEdit *sheet_edit, cairo_t *cr,
			      guint clef,
			      guint sharp_flats,
			      gboolean is_minor,
			      gint position,
			      gdouble x0, gdouble y0,
			      gdouble width, gdouble height)
{
  GtkSettings *settings;

  gchar *font_name;
  gchar *clef_str;
  gchar *sharp_str;
  gchar *flat_str;

  gdouble offset;
  guint i;
  
  settings = gtk_settings_get_default();

  font_name = NULL;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       NULL);
  
  offset = 0.0;
  
  cairo_set_source_rgba(cr,
			0.0,
			0.0,
			0.0,
			1.0);

  cairo_set_line_width(cr, 0.866);

  for(i = 0; i < 5; i++){
    cairo_move_to(cr,
		  (double) x0, y0 + (gdouble) (i * (height / 4.0)));
    cairo_line_to(cr,
		  (double) x0 + width, y0 + (gdouble) (i * (height / 4.0)));
    cairo_stroke(cr);
  }

  clef_str = NULL;

  switch(clef){
  case AGS_SHEET_EDIT_G_CLEF:
    {
      clef_str = "𝄞";
    }
    break;
  case AGS_SHEET_EDIT_F_CLEF:
    {
      clef_str = "𝄢";
    }
    break;
  }

  if(clef_str != NULL){
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  clef_str,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    AGS_SHEET_EDIT_DEFAULT_CLEF_FONT_SIZE * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			      &ink_rect,
			      &logical_rect);
    
    //TODO:JK: improve me
    if(clef == AGS_SHEET_EDIT_G_CLEF){
      cairo_move_to(cr,
		    x0 + ((gdouble) logical_rect.width / (gdouble) PANGO_SCALE * 0.5),
		    y0 + (((gdouble) height - ((gdouble) logical_rect.height / (gdouble) PANGO_SCALE)) * 0.5));
    }else{
      cairo_move_to(cr,
		    x0 + ((gdouble) logical_rect.width / (gdouble) PANGO_SCALE * 0.5),
		    y0 + (((gdouble) height - ((gdouble) logical_rect.height / (gdouble) PANGO_SCALE)) * 0.5));
    }
    
    offset += logical_rect.width / PANGO_SCALE;

    pango_cairo_show_layout(cr,
    			    layout);

    g_object_unref(layout);
  }

  sharp_str = "♯";
  flat_str = "♭";

  {
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  sharp_str,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			     &ink_rect,
			     &logical_rect);

    cairo_move_to(cr,
		  x0 + offset + (logical_rect.width / PANGO_SCALE * 0.5),
		  y0 - (logical_rect.height / PANGO_SCALE) + ((gdouble) (3.0 * (height / 4.0))) - 1.0);

    pango_cairo_show_layout(cr,
			    layout);
  }
  
  
  if(sharp_flats > 0){
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;

    if(clef == AGS_SHEET_EDIT_G_CLEF){
      if(!is_minor){
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      sharp_str,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);

	switch(sharp_flats){
	case 7:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (6.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (2.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 6:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (5.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (0.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 5:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (4.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (2.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 4:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (3.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (1.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 3:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (2.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (-0.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 2:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (1.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (1.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 1:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (0.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	}
      }else{
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      flat_str,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);

	switch(sharp_flats){
	case 7:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (6.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (3.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 6:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (5.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (1.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 5:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (4.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (3.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 4:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (3.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (1.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 3:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (2.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (2.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 2:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (1.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (0.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 1:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING +(logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (2.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	}
      }
    }else{
      if(!is_minor){
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      sharp_str,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);

	switch(sharp_flats){
	case 7:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (6.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (3.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 6:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (5.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (1.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 5:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (4.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (3.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 4:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (3.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (2.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 3:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (2.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (0.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 2:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (1.0 * (logical_rect.width / PANGO_SCALE * 0.5)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (2.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 1:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (1.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	}
      }else{
	layout = pango_cairo_create_layout(cr);
	pango_layout_set_text(layout,
			      flat_str,
			      -1);
	desc = pango_font_description_from_string(font_name);
	pango_font_description_set_size(desc,
					AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE * PANGO_SCALE);
	pango_layout_set_font_description(layout,
					  desc);
	pango_font_description_free(desc);    

	pango_layout_get_extents(layout,
				 &ink_rect,
				 &logical_rect);

	switch(sharp_flats){
	case 7:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (6.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (4.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 6:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (5.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (2.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 5:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (4.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (4.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 4:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (3.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (2.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 3:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (2.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (3.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 2:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (1.0 * (logical_rect.width / PANGO_SCALE * 1.0)) + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (1.5 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	case 1:
	  cairo_move_to(cr,
			x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (logical_rect.width / PANGO_SCALE * 0.5),
			y0 + ((gdouble) (3.0 * (height / 4.0))) - ((gdouble) height / 4.0 + logical_rect.height / PANGO_SCALE) - 1.0);

	  pango_cairo_show_layout(cr,
				  layout);
	}
      }      
    }
    
    g_object_unref(layout);
  }
}

void
ags_sheet_edit_draw_notation(AgsSheetEdit *sheet_edit, cairo_t *cr)
{
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *toolbar;
  AgsMachine *selected_machine;

  AgsNotebook *notebook;

  GtkSettings *settings;
  
  AgsTimestamp *timestamp;
  AgsTimestamp *current_timestamp;    

  AgsApplicationContext *application_context;

  GList *start_list_notation, *list_notation;

  gchar *font_name;
  
  gdouble page_width, page_height;
  gdouble offset;
  guint x0, x1;
  guint prev_note_x0, prev_note_x1;
  guint audio_channels;
  guint input_pads;
  guint midi_start_mapping;
  guint audio_channel;
  guint i;
  guint offset_counter;
  gboolean is_next_rest;
  
  const gchar* note_strv[] = {
    "𝅝",
    "𝅗𝅥",
    "𝅘𝅥",
    "𝅘𝅥𝅮",
    "𝅘𝅥𝅯",
    NULL,
  };
  
  application_context = ags_application_context_get_instance();

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       NULL);

  /*  */
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) sheet_edit,
								    AGS_TYPE_COMPOSITE_EDITOR);
    
  notebook = composite_editor->sheet_edit->channel_selector;
    
  toolbar = composite_editor->toolbar;

  selected_machine = composite_editor->selected_machine;

  if(sheet_edit->page_orientation == GTK_PAGE_ORIENTATION_PORTRAIT){
    page_width = AGS_SHEET_EDIT_DEFAULT_WIDTH;
    page_height = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
  }else{
    page_width = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
    page_height = AGS_SHEET_EDIT_DEFAULT_WIDTH;
  }  

  offset = 0.0;  

  /* clear with white color */
  cairo_set_source_rgba(cr,
			1.0,
			1.0,
			1.0,
			1.0);
  cairo_rectangle(cr,
		  (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING, (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING,
		  (double) page_width, (double) page_height);
  cairo_fill(cr);

  ags_sheet_edit_draw_tablature(sheet_edit, cr,
				AGS_SHEET_EDIT_G_CLEF,
				7,
				TRUE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP,
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);

  ags_sheet_edit_draw_tablature(sheet_edit, cr,
				AGS_SHEET_EDIT_G_CLEF,
				7,
				FALSE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT + AGS_SHEET_EDIT_DEFAULT_TABLATUR_SPACING,
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);


  ags_sheet_edit_draw_tablature(sheet_edit, cr,
				AGS_SHEET_EDIT_F_CLEF,
				7,
				TRUE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + 2.0 * (4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT + AGS_SHEET_EDIT_DEFAULT_TABLATUR_SPACING),
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);

  ags_sheet_edit_draw_tablature(sheet_edit, cr,
				AGS_SHEET_EDIT_F_CLEF,
				7,
				FALSE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + 3.0 * (4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT + AGS_SHEET_EDIT_DEFAULT_TABLATUR_SPACING),
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 4.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);

  /* test */
  x0 = 0;

  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  start_list_notation = ags_audio_get_notation(selected_machine->audio);

  audio_channels = ags_audio_get_audio_channels(selected_machine->audio);

  input_pads = ags_audio_get_input_pads(selected_machine->audio);

  midi_start_mapping = ags_audio_get_midi_start_mapping(selected_machine->audio);
  
  timestamp->timer.ags_offset.offset = (guint64) AGS_NOTATION_DEFAULT_OFFSET * floor((double) x0 / (double) AGS_NOTATION_DEFAULT_OFFSET);

  offset = 100.0;

  offset_counter = 0;

  prev_note_x0 = 0;
  prev_note_x1 = 0;

  is_next_rest = FALSE;
  
  for(i = 0; i < audio_channels; i++){
    list_notation = ags_notation_find_near_timestamp(start_list_notation, i,
						     timestamp);
    
    while(list_notation != NULL){
      AgsNotation *notation;

      GList *start_list_note, *list_note;

      notation = AGS_NOTATION(list_notation->data);

      g_object_get(notation,
		   "audio-channel", &audio_channel,
		   "timestamp", &current_timestamp,
		   NULL);

      if(ags_timestamp_get_ags_offset(current_timestamp) > AGS_NOTATION_DEFAULT_OFFSET * floor((double) x1 / (double) AGS_NOTATION_DEFAULT_OFFSET) + AGS_NOTATION_DEFAULT_OFFSET){
	break;
      }

      if(ags_timestamp_get_ags_offset(current_timestamp) + AGS_NOTATION_DEFAULT_OFFSET < x0){
	list_notation = list_notation->next;

	continue;
      }
      
      if(i != audio_channel){
	list_notation = list_notation->next;

	continue;
      }

      start_list_note = NULL;

      g_object_get(notation,
		   "note", &start_list_note,
		   NULL);
      
      list_note = start_list_note;

      while(list_note != NULL){
	AgsNote *note, *note_next;

	PangoLayout *layout;
	PangoFontDescription *desc;
	
	PangoRectangle ink_rect, logical_rect;

	GList *list;

	gchar *note_str;
	gchar *rest_str;
	
	guint note_x0, note_x1;
	guint note_y;
	guint note_next_x0, note_next_x1;
	guint note_next_pos_x0, note_next_pos_x1;
	gboolean success;
	
	note = AGS_NOTE(list_note->data);

	note_next = NULL;

	if(list_note->next != NULL){
	  note_next = AGS_NOTE(list_note->next->data);
	}
	
	note_x0 = 0;
	note_x1 = 0;

	note_y = 0;
	
	g_object_get(note,
		     "x0", &note_x0,
		     "x1", &note_x1,
		     "y", &note_y,
		     NULL);
	
	note_next_x0 = 0;
	note_next_x1 = 0;

	if(note_next != NULL){
	  g_object_get(note_next,
		       "x0", &note_next_x0,
		       "x1", &note_next_x1,
		       NULL);
	}

	note_next_pos_x0 = 0;
	note_next_pos_x1 = 0;

	list = list_note->next;
		    
	while(list != NULL){
	  note_next_pos_x0 = 0;
	  note_next_pos_x1 = 0;
		      
	  g_object_get(list->data,
		       "x0", &note_next_pos_x0,
		       "x1", &note_next_pos_x1,
		       NULL);

	  if(note_next_pos_x0 > note_x0){
	    break;
	  }
		      
	  list = list->next;
	}

	if(list == NULL){
	  note_next_pos_x0 = 0;
	  note_next_pos_x1 = 0;
	}
	
	note_str = NULL;
	rest_str = NULL;

	success = FALSE;

	if(!is_next_rest){
	  if((note_next_pos_x0 != note_next_pos_x1 &&
	      note_next_pos_x0 - note_x0 <= 16)){
	    if(note_x1 <= (guint) (16.0 * floor(note_x0 / 16.0) + 16.0)){
	      if((gint) log2((gdouble) (note_next_pos_x0 - note_x0)) >= 0 &&
		 (gint) log2((gdouble) (note_next_pos_x0 - note_x0)) < 5){
		success = TRUE;
	  
		note_str = note_strv[4 - (gint) log2((gdouble) (note_next_pos_x0 - note_x0))];

		if(note_x0 != note_next_x0){
		  offset_counter += (note_next_pos_x0 - note_x0);
		}
	      }
	    }else{
	    }
	  }else if((note_next_pos_x0 != note_next_pos_x1 &&
		    note_next_pos_x0 - note_x0 > 16)){
	    if(note_x1 <= (guint) (16.0 * floor(note_x0 / 16.0) + 16.0)){
	      if((gint) log2((gdouble) ((guint) (16.0 * floor(note_x0 / 16.0) + 16.0) - note_x0)) >= 0 &&
		 (gint) log2((gdouble) ((guint) (16.0 * floor(note_x0 / 16.0) + 16.0) - note_x0)) < 5){
		success = TRUE;
		is_next_rest = TRUE;
	  
		note_str = note_strv[4 - (gint) log2((gdouble) ((guint) (16.0 * floor(note_x0 / 16.0) + 16.0) - note_x0))];

		if(note_x0 != note_next_x0){
		  offset_counter += ((guint) (16.0 * floor(note_x0 / 16.0) + 16.0) - note_x0);
		}
	      }
	    }else{
	    }	  
	  }else if(note_next_pos_x0 == note_next_pos_x1 &&
		   note_x1 - note_x0 <= 16){
	    if(note_x1 <= (guint) (16.0 * floor(note_x0 / 16.0) + 16.0)){
	      if((gint) log2((gdouble) (note_x1 - note_x0)) >= 0 &&
		 (gint) log2((gdouble) (note_x1 - note_x0)) < 5){
		success = TRUE;
	  
		note_str = note_strv[4 - (gint) log2((gdouble) (note_x1 - note_x0))];

		offset_counter += (note_x1 - note_x0);
	      }
	    }else{
	    }
	  }
	}else{
	  is_next_rest = FALSE;
	}
	
	if(!success){
	  if(prev_note_x0 != prev_note_x1){
	    if(offset_counter % 16 == 0 && offset_counter + 16 <= note_x0){
	      rest_str = "𝄻";

	      offset_counter += 16;
	    }else if(offset_counter % 8 == 0 && offset_counter + 8 <= note_x0 && prev_note_x0 + 8 <= offset_counter){
	      rest_str = "𝄼";

	      offset_counter += 8;
	    }else if(offset_counter % 4 == 0 && offset_counter + 4 <= note_x0 && prev_note_x0 + 4 <= offset_counter){
	      rest_str = "𝄽";

	      offset_counter += 4;
	    }else if(offset_counter % 2 == 0 && offset_counter + 2 <= note_x0 && prev_note_x0 + 2 <= offset_counter){
	      rest_str = "𝄾";

	      offset_counter += 2;
	    }else if(offset_counter + 1 == note_x0 && prev_note_x0 + 1 == offset_counter){
	      rest_str = "𝄿";

	      offset_counter += 1;
	    }
	  }else{
	    if(offset_counter % 16 == 0 && offset_counter + 16 <= note_x0){
	      rest_str = "𝄻";

	      offset_counter += 16;
	    }else if(offset_counter % 8 == 0 && offset_counter + 8 <= note_x0){
	      rest_str = "𝄼";

	      offset_counter += 8;
	    }else if(offset_counter % 4 == 0 && offset_counter + 4 <= note_x0){
	      rest_str = "𝄽";

	      offset_counter += 4;
	    }else if(offset_counter % 2 == 0 && offset_counter + 2 <= note_x0){
	      rest_str = "𝄾";

	      offset_counter += 2;
	    }else if(offset_counter + 1 == note_x0){
	      rest_str = "𝄿";

	      offset_counter += 1;
	    }
	  }
	}
	
	if(note_str != NULL){
	  layout = pango_cairo_create_layout(cr);
	  pango_layout_set_text(layout,
				note_str,
				-1);
	  desc = pango_font_description_from_string(font_name);
	  pango_font_description_set_size(desc,
					  AGS_SHEET_EDIT_DEFAULT_KEY_FONT_SIZE * PANGO_SCALE);
	  pango_layout_set_font_description(layout,
					    desc);
	  pango_font_description_free(desc);    

	  pango_layout_get_extents(layout,
				   &ink_rect,
				   &logical_rect);

	  //TODO:JK: improve me
	  cairo_move_to(cr,
			(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset,
			(gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + (((gdouble) note_y - 62.0) * ((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT / 2.0)) - (((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT * 4.0) / 8.0) - 1.0);
	  
	  pango_cairo_show_layout(cr,
				  layout);
    
	  if(note_x0 < note_next_x0 ||
	     note_next_x0 == note_next_x1){
	    offset += 2.5 * (logical_rect.width / PANGO_SCALE);

	    if(offset_counter % 16 == 0){
	      cairo_move_to(cr,
			    (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP);
	      cairo_line_to(cr,
			    (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + ((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT * 4.0));
	      cairo_stroke(cr);	      

	      offset += 2.5 * (logical_rect.width / PANGO_SCALE);
	    }
	  }
	    
	  g_object_unref(layout);
	}

	if(rest_str != NULL){
	  layout = pango_cairo_create_layout(cr);
	  pango_layout_set_text(layout,
				rest_str,
				-1);
	  desc = pango_font_description_from_string(font_name);
	  pango_font_description_set_size(desc,
					  AGS_SHEET_EDIT_DEFAULT_REST_FONT_SIZE * PANGO_SCALE);
	  pango_layout_set_font_description(layout,
					    desc);
	  pango_font_description_free(desc);    

	  pango_layout_get_extents(layout,
				   &ink_rect,
				   &logical_rect);

	  //TODO:JK: improve me
	  cairo_move_to(cr,
			(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset,
			(gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + (1.0 * ((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT / 2.0)) - (((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT * 4.0) / 8.0) - 1.0);
	  
	  pango_cairo_show_layout(cr,
				  layout);
    
	  offset += 2.5 * (logical_rect.width / PANGO_SCALE);

	  if(offset_counter % 16 == 0){	      
	    cairo_move_to(cr,
			  (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP);
	    cairo_line_to(cr,
			  (gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + offset, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + ((gdouble) AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT * 4.0));
	    cairo_stroke(cr);	      

	    offset += 2.5 * (logical_rect.width / PANGO_SCALE);
	  }
	    
	  g_object_unref(layout);
	}
	
	if(success){
	  list_note = list_note->next;
	}
      }

      g_list_free_full(start_list_note,
		       g_object_unref);
      
      list_notation = list_notation->next;
    }
  }
}
#endif

AgsSheetEditTablatur*
ags_sheet_edit_tablatur_alloc()
{
  AgsSheetEditTablatur *sheet_edit_tablatur;

  sheet_edit_tablatur = (AgsSheetEditTablatur *) g_new0(AgsSheetEditTablatur,
							1);

  sheet_edit_tablatur->is_primary = FALSE;

  sheet_edit_tablatur->notation_x_start = 0;
  sheet_edit_tablatur->notation_x_end = 16;
  
  sheet_edit_tablatur->font_size = AGS_SHEET_EDIT_DEFAULT_FONT_SIZE;

  sheet_edit_tablatur->margin_top = 0.0;
  sheet_edit_tablatur->margin_bottom = 0.0;
  sheet_edit_tablatur->margin_left = 0.0;
  sheet_edit_tablatur->margin_right = 0.0;

  sheet_edit_tablatur->clef = AGS_SHEET_EDIT_G_CLEF;

  sheet_edit_tablatur->clef_translate_x = 0.0;
  sheet_edit_tablatur->clef_translate_y = 0.0;
  sheet_edit_tablatur->clef_translate_z = 0.0;

  sheet_edit_tablatur->clef_font_size = AGS_SHEET_EDIT_DEFAULT_CLEF_FONT_SIZE;
  
  sheet_edit_tablatur->tablatur_extends_top = 0;
  sheet_edit_tablatur->tablatur_extends_bottom = 0;

  sheet_edit_tablatur->tablatur_x0 = 0.0;
  sheet_edit_tablatur->tablatur_y0 = 0.0;

  sheet_edit_tablatur->tablatur_width = AGS_SHEET_EDIT_DEFAULT_WIDTH - (sheet_edit_tablatur->margin_left + sheet_edit_tablatur->margin_right);
  sheet_edit_tablatur->tablatur_height = ((4.0 + (gdouble) sheet_edit_tablatur->tablatur_extends_top + (gdouble) sheet_edit_tablatur->tablatur_extends_bottom) * sheet_edit_tablatur->font_size);

  sheet_edit_tablatur->sharp_translate_x = 0.0;
  sheet_edit_tablatur->sharp_translate_y = 0.0;
  sheet_edit_tablatur->sharp_translate_z = 0.0;

  sheet_edit_tablatur->flat_translate_x = 0.0;
  sheet_edit_tablatur->flat_translate_y = 0.0;
  sheet_edit_tablatur->flat_translate_z = 0.0;

  sheet_edit_tablatur->sharp_flat_font_size = AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE;

  sheet_edit_tablatur->key_translate_x = 0.0;
  sheet_edit_tablatur->key_translate_y = 0.0;
  sheet_edit_tablatur->key_translate_z = 0.0;

  sheet_edit_tablatur->key_font_size = AGS_SHEET_EDIT_DEFAULT_KEY_FONT_SIZE;

  sheet_edit_tablatur->rest_translate_x = 0.0;
  sheet_edit_tablatur->rest_translate_y = 0.0;
  sheet_edit_tablatur->rest_translate_z = 0.0;

  sheet_edit_tablatur->rest_font_size = AGS_SHEET_EDIT_DEFAULT_REST_FONT_SIZE;
  
  sheet_edit_tablatur->companion_tablatur = NULL;
  
  return(sheet_edit_tablatur);
}

void
ags_sheet_edit_tablatur_free(AgsSheetEditTablatur *sheet_edit_tablatur)
{
  g_return_if_fail(sheet_edit_tablatur != NULL);

  g_free(sheet_edit_tablatur);
}

GList*
ags_sheet_edit_get_tablatur(AgsSheetEdit *sheet_edit)
{
  g_return_val_if_fail(AGS_IS_SHEET_EDIT(sheet_edit), NULL);

  return(g_list_reverse(g_list_copy(sheet_edit->tablatur)));
}

void
ags_sheet_edit_add_tablatur(AgsSheetEdit *sheet_edit,
			    AgsSheetEditTablatur *sheet_edit_tablatur)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_tablatur != NULL);

  if(g_list_find(sheet_edit->tablatur, sheet_edit_tablatur) == NULL){
    sheet_edit->tablatur = g_list_prepend(sheet_edit->tablatur,
					  sheet_edit_tablatur);
  }
}

void
ags_sheet_edit_remove_tablatur(AgsSheetEdit *sheet_edit,
			       AgsSheetEditTablatur *sheet_edit_tablatur)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_tablatur != NULL);

  if(g_list_find(sheet_edit->tablatur, sheet_edit_tablatur) != NULL){
    sheet_edit->tablatur = g_list_remove(sheet_edit->tablatur,
					 sheet_edit_tablatur);
  }
}

void
ags_sheet_edit_draw_tablature(AgsSheetEdit *sheet_edit, cairo_t *cr,
			      AgsSheetEditTablatur *sheet_edit_tablatur,
			      gdouble x0, gdouble y0,
			      gdouble width, gdouble height,
			      gdouble font_size)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_tablatur != NULL);

  //TODO:JK: implement me
}

void
ags_sheet_edit_draw_clef(AgsSheetEdit *sheet_edit, cairo_t *cr,
			 AgsSheetEditTablatur *sheet_edit_tablatur,
			 guint clef,
			 gdouble x0, gdouble y0,
			 gdouble clef_font_size)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_tablatur != NULL);

  //TODO:JK: implement me
}

void
ags_sheet_edit_draw_sharp_flat(AgsSheetEdit *sheet_edit, cairo_t *cr,
			       AgsSheetEditTablatur *sheet_edit_tablatur,
			       guint sharp_flat,
			       gboolean is_minor,
			       gdouble x0, gdouble y0,
			       gdouble sharp_flat_font_size)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_tablatur != NULL);

  //TODO:JK: implement me
}

void
ags_sheet_edit_draw_note(AgsSheetEdit *sheet_edit, cairo_t *cr,
			 AgsSheetEditTablatur *sheet_edit_tablatur,
			 AgsNotation *notation,
			 AgsNote *note,
			 gdouble x0, gdouble y0,
			 gdouble key_font_size)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_tablatur != NULL);

  //TODO:JK: implement me
}

void
ags_sheet_edit_draw_rest(AgsSheetEdit *sheet_edit, cairo_t *cr,
			 AgsSheetEditTablatur *sheet_edit_tablatur,
			 guint rest_x0, guint rest_x1,
			 gdouble x0, gdouble y0,
			 gdouble rest_font_size)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_tablatur != NULL);

  //TODO:JK: implement me
}

void
ags_sheet_edit_draw_notation(AgsSheetEdit *sheet_edit, cairo_t *cr,
			     AgsSheetEditTablatur *sheet_edit_tablatur,
			     AgsNotation *notation,
			     guint notation_x0, guint notation_x1,
			     gdouble x0, gdouble y0,
			     gdouble key_font_size)
{
  g_return_if_fail(AGS_IS_SHEET_EDIT(sheet_edit));
  g_return_if_fail(sheet_edit_tablatur != NULL);

  //TODO:JK: implement me
}

void
ags_sheet_edit_draw(AgsSheetEdit *sheet_edit, cairo_t *cr)
{
  GtkStyleContext *style_context;

  GList *tablatur;
  
  gint width, height;
  gdouble page_width, page_height;

  style_context = gtk_widget_get_style_context((GtkWidget *) sheet_edit);

  width = gtk_widget_get_width(sheet_edit);
  height = gtk_widget_get_height(sheet_edit);

  if(sheet_edit->page_orientation == GTK_PAGE_ORIENTATION_PORTRAIT){
    page_width = AGS_SHEET_EDIT_DEFAULT_WIDTH;
    page_height = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
  }else{
    page_width = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
    page_height = AGS_SHEET_EDIT_DEFAULT_WIDTH;
  }

  gtk_render_background(style_context,
			cr,
			0.0, 0.0,
			(gdouble) width, (gdouble) height);
  
  //  ags_sheet_edit_draw_notation(sheet_edit, cr);
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
