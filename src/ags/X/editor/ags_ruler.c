/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/editor/ags_ruler.h>
#include <ags/X/editor/ags_ruler_callbacks.h>

#include <ags/X/ags_editor.h>

#include <math.h>

void ags_ruler_class_init(AgsRulerClass *ruler);
void ags_ruler_init(AgsRuler *ruler);
void ags_ruler_destroy(GtkObject *object);
void ags_ruler_connect(AgsRuler *ruler);
void ags_ruler_paint(AgsRuler *ruler);

GtkStyle *ruler_style;

GType
ags_ruler_get_type()
{
  static GType ags_ruler_type = 0;

  if(!ags_ruler_type){
    static const GtkTypeInfo ags_ruler_info = {
      "AgsRuler",
      sizeof(AgsRuler),
      sizeof(AgsRulerClass),
      (GtkClassInitFunc) ags_ruler_class_init,
      (GtkObjectInitFunc) ags_ruler_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    ags_ruler_type = gtk_type_unique (GTK_TYPE_DRAWING_AREA, &ags_ruler_info);
  }

  return (ags_ruler_type);
}

void
ags_ruler_class_init(AgsRulerClass *ruler)
{
}

void
ags_ruler_init(AgsRuler *ruler)
{
  GtkWidget *widget;
  PangoLanguage *lang;
  PangoFontMetrics *metrics;
  PangoAttrList *attr_list;
  PangoAttribute *attr;

  widget = (GtkWidget *) ruler;
  gtk_widget_set_style(widget, ruler_style);
  //  gtk_widget_set_size_request(widget, 60, 20);
  gtk_widget_set_events (widget, GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 );

  /*
  ruler->layout = gtk_widget_create_pango_layout(widget, NULL);

  ruler->font_name = "Sans 10";

  ruler->font_desc = pango_font_description_from_string(ruler->font_name);
  pango_layout_set_font_description(ruler->layout, ruler->font_desc);

  ruler->context = gtk_widget_get_pango_context(widget); // pango_xft_get_context(xdisplay, screen);

  attr_list = pango_attr_list_new();

  attr = pango_attr_scale_new(0.6);
  pango_attr_list_insert(attr_list, attr);

  pango_layout_set_attributes(ruler->layout, attr_list);
  */

}

void
ags_ruler_destroy(GtkObject *object)
{
}

void
ags_ruler_connect(AgsRuler *ruler)
{
  g_signal_connect((GObject *) ruler, "expose_event",
		   G_CALLBACK(ags_ruler_expose_event), (gpointer) ruler);

  g_signal_connect((GObject *) ruler, "configure_event",
		   G_CALLBACK(ags_ruler_configure_event), (gpointer) ruler);
}

void
ags_ruler_paint(AgsRuler *ruler)
{
  AgsEditor *editor;
  GtkWidget *widget;
  guint i;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) ruler, AGS_TYPE_EDITOR);
  widget = GTK_WIDGET(ruler);

  gdk_draw_rectangle(widget->window,
		     widget->style->bg_gc[0],
		     TRUE,
		     0, 0,
		     widget->allocation.width, widget->allocation.height);

  //  attr->end_index = 382;

  //  pango_layout_set_text();

  for(i = 0; i < (guint) floor((double)(widget->allocation.width - 60 - GTK_WIDGET(editor->vscrollbar)->allocation.width) / (double)editor->control_current.control_width); i++){
    gdk_draw_line(widget->window,
		  widget->style->fg_gc[0],
		  i * editor->control_current.control_width + 60, widget->allocation.height - 6,
		  i * editor->control_current.control_width + 60, widget->allocation.height);

  }
}

AgsRuler*
ags_ruler_new()
{
  AgsRuler *ruler;

  ruler = (AgsRuler *) g_object_new(AGS_TYPE_RULER, NULL);

  return(ruler);
}
