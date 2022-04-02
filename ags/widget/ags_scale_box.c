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

#include <ags/widget/ags_scale_box.h>
#include <ags/widget/ags_scale.h>
#include <ags/widget/ags_widget_marshal.h>

#include <ags/i18n.h>

void ags_scale_box_class_init(AgsScaleBoxClass *scale_box);
void ags_scale_box_init(AgsScaleBox *scale_box);
void ags_scale_box_dispose(GObject *gobject);
void ags_scale_box_finalize(GObject *gobject);

void ags_scale_box_notify_width_request_callback(GObject *gobject,
						 GParamSpec *pspec,
						 AgsScaleBox *scale_box);
void ags_scale_box_notify_height_request_callback(GObject *gobject,
						  GParamSpec *pspec,
						  AgsScaleBox *scale_box);

/**
 * SECTION:ags_scale_box
 * @short_description: box widget
 * @title: AgsScaleBox
 * @section_id:
 * @include: ags/widget/ags_scale_box.h
 *
 * The #AgsScaleBox is a box widget containing #AgsScale.
 */

enum{
  CHILD_WIDTH_REQUEST,
  CHILD_HEIGHT_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_scale_box_parent_class = NULL;
static guint scale_box_signals[LAST_SIGNAL];

GType
ags_scale_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scale_box = 0;

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

    ags_type_scale_box = g_type_register_static(GTK_TYPE_BOX,
						"AgsScaleBox", &ags_scale_box_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scale_box);
  }

  return g_define_type_id__volatile;
}

void
ags_scale_box_class_init(AgsScaleBoxClass *scale_box)
{
  GObjectClass *gobject;

  ags_scale_box_parent_class = g_type_class_peek_parent(scale_box);

  /* GObjectClass */
  gobject = (GObjectClass *) scale_box;

  gobject->dispose = ags_scale_box_dispose;
  gobject->finalize = ags_scale_box_finalize;

  /* AgsScaleBox */
  scale_box->child_width_request = NULL;
  scale_box->child_height_request = NULL;
  
  /**
   * AgsScaleBox::child-width-request:
   * @scale_box: the #AgsScaleBox.
   *
   * The ::child-width-request
   *
   * Since: 4.0.0
   */
  scale_box_signals[CHILD_WIDTH_REQUEST] = 
    g_signal_new("child-width-request",
		 G_TYPE_FROM_CLASS(scale_box),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScaleBoxClass, child_width_request),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__OBJECT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_INT);

  /**
   * AgsScaleBox::child-height-request:
   * @scale_box: the #AgsScaleBox.
   *
   * The ::child-height-request
   *
   * Since: 4.0.0
   */
  scale_box_signals[CHILD_HEIGHT_REQUEST] = 
    g_signal_new("child-height-request",
		 G_TYPE_FROM_CLASS(scale_box),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScaleBoxClass, child_height_request),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__OBJECT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_INT);
}

void
ags_scale_box_init(AgsScaleBox *scale_box)
{
  g_object_set(scale_box,
	       "homogeneous", FALSE,
	       "spacing", AGS_SCALE_BOX_DEFAULT_SPACING,
	       NULL);
  
  scale_box->scale = NULL;
}

void
ags_scale_box_dispose(GObject *gobject)
{
  AgsScaleBox *scale_box;

  scale_box = AGS_SCALE_BOX(gobject);
  
  g_list_free(scale_box->scale);

  scale_box->scale = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_scale_box_parent_class)->dispose(gobject);
}

void
ags_scale_box_finalize(GObject *gobject)
{
  AgsScaleBox *scale_box;

  scale_box = AGS_SCALE_BOX(gobject);
  
  g_list_free(scale_box->scale);

  /* call parent */
  G_OBJECT_CLASS(ags_scale_box_parent_class)->finalize(gobject);
}

void
ags_scale_box_notify_width_request_callback(GObject *gobject,
					    GParamSpec *pspec,
					    AgsScaleBox *scale_box)
{
  gint width_request;

  width_request = -1;
  
  g_object_get(gobject,
	       "width-request", &width_request,
	       NULL);
  
  ags_scale_box_child_width_request(scale_box,
				    AGS_SCALE(gobject),
				    width_request);
}

void
ags_scale_box_notify_height_request_callback(GObject *gobject,
					     GParamSpec *pspec,
					     AgsScaleBox *scale_box)
{
  gint height_request;

  height_request = -1;
  
  g_object_get(gobject,
	       "height-request", &height_request,
	       NULL);
  
  ags_scale_box_child_height_request(scale_box,
				     AGS_SCALE(gobject),
				     height_request);
}

