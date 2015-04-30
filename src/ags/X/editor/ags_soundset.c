/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/editor/ags_soundset.h>
#include <ags/X/editor/ags_soundset_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_editor.h>

#include <math.h>

void ags_soundset_class_init(AgsSoundsetClass *soundset);
void ags_soundset_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_soundset_init(AgsSoundset *soundset);
void ags_soundset_connect(AgsConnectable *connectable);
void ags_soundset_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_soundset
 * @short_description: piano widget
 * @title: AgsSoundset
 * @section_id:
 * @include: ags/X/editor/ags_soundset.h
 *
 * The #AgsSoundset draws you a piano.
 */

GtkStyle *soundset_style;

GType
ags_soundset_get_type(void)
{
  static GType ags_type_soundset = 0;

  if (!ags_type_soundset){
    static const GTypeInfo ags_soundset_info = {
      sizeof (AgsSoundsetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_soundset_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSoundset),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_soundset_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_soundset_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_soundset = g_type_register_static(GTK_TYPE_DRAWING_AREA,
					       "AgsSoundset\0", &ags_soundset_info,
					       0);
    
    g_type_add_interface_static(ags_type_soundset,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_soundset);
}

void
ags_soundset_class_init(AgsSoundsetClass *soundset)
{
}

void
ags_soundset_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_soundset_connect;
  connectable->disconnect = ags_soundset_disconnect;
}

void
ags_soundset_init(AgsSoundset *soundset)
{
  GtkWidget *widget;

  widget = (GtkWidget *) soundset;
  gtk_widget_set_style(widget, soundset_style);
  gtk_widget_set_size_request(widget, 60, -1);
  gtk_widget_set_events (GTK_WIDGET (soundset), GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 );
}

void
ags_soundset_connect(AgsConnectable *connectable)
{
  AgsSoundset *soundset;

  soundset = AGS_SOUNDSET(connectable);

  g_signal_connect((GObject *) soundset, "expose_event\0",
  		   G_CALLBACK(ags_soundset_expose_event), (gpointer) soundset);

  g_signal_connect((GObject *) soundset, "configure_event\0",
  		   G_CALLBACK(ags_soundset_configure_event), (gpointer) soundset);
}

void
ags_soundset_disconnect(AgsConnectable *connectable)
{
}

void
ags_soundset_paint(AgsSoundset *soundset)
{
  AgsEditor *editor;
  GtkWidget *widget;
  cairo_t *cr;
  guint bitmap;
  guint y[2];
  guint i, i_stop, j, j0;
  guint border_top;

  widget = (GtkWidget *) soundset;
  editor = (AgsEditor *) gtk_widget_get_ancestor(widget, AGS_TYPE_EDITOR);

  border_top = 24; // see ags_ruler.c

  bitmap = 0x555555; // description of the keyboard

  j = (guint) ceil(GTK_RANGE(editor->edit.pattern_edit->vscrollbar)->adjustment->value / (double) editor->edit.pattern_edit->control_height);
  j = j % 12;

  y[0] = (guint) round(GTK_RANGE(editor->edit.pattern_edit->vscrollbar)->adjustment->value) % editor->edit.pattern_edit->control_height;

  if(y[0] != 0){
    y[0] = editor->edit.pattern_edit->control_height - y[0];
  }

  y[1] = ((guint) (widget->allocation.height - border_top) - y[0]) % editor->edit.pattern_edit->control_height;
  i_stop = ((widget->allocation.height - border_top) - y[0] - y[1]) / editor->edit.pattern_edit->control_height;

  cr = gdk_cairo_create(widget->window);

  cairo_set_source_rgb(cr, 0.88, 0.8, 0.0);
  cairo_rectangle(cr, 0.0, (double) border_top, (double) widget->allocation.width, (double) widget->allocation.height - border_top);
  cairo_fill(cr);

  cairo_set_line_width(cr, 1.0);
  cairo_set_source_rgb(cr, 0.68, 0.68, 0.68);

  if(y[0] != 0){
    j0 = (j != 0) ? j -1: 11;

    if(((1 << j0) & bitmap) != 0){
      cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);

      cairo_rectangle(cr, 0.0, 0.0, (double) widget->allocation.width, (double) y[0] + border_top);
      cairo_fill(cr); 	

      cairo_set_source_rgb(cr, 0.68, 0.68, 0.68);

      if(y[0] > editor->edit.pattern_edit->control_height / 2){
	cairo_move_to(cr, (double) widget->allocation.width, (double) (y[0] - editor->edit.pattern_edit->control_height / 2) + border_top);
	cairo_line_to(cr, (double) widget->allocation.width, (double) (y[0] - editor->edit.pattern_edit->control_height / 2) + border_top);
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) widget->allocation.width, border_top);
      cairo_line_to(cr, (double) widget->allocation.width, (double) y[0] + border_top);
      cairo_stroke(cr);
    }else{
      if(((1 << (j0 + 1)) & bitmap) == 0){
	cairo_move_to(cr, 0.0, (double) y[0]);
	cairo_line_to(cr, (double) widget->allocation.width, (double) y[0] + border_top);
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) widget->allocation.width,  border_top);
      cairo_line_to(cr, (double) widget->allocation.width, (double) y[0] + border_top);
      cairo_stroke(cr);
    }
  }

  for(i = 0; i < i_stop; i++){
    if(((1 << j) & bitmap) != 0){
      // draw semi tone key
      cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);

      cairo_rectangle(cr, 0.0, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top), (double) widget->allocation.width, (double) editor->edit.pattern_edit->control_height);
      cairo_fill(cr); 	

      cairo_set_source_rgb(cr, 0.68, 0.68, 0.68);

      cairo_move_to(cr, (double) widget->allocation.width, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top + editor->edit.pattern_edit->control_height / 2));
      cairo_line_to(cr, (double) widget->allocation.width, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top + editor->edit.pattern_edit->control_height / 2));
      cairo_stroke(cr);

      cairo_move_to(cr, (double) widget->allocation.width, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top));
      cairo_line_to(cr, (double) widget->allocation.width, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top + editor->edit.pattern_edit->control_height));
      cairo_stroke(cr);
    }else{
      // no semi tone key
      if(((1 << (j + 1)) & bitmap) == 0){
	cairo_move_to(cr, 0.0, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top + editor->edit.pattern_edit->control_height));
	cairo_line_to(cr, (double) widget->allocation.width, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top + editor->edit.pattern_edit->control_height));
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) widget->allocation.width, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top));
      cairo_line_to(cr, (double) widget->allocation.width, (double) (i * editor->edit.pattern_edit->control_height + y[0] + border_top + editor->edit.pattern_edit->control_height));
      cairo_stroke(cr);
    }

    if(j == 11)
      j = 0;
    else
      j++;
  }

  if(y[1] != 0){
    j0 = j;

    if(((1 << j0) & bitmap) != 0){
      cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);

      cairo_rectangle(cr, 0.0, (double) (widget->allocation.height - y[1]), (double) widget->allocation.width, (double) y[1]);
      cairo_fill(cr); 	

      cairo_set_source_rgb(cr, 0.68, 0.68, 0.68);

      if(y[1] > editor->edit.pattern_edit->control_height / 2){
	cairo_move_to(cr, (double) widget->allocation.width, (double) (widget->allocation.height - y[1] + editor->edit.pattern_edit->control_height / 2));
	cairo_line_to(cr, (double) widget->allocation.width, (double) ((widget->allocation.height) - y[1] + editor->edit.pattern_edit->control_height / 2));
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) widget->allocation.width, (double) (widget->allocation.height - y[1]));
      cairo_line_to(cr, (double) widget->allocation.width, (double) widget->allocation.height);
      cairo_stroke(cr);
    }else{
      cairo_move_to(cr, (double) widget->allocation.width, (double) (widget->allocation.height - y[1]));
      cairo_line_to(cr, (double) widget->allocation.width, (double) widget->allocation.height);
      cairo_stroke(cr);
    }
  }
}

/**
 * ags_soundset_new:
 *
 * Create a new #AgsSoundset.
 *
 * Since: 0.4
 */
AgsSoundset*
ags_soundset_new()
{
  AgsSoundset *soundset;

  soundset = (AgsSoundset *) g_object_new(AGS_TYPE_SOUNDSET, NULL);

  return(soundset);
}
