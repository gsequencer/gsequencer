/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include "ags_file_entry.h"

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <glib/gstdio.h>

#include <gdk/gdkkeysyms.h>

#include <ags/i18n.h>

void ags_file_entry_class_init(AgsFileEntryClass *file_entry);
void ags_file_entry_orientable_interface_init(GtkOrientableIface *orientable);
void ags_file_entry_init(AgsFileEntry *file_entry);
void ags_file_entry_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_file_entry_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_file_entry_dispose(GObject *gobject);
void ags_file_entry_finalize(GObject *gobject);

void ags_file_entry_draw_callback(GtkWidget *drawing_area,
				  cairo_t *cr,
				  int width, int height,
				  AgsFileEntry *file_entry);

void ags_file_entry_real_activate(AgsFileEntry *file_entry);

/**
 * SECTION:ags_file_entry
 * @short_description: a file entry widget
 * @title: AgsFileEntry
 * @section_id:
 * @include: ags/widget/ags_file_entry.h
 *
 * #AgsFileEntry is a entry widget to open or save files.
 */

enum{
  ACTIVATE,
  LAST_SIGNAL,
};

enum{
  PROP_0, 
  PROP_FILENAME,
};

static gpointer ags_file_entry_parent_class = NULL;
static guint file_entry_signals[LAST_SIGNAL];

GType
ags_file_entry_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_file_entry = 0;

    static const GTypeInfo ags_file_entry_info = {
      sizeof(AgsFileEntryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_entry_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFileEntry),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_entry_init,
    };
    
    ags_type_file_entry = g_type_register_static(GTK_TYPE_BOX,
						 "AgsFileEntry", &ags_file_entry_info,
						 0);    
    
    g_once_init_leave(&g_define_type_id__static, ags_type_file_entry);
  }

  return(g_define_type_id__static);
}

void
ags_file_entry_class_init(AgsFileEntryClass *file_entry)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_file_entry_parent_class = g_type_class_peek_parent(file_entry);

  /* GObjectClass */
  gobject = (GObjectClass *) file_entry;

  gobject->set_property = ags_file_entry_set_property;
  gobject->get_property = ags_file_entry_get_property;

  gobject->dispose = ags_file_entry_dispose;
  gobject->finalize = ags_file_entry_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) file_entry;

  /* properties */
  /**
   * AgsFileEntry:filename:
   *
   * The file widget.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_string("filename",
				   "filename",
				   "The filename",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /* AgsFileEntry */
  file_entry->activate = ags_file_entry_real_activate;
  
  /* signals */
  /**
   * AgsFileEntry::activate:
   * @file_entry: the #AgsFileEntry
   *
   * The ::activate signal notifies about window interaction.
   *
   * Since: 7.5.0
   */
  file_entry_signals[ACTIVATE] =
    g_signal_new("activate",
		 G_TYPE_FROM_CLASS(file_entry),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileEntryClass, activate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_entry_init(AgsFileEntry *file_entry)
{
  static const gchar * const edit_strv[] = {
    "insert",
    "select all",
    "copy",
    "cut",
    "paste",
    NULL
  };
  
  file_entry->flags = 0;

  gtk_box_set_spacing((GtkBox *) file_entry,
		      6);

  file_entry->max_filename_length = AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH;
  
  file_entry->filename = (gchar *) g_malloc(AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH * sizeof(gchar));
  memset(file_entry->filename, 0, AGS_FILE_ENTRY_DEFAULT_MAX_FILENAME_LENGTH * sizeof(gchar));
  
  file_entry->auto_completion_filename = NULL;

  file_entry->font_size = 11;
  file_entry->xalign = 0.0;

  file_entry->selection_offset_x0 = NULL;
  file_entry->selection_offset_x1 = NULL;

  file_entry->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();  

  gtk_widget_set_halign((GtkWidget *) file_entry->drawing_area,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) file_entry->drawing_area,
			 TRUE);  

  gtk_box_append((GtkBox *) file_entry,
		 (GtkWidget *) file_entry->drawing_area);

  gtk_drawing_area_set_draw_func(file_entry->drawing_area,
				 (GtkDrawingAreaDrawFunc) ags_file_entry_draw_callback,
				 file_entry,
				 NULL);
  
  file_entry->edit_drop_down = (GtkDropDown *) gtk_drop_down_new_from_strings(edit_strv);
  gtk_box_append((GtkBox *) file_entry,
		 (GtkWidget *) file_entry->edit_drop_down);
}