/**
 * ags_scale_box_get_scale:
 * @scale_box: the #AgsScaleBox
 * 
 * Get scale.
 * 
 * Returns: the #GList-struct containing #AgsScale
 * 
 * Since: 4.0.0
 */
GList*
ags_scale_box_get_scale(AgsScaleBox *scale_box)
{
  g_return_if_fail(AGS_IS_SCALE_BOX(scale_box));

  return(g_list_reverse(g_list_copy(scale_box->scale)));
}

/**
 * ags_scale_box_add_scale:
 * @scale_box: the #AgsScaleBox
 * @scale: the #AgsScale
 * 
 * Add @scale to @scale_box.
 * 
 * Since: 4.0.0
 */
void
ags_scale_box_add_scale(AgsScaleBox *scale_box,
			AgsScale *scale)
{
  g_return_if_fail(AGS_IS_SCALE_BOX(scale_box));
  g_return_if_fail(AGS_IS_SCALE(scale));

  if(g_list_find(scale_box->scale, scale) == NULL){
    scale_box->scale = g_list_prepend(scale_box->scale,
				      scale);
    
    g_signal_connect(scale, "notify::width-request",
		     G_CALLBACK(ags_scale_box_notify_width_request_callback), scale_box);
    
    g_signal_connect(scale, "notify::height-request",
		     G_CALLBACK(ags_scale_box_notify_height_request_callback), scale_box);
    
    gtk_box_append(scale_box,
		   scale);
  }else{
    g_warning("scale already added to scale box");
  }
}

/**
 * ags_scale_box_remove_scale:
 * @scale_box: the #AgsScaleBox
 * @scale: the #AgsScale
 * 
 * Remove @scale from @scale_box.
 * 
 * Since: 4.0.0
 */
void
ags_scale_box_remove_scale(AgsScaleBox *scale_box,
			   AgsScale *scale)
{
  g_return_if_fail(AGS_IS_SCALE_BOX(scale_box));
  g_return_if_fail(AGS_IS_SCALE(scale));

  if(g_list_find(scale_box->scale, scale) != NULL){
    g_object_disconnect(scale,
			"any_signal::notify::width-request",
			G_CALLBACK(ags_scale_box_notify_width_request_callback),
			scale_box,
			"any_signal::notify::height-request",
			G_CALLBACK(ags_scale_box_notify_height_request_callback),
			scale_box,
			NULL);
    
    scale_box->scale = g_list_remove(scale_box->scale,
				     scale);

    gtk_box_remove(scale_box,
		   scale);
  }else{
    g_warning("scale not packed by scale box");
  }
}

/**
 * ags_scale_box_child_width_request:
 * @scale_box: the #AgsScaleBox
 * @scale: the #AgsScale
 * @width_request: the scale's width-request
 * 
 * Notify about child scale width request.
 * 
 * Since: 4.0.0
 */
void
ags_scale_box_child_width_request(AgsScaleBox *scale_box,
				  GtkWidget *scale,
				  gint width_request)
{
  g_return_if_fail(AGS_IS_SCALE_BOX(scale_box));

  g_object_ref((GObject *) scale_box);
  g_signal_emit(G_OBJECT(scale_box),
		scale_box_signals[CHILD_WIDTH_REQUEST], 0,
		scale, width_request);
  g_object_unref((GObject *) scale_box);
}

/**
 * ags_scale_box_child_height_request:
 * @scale_box: the #AgsScaleBox
 * @scale: the #AgsScale
 * @height_request: the scale's height-request
 * 
 * Notify about child scale height request.
 * 
 * Since: 4.0.0
 */
void
ags_scale_box_child_height_request(AgsScaleBox *scale_box,
				   GtkWidget *scale,
				   gint height_request)
{
  g_return_if_fail(AGS_IS_SCALE_BOX(scale_box));

  g_object_ref((GObject *) scale_box);
  g_signal_emit(G_OBJECT(scale_box),
		scale_box_signals[CHILD_HEIGHT_REQUEST], 0,
		scale, height_request);
  g_object_unref((GObject *) scale_box);
}

/**
 * ags_scale_box_new:
 * @orientation: the #GtkOrientation
 * 
 * Create a new instance of #AgsScaleBox.
 * 
 * Returns: the new #AgsScaleBox instance
 * 
 * Since: 3.0.0
 */
AgsScaleBox*
ags_scale_box_new(GtkOrientation orientation)
{
  AgsScaleBox *scale_box;

  scale_box = (AgsScaleBox *) g_object_new(AGS_TYPE_SCALE_BOX,
					   "orientation", orientation,
					   NULL);
  
  return(scale_box);
}
