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

#include <ags/widget/ags_dial.h>

void ags_dial_class_init(AgsDialClass *dial);
void ags_dial_init(AgsDial *dial);

void ags_dial_draw(AgsDial *dial);

GType
ags_dial_get_type(void)
{
  static GType ags_type_dial = 0;

  if(!ags_type_dial){
    static const GTypeInfo ags_dial_info = {
      sizeof(AgsDialClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_dial_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDial),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_dial_init,
    };

    ags_type_dial = g_type_register_static(GTK_TYPE_WINDOW,
					   "AgsDial\0", &ags_dial_info,
					   0);
  }

  return(ags_type_dial);
}

void
ags_dial_class_init(AgsDialClass *dial)
{
}

void
ags_dial_init(AgsDial *dial)
{
}

void
ags_dial_draw(AgsDial *dial)
{
  GtkWidget *widget;
  GdkWindow *window;
  cairo_t *cr;
  cairo_text_extents_t te;
  GValue value = {0,};

  widget = GTK_WIDGET(dial);

  /*
  g_value_init(&value, G_TYPE_OBJECT);
  window = (GdkWindow *) g_object_get_property(dial,
					       "window\0",
					       &value);
  g_value_unset(&value);
  */

  cr = gdk_cairo_create(widget->window);

  /* draw controller button down */
  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
  cairo_select_font_face (cr, "Georgia",
			  CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, 1.2);
  cairo_text_extents (cr, "-\0", &te);
  cairo_move_to (cr, 0.5 - te.width / 2 - te.x_bearing,
		 0.5 - te.height / 2 - te.y_bearing);
  cairo_show_text (cr, "-\0");

  /* draw control visualization */
  cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
  //  cairo_rectangle(cr, 0.0, 0.0, (double) widget->allocation.width, (double) widget->allocation.height);
  cairo_arc (cr, 0.5, 0.5, 0.25 * sqrt(2), -1 * M_PI, 1 * M_PI);
  cairo_fill(cr);

  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
  cairo_arc (cr, 0.5, 0.5, 0.25 * sqrt(2), 0.45 * M_PI, 0.55 * M_PI);
  cairo_fill(cr);

  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8);
  cairo_arc (cr, 0.5, 0.5, 0.25 * sqrt(2), -0.45 * M_PI, -0.55 * M_PI);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_arc (cr, 0.5, 0.5, 0.25 * sqrt(2), -1 * M_PI, 1 * M_PI);
  cairo_stroke(cr);

  /* draw controller button up */
  cairo_text_extents (cr, "+\0", &te);
  cairo_move_to (cr, 0.5 - te.width / 2 - te.x_bearing,
		 0.5 - te.height / 2 - te.y_bearing);
  cairo_show_text (cr, "+\0");
}

AgsDial*
ags_dial_new()
{
  AgsDial *dial;

  dial = (AgsDial *) g_object_new(AGS_TYPE_DIAL, NULL);
  
  return(dial);
}
