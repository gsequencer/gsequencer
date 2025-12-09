/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/machine/ags_modulation_matrix.h>
#include <ags/app/machine/ags_modulation_matrix_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <ags/ags_api_config.h>

#include <ags/i18n.h>

void ags_modulation_matrix_class_init(AgsModulationMatrixClass *modulation_matrix);
void ags_modulation_matrix_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_modulation_matrix_init(AgsModulationMatrix *modulation_matrix);
void ags_modulation_matrix_finalize(GObject *gobject);

gboolean ags_modulation_matrix_is_connected(AgsConnectable *connectable);
void ags_modulation_matrix_connect(AgsConnectable *connectable);
void ags_modulation_matrix_disconnect(AgsConnectable *connectable);

void ags_modulation_matrix_show(GtkWidget *widget);

/**
 * SECTION:ags_modulation_matrix
 * @short_description: modular synth
 * @title: AgsModulationMatrix
 * @section_id:
 * @include: ags/app/machine/ags_modulation_matrix.h
 *
 * The #AgsModulationMatrix is a composite widget to act as modular synth.
 */

static gpointer ags_modulation_matrix_parent_class = NULL;
static AgsConnectableInterface *ags_modulation_matrix_parent_connectable_interface;

GType
ags_modulation_matrix_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_modulation_matrix = 0;

    static const GTypeInfo ags_modulation_matrix_info = {
      sizeof(AgsModulationMatrixClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_modulation_matrix_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsModulationMatrix),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_modulation_matrix_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_modulation_matrix_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_modulation_matrix = g_type_register_static(GTK_TYPE_BOX,
							"AgsModulationMatrix", &ags_modulation_matrix_info,
							0);
    
    g_type_add_interface_static(ags_type_modulation_matrix,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_modulation_matrix);
  }

  return(g_define_type_id__static);
}

void
ags_modulation_matrix_class_init(AgsModulationMatrixClass *modulation_matrix)
{
  GObjectClass *gobject;

  ags_modulation_matrix_parent_class = g_type_class_peek_parent(modulation_matrix);

  /* GObjectClass */
  gobject = (GObjectClass *) modulation_matrix;

  gobject->finalize = ags_modulation_matrix_finalize;
}

void
ags_modulation_matrix_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_modulation_matrix_is_connected;

  connectable->connect = ags_modulation_matrix_connect;
  connectable->disconnect = ags_modulation_matrix_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_modulation_matrix_init(AgsModulationMatrix *modulation_matrix)
{
  modulation_matrix->font_size = 11;

  modulation_matrix->grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) modulation_matrix->grid,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) modulation_matrix->grid,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) modulation_matrix->grid,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) modulation_matrix->grid,
			 TRUE);

  gtk_box_append((GtkBox *) modulation_matrix,
		 (GtkWidget *) modulation_matrix->grid);

  modulation_matrix->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();

  gtk_widget_set_can_focus((GtkWidget *) modulation_matrix->drawing_area,
			   TRUE);
  gtk_widget_set_focusable((GtkWidget *) modulation_matrix->drawing_area,
			   TRUE);

  gtk_widget_set_size_request(GTK_WIDGET(modulation_matrix->drawing_area),
			      (AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + 2) + ((AGS_MODULATION_MATRIX_DEFAULT_CONTROLS_HORIZONTALLY * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_WIDTH) + 2),
			      (AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + 2) + ((AGS_MODULATION_MATRIX_DEFAULT_CONTROLS_VERTICALLY * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_HEIGHT) + 2));

  gtk_widget_set_halign((GtkWidget *) modulation_matrix->drawing_area,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) modulation_matrix->drawing_area,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) modulation_matrix->drawing_area,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) modulation_matrix->drawing_area,
			 TRUE);

  gtk_grid_attach((GtkGrid *) modulation_matrix->grid,
		  (GtkWidget *) modulation_matrix->drawing_area,
		  0, 0,
		  1, 1);
  
  //TODO:JK: implement me
}

void
ags_modulation_matrix_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_modulation_matrix_parent_class)->finalize(gobject);
}

