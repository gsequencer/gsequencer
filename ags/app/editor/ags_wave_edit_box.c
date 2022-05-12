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

#include <ags/app/editor/ags_wave_edit_box.h>

#include <ags/app/editor/ags_wave_edit.h>

#include <ags/i18n.h>

void ags_wave_edit_box_class_init(AgsWaveEditBoxClass *wave_edit_box);
void ags_wave_edit_box_init(AgsWaveEditBox *wave_edit_box);
void ags_wave_edit_box_dispose(GObject *gobject);
void ags_wave_edit_box_finalize(GObject *gobject);

void ags_wave_edit_box_notify_width_request_callback(GObject *gobject,
						     GParamSpec *pspec,
						     AgsWaveEditBox *wave_edit_box);
void ags_wave_edit_box_notify_height_request_callback(GObject *gobject,
						      GParamSpec *pspec,
						      AgsWaveEditBox *wave_edit_box);

/**
 * SECTION:ags_wave_edit_box
 * @short_description: abstract box widget
 * @title: AgsWaveEditBox
 * @section_id:
 * @include: ags/widget/ags_wave_edit_box.h
 *
 * The #AgsWaveEditBox is an abstract box widget containing #AgsWaveEdit.
 */

enum{
  CHILD_WIDTH_REQUEST,
  CHILD_HEIGHT_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_wave_edit_box_parent_class = NULL;
static guint wave_edit_box_signals[LAST_SIGNAL];

GType
ags_wave_edit_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave_edit_box = 0;

    static const GTypeInfo ags_wave_edit_box_info = {
      sizeof (AgsWaveEditBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_edit_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWaveEditBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_edit_box_init,
    };

    ags_type_wave_edit_box = g_type_register_static(GTK_TYPE_BOX,
						    "AgsWaveEditBox", &ags_wave_edit_box_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave_edit_box);
  }

  return g_define_type_id__volatile;
}

void
ags_wave_edit_box_class_init(AgsWaveEditBoxClass *wave_edit_box)
{
  GObjectClass *gobject;

  ags_wave_edit_box_parent_class = g_type_class_peek_parent(wave_edit_box);

  /* GObjectClass */
  gobject = (GObjectClass *) wave_edit_box;

  gobject->dispose = ags_wave_edit_box_dispose;
  gobject->finalize = ags_wave_edit_box_finalize;

  /* AgsWaveEditBox */
  wave_edit_box->child_width_request = NULL;
  wave_edit_box->child_height_request = NULL;
  
  /**
   * AgsWaveEditBox::child-width-request:
   * @wave_edit_box: the #AgsWaveEditBox.
   *
   * The ::child-width-request
   *
   * Since: 4.0.0
   */
  wave_edit_box_signals[CHILD_WIDTH_REQUEST] = 
    g_signal_new("child-width-request",
		 G_TYPE_FROM_CLASS(wave_edit_box),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsWaveEditBoxClass, child_width_request),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__OBJECT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_INT);

  /**
   * AgsWaveEditBox::child-height-request:
   * @wave_edit_box: the #AgsWaveEditBox.
   *
   * The ::child-height-request
   *
   * Since: 4.0.0
   */
  wave_edit_box_signals[CHILD_HEIGHT_REQUEST] = 
    g_signal_new("child-height-request",
		 G_TYPE_FROM_CLASS(wave_edit_box),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsWaveEditBoxClass, child_height_request),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__OBJECT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_INT);
}

void
ags_wave_edit_box_init(AgsWaveEditBox *wave_edit_box)
{
  g_object_set(wave_edit_box,
	       "homogeneous", FALSE,
	       "spacing", AGS_WAVE_EDIT_BOX_DEFAULT_SPACING,
	       NULL);
  
  wave_edit_box->wave_edit = NULL;
}

void
ags_wave_edit_box_dispose(GObject *gobject)
{
  AgsWaveEditBox *wave_edit_box;

  wave_edit_box = AGS_WAVE_EDIT_BOX(gobject);
  
  g_list_free(wave_edit_box->wave_edit);

  wave_edit_box->wave_edit = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_wave_edit_box_parent_class)->dispose(gobject);
}

void
ags_wave_edit_box_finalize(GObject *gobject)
{
  AgsWaveEditBox *wave_edit_box;

  wave_edit_box = AGS_WAVE_EDIT_BOX(gobject);
  
  g_list_free(wave_edit_box->wave_edit);

  /* call parent */
  G_OBJECT_CLASS(ags_wave_edit_box_parent_class)->finalize(gobject);
}

void
ags_wave_edit_box_notify_width_request_callback(GObject *gobject,
						GParamSpec *pspec,
						AgsWaveEditBox *wave_edit_box)
{
  gint width_request;

  width_request = -1;
  
  g_object_get(gobject,
	       "width-request", &width_request,
	       NULL);
  
  ags_wave_edit_box_child_width_request(wave_edit_box,
					AGS_WAVE_EDIT(gobject),
					width_request);
}

void
ags_wave_edit_box_notify_height_request_callback(GObject *gobject,
						 GParamSpec *pspec,
						 AgsWaveEditBox *wave_edit_box)
{
  gint height_request;

  height_request = -1;
  
  g_object_get(gobject,
	       "height-request", &height_request,
	       NULL);
  
  ags_wave_edit_box_child_height_request(wave_edit_box,
					 AGS_WAVE_EDIT(gobject),
					 height_request);
}

