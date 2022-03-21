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

#include <ags/app/editor/ags_automation_edit_box.h>

#include <ags/app/editor/ags_automation_edit.h>

#include <ags/i18n.h>

void ags_automation_edit_box_class_init(AgsAutomationEditBoxClass *automation_edit_box);
void ags_automation_edit_box_init(AgsAutomationEditBox *automation_edit_box);
void ags_automation_edit_box_finalize(GObject *gobject);

void ags_automation_edit_box_notify_width_request_callback(GObject *gobject,
							   GParamSpec *pspec,
							   AgsAutomationEditBox *automation_edit_box);
void ags_automation_edit_box_notify_height_request_callback(GObject *gobject,
							    GParamSpec *pspec,
							    AgsAutomationEditBox *automation_edit_box);

/**
 * SECTION:ags_automation_edit_box
 * @short_description: box widget
 * @title: AgsAutomationEditBox
 * @section_id:
 * @include: ags/widget/ags_automation_edit_box.h
 *
 * The #AgsAutomationEditBox is a box widget containing #AgsAutomationEdit.
 */

enum{
  CHILD_WIDTH_REQUEST,
  CHILD_HEIGHT_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_automation_edit_box_parent_class = NULL;
static guint automation_edit_box_signals[LAST_SIGNAL];

GType
ags_automation_edit_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_automation_edit_box = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_automation_edit_box);
  }

  return g_define_type_id__volatile;
}

void
ags_automation_edit_box_class_init(AgsAutomationEditBoxClass *automation_edit_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_automation_edit_box_parent_class = g_type_class_peek_parent(automation_edit_box);

  /* GObjectClass */
  gobject = (GObjectClass *) automation_edit_box;

  gobject->finalize = ags_automation_edit_box_finalize;

  /* properties */

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) automation_edit_box;

  /* AgsAutomationEditBox */
  automation_edit_box->child_width_request = NULL;
  automation_edit_box->child_height_request = NULL;
  
  /**
   * AgsAutomationEditBox::child-width-request:
   * @automation_edit_box: the #AgsAutomationEditBox.
   *
   * The ::child-width-request
   *
   * Since: 4.0.0
   */
  automation_edit_box_signals[CHILD_WIDTH_REQUEST] = 
    g_signal_new("child-width-request",
		 G_TYPE_FROM_CLASS(automation_edit_box),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAutomationEditBoxClass, child_width_request),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__OBJECT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_INT);

  /**
   * AgsAutomationEditBox::child-height-request:
   * @automation_edit_box: the #AgsAutomationEditBox.
   *
   * The ::child-height-request
   *
   * Since: 4.0.0
   */
  automation_edit_box_signals[CHILD_HEIGHT_REQUEST] = 
    g_signal_new("child-height-request",
		 G_TYPE_FROM_CLASS(automation_edit_box),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAutomationEditBoxClass, child_height_request),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__OBJECT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_INT);
}

void
ags_automation_edit_box_init(AgsAutomationEditBox *automation_edit_box)
{
  g_object_set(automation_edit_box,
	       "homogeneous", FALSE,
	       NULL);

  gtk_box_set_spacing(automation_edit_box,
		      AGS_AUTOMATION_EDIT_BOX_DEFAULT_SPACING);

  automation_edit_box->automation_edit = NULL;
}

void
ags_automation_edit_box_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_automation_edit_box_parent_class)->finalize(gobject);
}

void
ags_automation_edit_box_notify_width_request_callback(GObject *gobject,
						      GParamSpec *pspec,
						      AgsAutomationEditBox *automation_edit_box)
{
  gint width_request;

  width_request = -1;
  
  g_object_get(gobject,
	       "width-request", &width_request,
	       NULL);
  
  ags_automation_edit_box_child_width_request(automation_edit_box,
					      AGS_AUTOMATION_EDIT(gobject),
					      width_request);
}

void
ags_automation_edit_box_notify_height_request_callback(GObject *gobject,
						       GParamSpec *pspec,
						       AgsAutomationEditBox *automation_edit_box)
{
  gint height_request;

  height_request = -1;
  
  g_object_get(gobject,
	       "height-request", &height_request,
	       NULL);
  
  ags_automation_edit_box_child_height_request(automation_edit_box,
					       AGS_AUTOMATION_EDIT(gobject),
					       height_request);
}

/**
 * ags_automation_edit_box_get_automation_edit:
 * @automation_edit_box: the #AgsAutomationEditBox
 * 
 * Get automation_edit.
 * 
 * Returns: the #GList-struct containing #AgsAutomationEdit
 * 
 * Since: 4.0.0
 */
GList*
ags_automation_edit_box_get_automation_edit(AgsAutomationEditBox *automation_edit_box)
{
  g_return_if_fail(AGS_IS_AUTOMATION_EDIT_BOX(automation_edit_box));

  return(g_list_copy(automation_edit_box->automation_edit));
}

