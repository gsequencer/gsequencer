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

#include <ags/X/editor/ags_automation_edit_box.h>

#include <ags/X/editor/ags_automation_edit.h>

void ags_automation_edit_box_class_init(AgsAutomationEditBoxClass *automation_edit_box);
void ags_automation_edit_box_init(AgsAutomationEditBox *automation_edit_box);
void ags_automation_edit_box_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_automation_edit_box_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_automation_edit_box_finalize(GObject *gobject);

GType ags_automation_edit_box_child_type(GtkContainer *container);

/**
 * SECTION:ags_automation_edit_box
 * @short_description: abstract box widget
 * @title: AgsAutomationEditBox
 * @section_id:
 * @include: ags/widget/ags_automation_edit_box.h
 *
 * The #AgsAutomationEditBox is an abstract box widget containing #AgsAutomationEdit.
 */

enum{
  PROP_0,
  PROP_FIXED_EDIT_WIDTH,
  PROP_FIXED_EDIT_HEIGHT,
};

static gpointer ags_automation_edit_box_parent_class = NULL;

GType
ags_automation_edit_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_automation_edit_box;

    static const GTypeInfo ags_automation_edit_box_info = {
      sizeof (AgsAutomationEditBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_edit_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationEditBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_edit_box_init,
    };

    ags_type_automation_edit_box = g_type_register_static(GTK_TYPE_BOX,
							  "AgsAutomationEditBox", &ags_automation_edit_box_info,
							  0);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_automation_edit_box);
  }

  return g_define_type_id__volatile;
}

void
ags_automation_edit_box_class_init(AgsAutomationEditBoxClass *automation_edit_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GtkContainerClass *container;

  GParamSpec *param_spec;

  ags_automation_edit_box_parent_class = g_type_class_peek_parent(automation_edit_box);

  /* GObjectClass */
  gobject = (GObjectClass *) automation_edit_box;

  gobject->set_property = ags_automation_edit_box_set_property;
  gobject->get_property = ags_automation_edit_box_get_property;

  gobject->finalize = ags_automation_edit_box_finalize;

  /* properties */
  /**
   * AgsAutomationEditBox:fixed-edit-height:
   *
   * The fixed height of a edit.
   * 
   * Since: 1.3.0
   */
  param_spec = g_param_spec_uint("fixed-edit-height",
				 "fixed edit height",
				 "The fixed height of a edit",
				 0,
				 G_MAXUINT,
				 AGS_AUTOMATION_EDIT_BOX_DEFAULT_FIXED_EDIT_HEIGHT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIXED_EDIT_HEIGHT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) automation_edit_box;
  
  /* GtkContainerClass */
  container = (GtkWidgetClass *) automation_edit_box;

  container->child_type = ags_automation_edit_box_child_type;
}

void
ags_automation_edit_box_init(AgsAutomationEditBox *automation_edit_box)
{
  g_object_set(automation_edit_box,
	       "homogeneous", FALSE,
	       "spacing", 0,
	       NULL);
  
  automation_edit_box->flags = 0;

  automation_edit_box->fixed_edit_height = AGS_AUTOMATION_EDIT_BOX_DEFAULT_FIXED_EDIT_HEIGHT;
}

void
ags_automation_edit_box_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsAutomationEditBox *automation_edit_box;

  automation_edit_box = AGS_AUTOMATION_EDIT_BOX(gobject);

  switch(prop_id){
  case PROP_FIXED_EDIT_HEIGHT:
    {
      automation_edit_box->fixed_edit_height = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_edit_box_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsAutomationEditBox *automation_edit_box;

  automation_edit_box = AGS_AUTOMATION_EDIT_BOX(gobject);

  switch(prop_id){
  case PROP_FIXED_EDIT_HEIGHT:
    {
      g_value_set_uint(value,
		       automation_edit_box->fixed_edit_height);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_automation_edit_box_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_automation_edit_box_parent_class)->finalize(gobject);
}

GType ags_automation_edit_box_child_type(GtkContainer *container)
{
  return(AGS_TYPE_AUTOMATION_EDIT);
}

/**
 * ags_automation_edit_box_new:
 * 
 * Create a new instance of #AgsAutomationEditBox.
 * 
 * Returns: the new #AgsAutomationEditBox instance
 * 
 * Since: 1.3.0
 */
AgsAutomationEditBox*
ags_automation_edit_box_new()
{
  AgsAutomationEditBox *automation_edit_box;

  automation_edit_box = (AgsAutomationEditBox *) g_object_new(AGS_TYPE_AUTOMATION_EDIT_BOX,
							      NULL);
  
  return(automation_edit_box);
}