gboolean
ags_modulation_matrix_is_connected(AgsConnectable *connectable)
{
  AgsModulationMatrix *modulation_matrix;

  gboolean is_connected;
  
  /* AgsModulationMatrix */
  modulation_matrix = AGS_MODULATION_MATRIX(connectable);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (modulation_matrix->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_modulation_matrix_connect(AgsConnectable *connectable)
{
  AgsModulationMatrix *modulation_matrix;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  /* AgsModulationMatrix */
  modulation_matrix = AGS_MODULATION_MATRIX(connectable);

  /* drawing area */
  gtk_drawing_area_set_draw_func(modulation_matrix->drawing_area,
				 (GtkDrawingAreaDrawFunc) ags_modulation_matrix_draw_callback,
				 modulation_matrix,
				 NULL);

  //TODO:JK: implement me
}

void
ags_modulation_matrix_disconnect(AgsConnectable *connectable)
{
  AgsModulationMatrix *modulation_matrix;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  /* AgsModulationMatrix */
  modulation_matrix = AGS_MODULATION_MATRIX(connectable);

  /* drawing area */
  gtk_drawing_area_set_draw_func(modulation_matrix->drawing_area,
				 NULL,
				 NULL,
				 NULL);

  //TODO:JK: implement me
}

void
ags_modulation_matrix_draw(AgsModulationMatrix *modulation_matrix,
			   cairo_t *cr)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  PangoLayout *layout;
  PangoFontDescription *desc;

  PangoRectangle ink_rect, logical_rect;

  GtkAllocation allocation;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;
  
  gchar **sends_source_strv;
  gchar **sends_sink_strv;

  gchar *font_name;
  gchar *text;

  double width, height;
  gdouble x_start, y_start;
  guint i;
  
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean shadow_success;

  GStrvBuilder *strv_builder;

  strv_builder = g_strv_builder_new();

  g_strv_builder_add_many(strv_builder,
			  "osc-0 - frequency",
			  "osc-0 - phase",
			  "osc-0 - volume",
			  "osc-1 - frequency",
			  "osc-1 - phase",
			  "osc-1 - volume",
			  "pitch tuning",
			  "volume",
			  NULL);

  sends_sink_strv = g_strv_builder_end(strv_builder);

  g_strv_builder_add_many(strv_builder,
			  "env-0",
			  "env-1",
			  "lfo-0",
			  "lfo-1",
			  "noise",
			  NULL);

  sends_source_strv = g_strv_builder_end(strv_builder);

  g_strv_builder_unref(strv_builder);
  
  gtk_widget_get_allocation(GTK_WIDGET(modulation_matrix->drawing_area),
			    &allocation);

  width = (double) allocation.width;
  height = (double) allocation.height;

  /* style context */
  style_context = gtk_widget_get_style_context((GtkWidget *) modulation_matrix->drawing_area);  

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

  if(!fg_success ||
     !bg_success ||
     !shadow_success){
    if(!dark_theme){
      gdk_rgba_parse(&fg_color,
		     "#101010");
      
      gdk_rgba_parse(&bg_color,
		     "#cbd5d9");
      
      gdk_rgba_parse(&shadow_color,
		     "#ffffff40");
    }else{
      gdk_rgba_parse(&fg_color,
		     "#eeeeec");
      
      gdk_rgba_parse(&bg_color,
		     "#353535");
      
      gdk_rgba_parse(&shadow_color,
		     "#202020");
    }
  }

  /* push group */
  cairo_push_group(cr);

  /* clear with background color */
  cairo_set_source_rgba(cr,
			bg_color.red,
			bg_color.green,
			bg_color.blue,
			bg_color.alpha);
  cairo_rectangle(cr,
		  0.0, 0.0,
		  width, height);
  cairo_fill(cr);

  /* horizontal lines */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);

  cairo_set_line_width(cr,
		       1.0);

  for(i = 0; i < 6; i++){
    cairo_move_to(cr,
		  (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH,
		  2.0 + AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (double) (i * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_HEIGHT));
  
    cairo_line_to(cr,
		  (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (double) (AGS_MODULATION_MATRIX_DEFAULT_CONTROLS_HORIZONTALLY * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_WIDTH),
		  2.0 + (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (i * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_HEIGHT));

  }
  
  cairo_stroke(cr);

  /* vertical lines */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);

  cairo_set_line_width(cr,
		       1.0);

  for(i = 0; i < 9; i++){
    cairo_move_to(cr,
		  (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (double) (i * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_WIDTH),
		  2.0 + AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH);
  
    cairo_line_to(cr,
		  AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (double) (i * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_WIDTH),
		  2.0 + AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (double) (AGS_MODULATION_MATRIX_DEFAULT_CONTROLS_VERTICALLY * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_HEIGHT));

  }
  
  cairo_stroke(cr);

  /* horizontal text */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);

  x_start = 8.0;
  y_start = 2.0;
  
  for(i = 0; i < 5; i++){
    text = g_strdup_printf("%s",
			   (sends_source_strv[i]));

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  text,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    modulation_matrix->font_size * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			     &ink_rect,
			     &logical_rect);

    cairo_move_to(cr,
		  x_start,
		  y_start + (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (double) (i * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_HEIGHT) - 8.0 + ((double) logical_rect.height / (double) PANGO_SCALE) / 4.0);
  
    pango_cairo_show_layout(cr,
			    layout);
    
    /* unref */
    g_object_unref(layout);

    g_free(text);
  }

  /* vertical text */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);
  x_start = 8.0;
  y_start = 2.0;
  
  for(i = 0; i < 8; i++){
    cairo_save(cr);

    text = g_strdup_printf("%s",
			   (sends_sink_strv[i]));

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  text,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    modulation_matrix->font_size * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			     &ink_rect,
			     &logical_rect);
    
    cairo_move_to(cr,
		  x_start + (double) AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH + (double) ((i + 1) * AGS_MODULATION_MATRIX_DEFAULT_MODULATION_WIDTH) - 8.0,
		  y_start + ((double) logical_rect.height / (double) PANGO_SCALE) / 4.0);
    cairo_rotate(cr,
		 2.0 * M_PI * -0.75);
    
    pango_cairo_show_layout(cr,
			    layout);

    cairo_restore(cr);    

    /* unref */
    g_object_unref(layout);

    g_free(text);
  }
  
  /* complete */
  cairo_pop_group_to_source(cr);

  cairo_paint(cr);
}

/**
 * ags_modulation_matrix_new:
 *
 * Create a new instance of #AgsModulationMatrix
 *
 * Returns: the new #AgsModulationMatrix
 *
 * Since: 8.2.0
 */
AgsModulationMatrix*
ags_modulation_matrix_new()
{
  AgsModulationMatrix *modulation_matrix;

  modulation_matrix = (AgsModulationMatrix *) g_object_new(AGS_TYPE_MODULATION_MATRIX,
							   NULL);

  return(modulation_matrix);
}
