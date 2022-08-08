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
  guint j, j_stop;
  
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

  cairo_set_line_width(cr, 0.67);

  for(i = 0; i < 5; i++){
    cairo_move_to(cr,
		  (double) x0, y0 + (gdouble) (i * (height / 4)));
    cairo_line_to(cr,
		  (double) x0 + width, y0 + (gdouble) (i * (height / 4)));
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

    cairo_move_to(cr,
		  x0 + ((logical_rect.width / PANGO_SCALE) / 2.0),
		  y0 + ((height - (logical_rect.height / PANGO_SCALE)) / 2.0));

    offset += logical_rect.width / PANGO_SCALE;

    pango_cairo_show_layout(cr,
			    layout);

    g_object_unref(layout);
  }

  sharp_str = "♯";
  flat_str = "♭";
  
  if(sharp_flats > 0){
    gchar *sharp_flat_str;

    guint newline_count;
    
    sharp_flat_str = NULL;
    
    if(!is_minor){
      sharp_flat_str = sharp_str;
    }else{
      sharp_flat_str = flat_str;
    }

    j_stop = 2;

    if(sharp_flats > 4){
      j_stop = 3;
    }

    if(sharp_flats > 9){
      j_stop = 4;
    }
    
    newline_count = 0;
    
    for(i = 0, j = 0; i < sharp_flats; i++, j++){
      PangoLayout *layout;
      PangoFontDescription *desc;

      PangoRectangle ink_rect, logical_rect;

      if(j == j_stop){
	newline_count++;
	
	j = 0;
      }
      
      layout = pango_cairo_create_layout(cr);
      pango_layout_set_text(layout,
			    sharp_flat_str,
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
		    x0 + offset + AGS_SHEET_EDIT_DEFAULT_SPACING + (j * (logical_rect.width / PANGO_SCALE)) + ((logical_rect.width / PANGO_SCALE) / 2.0),
		    y0 + (height - ((floor(sharp_flats / j_stop) * (AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE + AGS_SHEET_EDIT_DEFAULT_SPACING)) - (newline_count * (AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE + AGS_SHEET_EDIT_DEFAULT_SPACING))) - ((logical_rect.height / PANGO_SCALE) / 2.0)));

      pango_cairo_show_layout(cr,
			      layout);

      g_object_unref(layout);
    }
  }
}

void
ags_sheet_edit_draw_notation(AgsSheetEdit *sheet_edit, cairo_t *cr)
{
  gdouble page_width, page_height;

  if(sheet_edit->page_orientation == GTK_PAGE_ORIENTATION_PORTRAIT){
    page_width = AGS_SHEET_EDIT_DEFAULT_WIDTH;
    page_height = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
  }else{
    page_width = AGS_SHEET_EDIT_DEFAULT_HEIGHT;
    page_height = AGS_SHEET_EDIT_DEFAULT_WIDTH;
  }  

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
				4,
				FALSE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP,
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 5.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);

  ags_sheet_edit_draw_tablature(sheet_edit, cr,
				AGS_SHEET_EDIT_F_CLEF,
				0,
				FALSE,
				0,
				(gdouble) AGS_SHEET_EDIT_DEFAULT_SPACING + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT, (gdouble) AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP + 5.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT + AGS_SHEET_EDIT_DEFAULT_TABLATUR_SPACING,
				page_width - (AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT + AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT), 5.0 * AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT);
}

void
ags_sheet_edit_draw(AgsSheetEdit *sheet_edit, cairo_t *cr)
{
  GtkStyleContext *style_context;
  
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
  
  ags_sheet_edit_draw_notation(sheet_edit, cr);
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
