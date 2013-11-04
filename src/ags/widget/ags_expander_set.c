/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include "ags_expander_set.h"

void ags_expander_set_class_init(AgsExpanderSetClass *expander_set);
void ags_expander_set_init(AgsExpanderSet *expander_set);
void ags_pad_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec);
void ags_pad_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec);
void ags_expander_set_show(GtkWidget *widget);

void ags_expander_set_realize(GtkWidget *widget);
void ags_expander_set_size_request(GtkWidget *widget,
				   GtkRequisition   *requisition);
void ags_expander_set_size_allocate(GtkWidget *widget,
				    GtkAllocation *allocation);
gboolean ags_expander_set_expose(GtkWidget *widget,
				 GdkEventExpose *event);

void ags_expander_set_width_changed(GObject *gobject,
				    GParamSpec *pspec,
				    AgsExpanderSet *expander_set);
void ags_expander_set_height_changed(GObject *gobject,
				     GParamSpec *pspec,
				     AgsExpanderSet *expander_set);

enum{
  PROP_0,
  PROP_LOCATION_X,
  PROP_LOCATION_Y,
  PROP_LOCATION_WIDTH,
  PROP_LOCATION_HEIGHT,
};

static gpointer ags_expander_set_parent_class = NULL;

GType
ags_expander_set_get_type(void)
{
  static GType ags_type_expander_set = 0;

  if(!ags_type_expander_set){
    static const GTypeInfo ags_expander_set_info = {
      sizeof(AgsExpanderSetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_expander_set_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsExpanderSet),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_expander_set_init,
    };

    ags_type_expander_set = g_type_register_static(GTK_TYPE_TABLE,
						   "AgsExpanderSet\0", &ags_expander_set_info,
						   0);
  }

  return(ags_type_expander_set);
}

void
ags_expander_set_class_init(AgsExpanderSetClass *expander_set)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_expander_set_parent_class = g_type_class_peek_parent(expander_set);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(pad);

  gobject->set_property = ags_pad_set_property;
  gobject->get_property = ags_pad_get_property;

  //TODO:JK: add finalize

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) expander_set;

  widget->realize = ags_expander_set_realize;
  widget->expose_event = ags_expander_set_expose;
  widget->size_request = ags_expander_set_size_request;
  widget->size_allocate = ags_expander_set_size_allocate;
}

void
ags_expander_set_init(AgsExpanderSet *expander_set)
{
  g_object_set(G_OBJECT(expander_set),
	       "app-paintable\0", TRUE,
	       NULL);

  g_signal_connect(expander_set, "notify::width\0",
		   G_CALLBACK(ags_expander_set_width_changed),
		   expander_set);
  g_signal_connect(expander_set, "notify::height\0",
		   G_CALLBACK(ags_expander_set_height_changed),
		   expander_set);

  expander_set->flags = 0;

  expander_set->location_x = NULL;
  expander_set->location_y = NULL;

  expander_set->location_width = NULL;
  expander_set->location_height = NULL;
}

void
ags_expander_set_realize(GtkWidget *widget)
{
  //TODO:JK: implement me
}

void
ags_expander_set_size_request(GtkWidget *widget,
			      GtkRequisition   *requisition)
{
  //TODO:JK: implement me
}

void
ags_expander_set_size_allocate(GtkWidget *widget,
			       GtkAllocation *allocation)
{
  //TODO:JK: implement me
}

gboolean
ags_expander_set_expose(GtkWidget *widget,
			GdkEventExpose *event)
{
  //TODO:JK: implement me
}

void
ags_expander_set_width_changed(GObject *gobject,
			       GParamSpec *pspec,
			       AgsExpanderSet *expander_set)
{
  //TODO:JK: implement me
}

void
ags_expander_set_height_changed(GObject *gobject,
				GParamSpec *pspec,
				AgsExpanderSet *expander_set)
{
  //TODO:JK: implement me
}

void
ags_expander_set_add(AgsExpanderSet *expander_set,
		     GtkWidget *widget,
		     guint x, guint y,
		     guint width, guint height)
{
  //TODO:JK: implement me
}

void
ags_expander_set_remove(AgsExpanderSet *expander_set
			GtkWidget *widget)
{
  //TODO:JK: implement me
}

void
ags_expander_set_move(AgsExpanderSet *expander_set,
		      guint *x_new, guint *y_new,
		      guint *x_old, guint *y_old)
{
  //TODO:JK: implement me
}

AgsExpanderSet*
ags_expander_set_new(guint width, guint height,
		     guint *x, guint *y)
{
  AgsExpanderSet *expander_set;

  expander_set = (AgsExpanderSet *) g_object_new(AGS_TYPE_EXPANDER_SET,
						 "width\0", width,
						 "height\0", height,
						 "location_x\0", x,
						 "location_y\0", y,
						 NULL);
  
  return(expander_set);
}