/**
 * ags_wave_edit_box_get_wave_edit:
 * @wave_edit_box: the #AgsWaveEditBox
 * 
 * Get wave_edit.
 * 
 * Returns: the #GList-struct containing #AgsWaveEdit
 * 
 * Since: 4.0.0
 */
GList*
ags_wave_edit_box_get_wave_edit(AgsWaveEditBox *wave_edit_box)
{
  g_return_val_if_fail(AGS_IS_WAVE_EDIT_BOX(wave_edit_box), NULL);

  return(g_list_reverse(g_list_copy(wave_edit_box->wave_edit)));
}

/**
 * ags_wave_edit_box_add_wave_edit:
 * @wave_edit_box: the #AgsWaveEditBox
 * @wave_edit: the #AgsWaveEdit
 * 
 * Add @wave_edit to @wave_edit_box.
 * 
 * Since: 4.0.0
 */
void
ags_wave_edit_box_add_wave_edit(AgsWaveEditBox *wave_edit_box,
				AgsWaveEdit *wave_edit)
{
  g_return_if_fail(AGS_IS_WAVE_EDIT_BOX(wave_edit_box));
  g_return_if_fail(AGS_IS_WAVE_EDIT(wave_edit));

  if(g_list_find(wave_edit_box->wave_edit, wave_edit) == NULL){
    wave_edit_box->wave_edit = g_list_prepend(wave_edit_box->wave_edit,
					      wave_edit);
    
    g_signal_connect(wave_edit, "notify::width-request",
		     G_CALLBACK(ags_wave_edit_box_notify_width_request_callback), wave_edit_box);
    
    g_signal_connect(wave_edit, "notify::height-request",
		     G_CALLBACK(ags_wave_edit_box_notify_height_request_callback), wave_edit_box);
    
    gtk_box_append(wave_edit_box,
		   wave_edit);
  }
}

/**
 * ags_wave_edit_box_remove_wave_edit:
 * @wave_edit_box: the #AgsWaveEditBox
 * @wave_edit: the #AgsWaveEdit
 * 
 * Remove wave_edit at @position of @wave_edit_box.
 * 
 * Since: 4.0.0
 */
void
ags_wave_edit_box_remove_wave_edit(AgsWaveEditBox *wave_edit_box,
				   AgsWaveEdit *wave_edit)
{
  g_return_if_fail(AGS_IS_WAVE_EDIT_BOX(wave_edit_box));
  g_return_if_fail(AGS_IS_WAVE_EDIT(wave_edit));

  if(g_list_find(wave_edit_box->wave_edit, wave_edit) != NULL){
    g_object_disconnect(wave_edit,
			"any_signal::notify::width-request",
			G_CALLBACK(ags_wave_edit_box_notify_width_request_callback),
			wave_edit_box,
			"any_signal::notify::height-request",
			G_CALLBACK(ags_wave_edit_box_notify_height_request_callback),
			wave_edit_box,
			NULL);
    
    wave_edit_box->wave_edit = g_list_remove(wave_edit_box->wave_edit,
					     wave_edit);

    gtk_box_remove(wave_edit_box,
		   wave_edit);
  }
}

/**
 * ags_wave_edit_box_child_width_request:
 * @wave_edit_box: the #AgsWaveEditBox
 * @wave_edit: the #AgsWaveEdit
 * @width_request: the wave_edit's width-request
 * 
 * Notify about child wave_edit width request.
 * 
 * Since: 4.0.0
 */
void
ags_wave_edit_box_child_width_request(AgsWaveEditBox *wave_edit_box,
				      GtkWidget *wave_edit,
				      gint width_request)
{
  g_return_if_fail(AGS_IS_WAVE_EDIT_BOX(wave_edit_box));

  g_object_ref((GObject *) wave_edit_box);
  g_signal_emit(G_OBJECT(wave_edit_box),
		wave_edit_box_signals[CHILD_WIDTH_REQUEST], 0,
		wave_edit, width_request);
  g_object_unref((GObject *) wave_edit_box);
}

/**
 * ags_wave_edit_box_child_height_request:
 * @wave_edit_box: the #AgsWaveEditBox
 * @wave_edit: the #AgsWaveEdit
 * @height_request: the wave_edit's height-request
 * 
 * Notify about child wave_edit height request.
 * 
 * Since: 4.0.0
 */
void
ags_wave_edit_box_child_height_request(AgsWaveEditBox *wave_edit_box,
				       GtkWidget *wave_edit,
				       gint height_request)
{
  g_return_if_fail(AGS_IS_WAVE_EDIT_BOX(wave_edit_box));

  g_object_ref((GObject *) wave_edit_box);
  g_signal_emit(G_OBJECT(wave_edit_box),
		wave_edit_box_signals[CHILD_HEIGHT_REQUEST], 0,
		wave_edit, height_request);
  g_object_unref((GObject *) wave_edit_box);
}

/**
 * ags_wave_edit_box_new:
 * @orientation: the #GtkOrientation
 * 
 * Create a new instance of #AgsWaveEditBox.
 * 
 * Returns: the new #AgsWaveEditBox instance
 * 
 * Since: 3.0.0
 */
AgsWaveEditBox*
ags_wave_edit_box_new(GtkOrientation orientation)
{
  AgsWaveEditBox *wave_edit_box;

  wave_edit_box = (AgsWaveEditBox *) g_object_new(AGS_TYPE_WAVE_EDIT_BOX,
						  "orientation", orientation,
						  NULL);
  
  return(wave_edit_box);
}
