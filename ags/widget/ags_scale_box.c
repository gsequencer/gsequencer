/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/widget/ags_scale_box.h>

void ags_scale_box_class_init(AgsScaleBoxClass *scale_box);
void ags_scale_box_init(AgsScaleBox *scale_box);
void ags_scale_box_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_scale_box_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_scale_box_finalize(GObject *gobject);

void ags_scale_box_add(GtkContainer *container,
		       GtkWidget *widget);
void ags_scale_box_remove(GtkContainer *container,
			  GtkWidget *widget);
void ags_scale_box_forall(GtkContainer *container,
			  gboolean include_internals,
			  GtkCallback callback,
			  gpointer callback_data);
void ags_scale_box_set_child_property(GtkContainer *container,
				      GtkWidget *child,
				      guint property_id,
				      const GValue *value,
				      GParamSpec *pspec);
void ags_scale_box_get_child_property(GtkContainer *container,
				      GtkWidget *child,
				      guint property_id,
				      GValue *value,
				      GParamSpec *pspec);
GType ags_scale_box_child_type(GtkContainer *container);

/**
 * SECTION:ags_scale_box
 * @short_description: abstract box widget
 * @title: AgsScaleBox
 * @section_id:
 * @include: ags/widget/ags_scale_box.h
 *
 * The #AgsScaleBox is an abstract box widget containing #AgsScale.
 */

enum{
  PROP_0,
  PROP_FIXED_SCALE_WIDTH,
  PROP_FIXED_SCALE_HEIGHT,
};

enum {
  CHILD_PROP_0,
  CHILD_PROP_PADDING,
  CHILD_PROP_POSITION,
};

static gpointer ags_scale_box_parent_class = NULL;

GType
ags_scale_box_get_type(void)
{
  static GType ags_type_scale_box = 0;

  if(!ags_type_scale_box){
    static const GTypeInfo ags_scale_box_info = {
      sizeof (AgsScaleBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scale_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsScaleBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scale_box_init,
    };

    ags_type_scale_box = g_type_register_static(GTK_TYPE_CONTAINER,
					       "AgsScaleBox", &ags_scale_box_info,
					       0);
  }
  
  return(ags_type_scale_box);
}

void
ags_scale_box_class_init(AgsScaleBoxClass *scale_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GtkContainerClass *container;

  GParamSpec *param_spec;

  ags_scale_box_parent_class = g_type_class_peek_parent(scale_box);

  /* GObjectClass */
  gobject = (GObjectClass *) scale_box;

  gobject->set_property = ags_scale_box_set_property;
  gobject->get_property = ags_scale_box_get_property;

  gobject->finalize = ags_scale_box_finalize;

  /* properties */
  /**
   * AgsScaleBox:fixed-scale-width:
   *
   * The fixed width of a scale.
   * 
   * Since: 1.3.0
   */
  param_spec = g_param_spec_string("fixed-scale-width",
				   "fixed scale width",
				   "The fixed width of a scale",
				   AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_SCALE_WIDTH,
				  param_spec);

  /**
   * AgsScaleBox:fixed-scale-height:
   *
   * The fixed height of a scale.
   * 
   * Since: 1.3.0
   */
  param_spec = g_param_spec_string("fixed-scale-height",
				   "fixed scale height",
				   "The fixed height of a scale",
				   AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_SCALE_HEIGHT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) scale_box;

  /* GtkContainerClass */
  container = (GtkWidgetClass *) scale_box;

  container->add = gtk_scale_box_add;
  container->remove = gtk_scale_box_remove;
  container->forall = gtk_scale_box_forall;
  container->child_type = gtk_scale_box_child_type;
  container->set_child_property = gtk_scale_box_set_child_property;
  container->get_child_property = gtk_scale_box_get_child_property;

  gtk_container_class_install_child_property(container,
					     CHILD_PROP_PADDING,
					     g_param_spec_uint("padding", 
							       "padding",
							       "Extra space to put between the child and its neighbors, in pixels",
							       0, G_MAXINT, 0,
							       GTK_PARAM_READWRITE));
  
  gtk_container_class_install_child_property(container,
					     CHILD_PROP_POSITION,
					     g_param_spec_int("position", 
							      "position", 
							      "The index of the child in the parent",
							      -1, G_MAXINT, 0,
							      GTK_PARAM_READWRITE));
}

void
ags_scale_box_init(AgsScaleBox *scale_box)
{
  scale_box->flags = 0;

  scale_box->fixed_scale_width = AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_WIDTH;
  scale_box->fixed_scale_height = AGS_SCALE_BOX_DEFAULT_FIXED_SCALE_HEIGHT;

  scale_box->scale = NULL;
}

void
ags_scale_box_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsScaleBox *scale_box;

  scale_box = AGS_SCALE_BOX(gobject);

  switch(prop_id){
  case PROP_FIXED_SCALE_WIDTH:
    {
      scale_box->fixed_scale_width = g_value_get_uint(value);
    }
    break;
  case PROP_FIXED_SCALE_HEIGHT:
    {
      scale_box->fixed_scale_height = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scale_box_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsScaleBox *scale_box;

  scale_box = AGS_SCALE_BOX(gobject);

  switch(prop_id){
  case PROP_FIXED_SCALE_WIDTH:
    {
      g_value_set_uint(scale_box->fixed_scale_width);
    }
    break;
  case PROP_FIXED_SCALE_HEIGHT:
    {
      g_value_set_uint(scale_box->fixed_scale_height);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scale_box_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_scale_box_parent_class)->finalize(gobject);
}

void
ags_scale_box_add(GtkContainer *container,
		  GtkWidget *widget)
{
}

void
ags_scale_box_remove(GtkContainer *container,
		     GtkWidget *widget)
{
}

void
ags_scale_box_forall(GtkContainer *container,
		     gboolean include_internals,
		     GtkCallback callback,
		     gpointer callback_data)
{
}

void
ags_scale_box_set_child_property(GtkContainer *container,
				 GtkWidget *child,
				 guint property_id,
				 const GValue *value,
				 GParamSpec *pspec)
{
}

void
ags_scale_box_get_child_property(GtkContainer *container,
				 GtkWidget *child,
				 guint property_id,
				 GValue *value,
				 GParamSpec *pspec)
{
}

GType ags_scale_box_child_type(GtkContainer *container)
{
}

/**
 * ags_scale_box_new:
 * 
 * Create a new instance of #AgsScaleBox.
 * 
 * Returns: the new #AgsScaleBox instance
 * 
 * Since: 1.3.0
 */
AgsScale_Box*
ags_scale_box_new()
{
  AgsScaleBox *scale_box;

  scale_box = (AgsScaleBox *) g_object_new(AGS_TYPE_SCALE_BOX,
					   NULL);
  
  return(scale_box);
}
