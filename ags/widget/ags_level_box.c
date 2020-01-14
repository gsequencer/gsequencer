/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/widget/ags_level_box.h>
#include <ags/widget/ags_level.h>

void ags_level_box_class_init(AgsLevelBoxClass *level_box);
void ags_level_box_init(AgsLevelBox *level_box);
void ags_level_box_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_level_box_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_level_box_finalize(GObject *gobject);

GType ags_level_box_child_type(GtkContainer *container);

/**
 * SECTION:ags_level_box
 * @short_description: abstract box widget
 * @title: AgsLevelBox
 * @section_id:
 * @include: ags/widget/ags_level_box.h
 *
 * The #AgsLevelBox is an abstract box widget containing #AgsLevel.
 */

enum{
  PROP_0,
  PROP_FIXED_LEVEL_WIDTH,
  PROP_FIXED_LEVEL_HEIGHT,
};

static gpointer ags_level_box_parent_class = NULL;

GType
ags_level_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_level_box = 0;

    static const GTypeInfo ags_level_box_info = {
      sizeof (AgsLevelBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_level_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLevelBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_level_box_init,
    };

    ags_type_level_box = g_type_register_static(GTK_TYPE_BOX,
						"AgsLevelBox", &ags_level_box_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_level_box);
  }

  return g_define_type_id__volatile;
}

void
ags_level_box_class_init(AgsLevelBoxClass *level_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GtkContainerClass *container;

  GParamSpec *param_spec;

  ags_level_box_parent_class = g_type_class_peek_parent(level_box);

  /* GObjectClass */
  gobject = (GObjectClass *) level_box;

  gobject->set_property = ags_level_box_set_property;
  gobject->get_property = ags_level_box_get_property;

  gobject->finalize = ags_level_box_finalize;

  /* properties */
  /**
   * AgsLevelBox:fixed-level-width:
   *
   * The fixed width of a level.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("fixed-level-width",
				 "fixed level width",
				 "The fixed width of a level",
				 0,
				 G_MAXUINT,
				 AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_WIDTH,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_LEVEL_WIDTH,
				  param_spec);

  /**
   * AgsLevelBox:fixed-level-height:
   *
   * The fixed height of a level.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("fixed-level-height",
				 "fixed level height",
				 "The fixed height of a level",
				 0,
				 G_MAXUINT,
				 AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_HEIGHT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_LEVEL_HEIGHT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) level_box;

  /* GtkContainerClass */
  container = (GtkWidgetClass *) level_box;

  container->child_type = ags_level_box_child_type;
}

void
ags_level_box_init(AgsLevelBox *level_box)
{
  g_object_set(level_box,
	       "homogeneous", FALSE,
	       "spacing", 0,
	       NULL);
  
  level_box->flags = 0;

  level_box->fixed_level_width = AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_WIDTH;
  level_box->fixed_level_height = AGS_LEVEL_BOX_DEFAULT_FIXED_LEVEL_HEIGHT;
}

void
ags_level_box_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsLevelBox *level_box;

  level_box = AGS_LEVEL_BOX(gobject);

  switch(prop_id){
  case PROP_FIXED_LEVEL_WIDTH:
    {
      level_box->fixed_level_width = g_value_get_uint(value);
    }
    break;
  case PROP_FIXED_LEVEL_HEIGHT:
    {
      level_box->fixed_level_height = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_level_box_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsLevelBox *level_box;

  level_box = AGS_LEVEL_BOX(gobject);

  switch(prop_id){
  case PROP_FIXED_LEVEL_WIDTH:
    {
      g_value_set_uint(value,
		       level_box->fixed_level_width);
    }
    break;
  case PROP_FIXED_LEVEL_HEIGHT:
    {
      g_value_set_uint(value,
		       level_box->fixed_level_height);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_level_box_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_level_box_parent_class)->finalize(gobject);
}

GType
ags_level_box_child_type(GtkContainer *container)
{
  return(AGS_TYPE_LEVEL);
}

/**
 * ags_level_box_new:
 * 
 * Create a new instance of #AgsLevelBox.
 * 
 * Returns: the new #AgsLevelBox instance
 * 
 * Since: 3.0.0
 */
AgsLevelBox*
ags_level_box_new()
{
  AgsLevelBox *level_box;

  level_box = (AgsLevelBox *) g_object_new(AGS_TYPE_LEVEL_BOX,
					   NULL);
  
  return(level_box);
}