/**
 * ags_automation_edit_box_add:
 * @automation_edit_box: the #AgsAutomationEditBox
 * @automation_edit: the #AgsAutomationEdit
 * 
 * Add @automation_edit to @automation_edit_box.
 * 
 * Since: 4.0.0
 */
void
ags_automation_edit_box_add(AgsAutomationEditBox *automation_edit_box,
			    AgsAutomationEdit *automation_edit)
{
  g_return_if_fail(AGS_IS_AUTOMATION_EDIT_BOX(automation_edit_box));
  g_return_if_fail(AGS_IS_AUTOMATION_EDIT(automation_edit));

  if(g_list_find(automation_edit_box->automation_edit, automation_edit) == NULL){
    automation_edit_box->automation_edit = g_list_prepend(automation_edit_box->automation_edit,
							  automation_edit);
    
    g_signal_connect(automation_edit, "notify::width-request",
		     G_CALLBACK(ags_automation_edit_box_notify_width_request_callback), automation_edit_box);
    
    g_signal_connect(automation_edit, "notify::height-request",
		     G_CALLBACK(ags_automation_edit_box_notify_height_request_callback), automation_edit_box);
    
    gtk_box_append(automation_edit_box,
		   automation_edit);
  }
}

/**
 * ags_automation_edit_box_remove:
 * @automation_edit_box: the #AgsAutomationEditBox
 * @automation_edit: the #AgsAutomationEdit
 * 
 * Remove automation_edit at @position of @automation_edit_box.
 * 
 * Since: 4.0.0
 */
void
ags_automation_edit_box_remove(AgsAutomationEditBox *automation_edit_box,
			       AgsAutomationEdit *automation_edit)
{
  g_return_if_fail(AGS_IS_AUTOMATION_EDIT_BOX(automation_edit_box));
  g_return_if_fail(AGS_IS_AUTOMATION_EDIT(automation_edit));

  if(g_list_find(automation_edit_box->automation_edit, automation_edit) != NULL){
    g_object_disconnect(automation_edit,
			"any_signal::notify::width-request",
			G_CALLBACK(ags_automation_edit_box_notify_width_request_callback),
			automation_edit_box,
			"any_signal::notify::height-request",
			G_CALLBACK(ags_automation_edit_box_notify_height_request_callback),
			automation_edit_box,
			NULL);
    
    automation_edit_box->automation_edit = g_list_remove(automation_edit_box->automation_edit,
							 automation_edit);

    gtk_box_remove(automation_edit_box,
		   automation_edit);
  }
}

/**
 * ags_automation_edit_box_child_width_request:
 * @automation_edit_box: the #AgsAutomationEditBox
 * @automation_edit: the #AgsAutomationEdit
 * @width_request: the automation_edit's width-request
 * 
 * Notify about child automation_edit width request.
 * 
 * Since: 4.0.0
 */
void
ags_automation_edit_box_child_width_request(AgsAutomationEditBox *automation_edit_box,
					    GtkWidget *automation_edit,
					    gint width_request)
{
  g_return_if_fail(AGS_IS_AUTOMATION_EDIT_BOX(automation_edit_box));

  g_object_ref((GObject *) automation_edit_box);
  g_signal_emit(G_OBJECT(automation_edit_box),
		automation_edit_box_signals[CHILD_WIDTH_REQUEST], 0,
		automation_edit, width_request);
  g_object_unref((GObject *) automation_edit_box);
}

/**
 * ags_automation_edit_box_child_height_request:
 * @automation_edit_box: the #AgsAutomationEditBox
 * @automation_edit: the #AgsAutomationEdit
 * @height_request: the automation_edit's height-request
 * 
 * Notify about child automation_edit height request.
 * 
 * Since: 4.0.0
 */
void
ags_automation_edit_box_child_height_request(AgsAutomationEditBox *automation_edit_box,
					     GtkWidget *automation_edit,
					     gint height_request)
{
  g_return_if_fail(AGS_IS_AUTOMATION_EDIT_BOX(automation_edit_box));

  g_object_ref((GObject *) automation_edit_box);
  g_signal_emit(G_OBJECT(automation_edit_box),
		automation_edit_box_signals[CHILD_HEIGHT_REQUEST], 0,
		automation_edit, height_request);
  g_object_unref((GObject *) automation_edit_box);
}

/**
 * ags_automation_edit_box_new:
 * @orientation: the #GtkOrientation
 * 
 * Create a new instance of #AgsAutomationEditBox.
 * 
 * Returns: the new #AgsAutomationEditBox instance
 * 
 * Since: 3.0.0
 */
AgsAutomationEditBox*
ags_automation_edit_box_new(GtkOrientation orientation)
{
  AgsAutomationEditBox *automation_edit_box;

  automation_edit_box = (AgsAutomationEditBox *) g_object_new(AGS_TYPE_AUTOMATION_EDIT_BOX,
							      "orientation", orientation,
							      NULL);
  
  return(automation_edit_box);
}