void
ags_file_entry_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsFileEntry *file_entry;

  file_entry = AGS_FILE_ENTRY(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(file_entry->filename == filename){
	return;
      }

      snprintf(file_entry->filename, file_entry->max_filename_length, "%s", filename);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_entry_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsFileEntry *file_entry;

  file_entry = AGS_FILE_ENTRY(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      if(file_entry->filename == NULL ||
	 file_entry->filename[0] == '\0'){
	g_value_set_string(value, NULL);
      }else{
	g_value_set_string(value, file_entry->filename);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_entry_dispose(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_file_entry_parent_class)->dispose(gobject);
}

void
ags_file_entry_finalize(GObject *gobject)
{
  AgsFileEntry *file_entry;
  
  file_entry = AGS_FILE_ENTRY(gobject);

  g_free(file_entry->filename);
  g_free(file_entry->auto_completion_filename);
  
  /* call parent */
  G_OBJECT_CLASS(ags_file_entry_parent_class)->finalize(gobject);
}

void
ags_file_entry_draw_callback(GtkWidget *drawing_area,
			     cairo_t *cr,
			     int width, int height,
			     AgsFileEntry *file_entry)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  PangoLayout *layout;
  PangoFontDescription *desc;

  PangoRectangle ink_rect, logical_rect;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;
  GdkRGBA text_color;
    
  gchar *font_name;

  gdouble x_start, y_start;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean text_success;
  gboolean shadow_success;

  style_context = gtk_widget_get_style_context((GtkWidget *) drawing_area);

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);
    
  shadow_success = gtk_style_context_lookup_color(style_context,
						  "theme_shadow_color",
						  &shadow_color);
    
  text_success = gtk_style_context_lookup_color(style_context,
						"theme_text_color",
						&text_color);

  if(!fg_success ||
     !bg_success ||
     !shadow_success ||
     !text_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");

    gdk_rgba_parse(&shadow_color,
		   "#ffffff40");

    gdk_rgba_parse(&text_color,
		   "#1a1a1a");
  }

  cairo_push_group(cr);

  /* background */
  cairo_set_source_rgb(cr,
		       1.0,
		       1.0,
		       1.0);
  cairo_rectangle(cr,
		  (gdouble) 0.0, (gdouble) 0.0,
		  (gdouble) width, (gdouble) height);
  cairo_fill(cr);

  /* pango */
  layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout,
			file_entry->filename,
			-1);
  desc = pango_font_description_from_string(font_name);
  pango_font_description_set_size(desc,
				  file_entry->font_size * PANGO_SCALE);
  pango_layout_set_font_description(layout,
				    desc);
  pango_font_description_free(desc);    

  pango_layout_get_extents(layout,
			   &ink_rect,
			   &logical_rect);

  cairo_set_source_rgb(cr,
		       0.0,
		       0.0,
		       0.0);
  
  x_start = 0.0;
  y_start = 0.0;
  
  cairo_move_to(cr,
		x_start,
		y_start + (logical_rect.height / PANGO_SCALE) / 4.0 + 1.0);
  
  pango_cairo_show_layout(cr,
			  layout);

  g_object_unref(layout);

  /* paint */  
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  g_free(font_name);
}

void
ags_file_entry_real_activate(AgsFileEntry *file_entry)
{
  //TODO:JK: implement me
}

/**
 * ags_file_entry_activate:
 * @file_entry: the #AgsFileEntry
 *
 * Activate @file_entry due to user action.
 * 
 * Since: 7.5.0
 */
void
ags_file_entry_activate(AgsFileEntry *file_entry)
{
  g_return_if_fail(AGS_IS_FILE_ENTRY(file_entry));
  
  g_object_ref((GObject *) file_entry);
  g_signal_emit(G_OBJECT(file_entry),
		file_entry_signals[ACTIVATE], 0);
  g_object_unref((GObject *) file_entry);
}

/**
 * ags_file_entry_get_filename:
 * @file_entry: the #AgsFileEntry
 *
 * Get filename of @file_entry.
 *
 * Returns: (transfer full): the filename
 * 
 * Since: 7.5.0
 */
gchar*
ags_file_entry_get_filename(AgsFileEntry *file_entry)
{
  gchar *filename;

  g_return_val_if_fail(AGS_IS_FILE_ENTRY(file_entry), NULL);
  
  filename = NULL;
  
  g_object_get(file_entry,
	       "filename", &filename,
	       NULL);
  
  return(filename);
}

/**
 * ags_file_entry_set_filename:
 * @file_entry: the #AgsFileEntry
 * @filename: the filename
 *
 * Set filename of @file_entry.
 * 
 * Since: 7.5.0
 */
void
ags_file_entry_set_filename(AgsFileEntry *file_entry,
			    gchar *filename)
{
  g_return_if_fail(AGS_IS_FILE_ENTRY(file_entry));

  g_object_set(file_entry,
	       "filename", filename,
	       NULL);

  gtk_widget_queue_draw((GtkWidget *) file_entry->drawing_area);
}

/**
 * ags_file_entry_new:
 *
 * Creates a new instance of #AgsFileEntry.
 *
 * Returns: the new #AgsFileEntry
 *
 * Since: 7.5.0
 */
AgsFileEntry*
ags_file_entry_new()
{
  AgsFileEntry *file_entry;

  file_entry = (AgsFileEntry *) g_object_new(AGS_TYPE_FILE_ENTRY,
					     NULL);
  
  return(file_entry);
}

