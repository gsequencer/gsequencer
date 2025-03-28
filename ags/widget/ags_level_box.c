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

#include <ags/widget/ags_level_box.h>
#include <ags/widget/ags_level.h>
#include <ags/widget/ags_widget_marshal.h>

#include <ags/i18n.h>

void ags_level_box_class_init(AgsLevelBoxClass *level_box);
void ags_level_box_init(AgsLevelBox *level_box);
void ags_level_box_dispose(GObject *gobject);
void ags_level_box_finalize(GObject *gobject);

void ags_level_box_notify_width_request_callback(GObject *gobject,
						 GParamSpec *pspec,
						 AgsLevelBox *level_box);
void ags_level_box_notify_height_request_callback(GObject *gobject,
						  GParamSpec *pspec,
						  AgsLevelBox *level_box);

/**
 * SECTION:ags_level_box
 * @short_description: box widget
 * @title: AgsLevelBox
 * @section_id:
 * @include: ags/widget/ags_level_box.h
 *
 * The #AgsLevelBox is a box widget containing #AgsLevel.
 */

enum{
  CHILD_WIDTH_REQUEST,
  CHILD_HEIGHT_REQUEST,
  LAST_SIGNAL,
};

static gpointer ags_level_box_parent_class = NULL;
static guint level_box_signals[LAST_SIGNAL];

GType
ags_level_box_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_level_box);
  }

  return(g_define_type_id__static);
}

void
ags_level_box_class_init(AgsLevelBoxClass *level_box)
{
  GObjectClass *gobject;

  ags_level_box_parent_class = g_type_class_peek_parent(level_box);

  /* GObjectClass */
  gobject = (GObjectClass *) level_box;

  gobject->dispose = ags_level_box_dispose;
  gobject->finalize = ags_level_box_finalize;

  /* AgsLevelBox */
  level_box->child_width_request = NULL;
  level_box->child_height_request = NULL;
  
  /**
   * AgsLevelBox::child-width-request:
   * @level_box: the #AgsLevelBox.
   * @level: the #AgsLevel
   * @width_request: the level's width-request
   *
   * The ::child-width-request
   *
   * Since: 4.0.0
   */
  level_box_signals[CHILD_WIDTH_REQUEST] = 
    g_signal_new("child-width-request",
		 G_TYPE_FROM_CLASS(level_box),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLevelBoxClass, child_width_request),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__OBJECT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_INT);

  /**
   * AgsLevelBox::child-height-request:
   * @level_box: the #AgsLevelBox.
   * @level: the #AgsLevel
   * @height_request: the level's height-request
   *
   * The ::child-height-request
   *
   * Since: 4.0.0
   */
  level_box_signals[CHILD_HEIGHT_REQUEST] = 
    g_signal_new("child-height-request",
		 G_TYPE_FROM_CLASS(level_box),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLevelBoxClass, child_height_request),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__OBJECT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_OBJECT, G_TYPE_INT);
}

void
ags_level_box_init(AgsLevelBox *level_box)
{
  g_object_set(level_box,
	       "homogeneous", FALSE,
	       "spacing", AGS_LEVEL_BOX_DEFAULT_SPACING,
	       NULL);
  
  level_box->level = NULL;
}

void
ags_level_box_dispose(GObject *gobject)
{
  AgsLevelBox *level_box;

  level_box = AGS_LEVEL_BOX(gobject);
  
  g_list_free(level_box->level);

  level_box->level = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_level_box_parent_class)->dispose(gobject);
}

void
ags_level_box_finalize(GObject *gobject)
{
  AgsLevelBox *level_box;

  level_box = AGS_LEVEL_BOX(gobject);
  
  g_list_free(level_box->level);

  /* call parent */
  G_OBJECT_CLASS(ags_level_box_parent_class)->finalize(gobject);
}

void
ags_level_box_notify_width_request_callback(GObject *gobject,
					    GParamSpec *pspec,
					    AgsLevelBox *level_box)
{
  gint width_request;

  width_request = -1;
  
  g_object_get(gobject,
	       "width-request", &width_request,
	       NULL);
  
  ags_level_box_child_width_request(level_box,
				    GTK_WIDGET(gobject),
				    width_request);
}

void
ags_level_box_notify_height_request_callback(GObject *gobject,
					     GParamSpec *pspec,
					     AgsLevelBox *level_box)
{
  gint height_request;

  height_request = -1;
  
  g_object_get(gobject,
	       "height-request", &height_request,
	       NULL);
  
  ags_level_box_child_height_request(level_box,
				     GTK_WIDGET(gobject),
				     height_request);
}

/**
 * ags_level_box_get_level:
 * @level_box: the #AgsLevelBox
 * 
 * Get level.
 * 
 * Returns: (element-type AgsGui.Level) (transfer container): the #GList-struct containing #AgsLevel
 * 
 * Since: 4.0.0
 */
GList*
ags_level_box_get_level(AgsLevelBox *level_box)
{
  g_return_val_if_fail(AGS_IS_LEVEL_BOX(level_box), NULL);

  return(g_list_reverse(g_list_copy(level_box->level)));
}

/**
 * ags_level_box_add_level:
 * @level_box: the #AgsLevelBox
 * @level: the #AgsLevel
 * 
 * Add @level to @level_box.
 * 
 * Since: 4.0.0
 */
void
ags_level_box_add_level(AgsLevelBox *level_box,
			AgsLevel *level)
{
  g_return_if_fail(AGS_IS_LEVEL_BOX(level_box));
  g_return_if_fail(AGS_IS_LEVEL(level));

  if(g_list_find(level_box->level, level) == NULL){
    level_box->level = g_list_prepend(level_box->level,
				      level);
    
    g_signal_connect(level, "notify::width-request",
		     G_CALLBACK(ags_level_box_notify_width_request_callback), level_box);
    
    g_signal_connect(level, "notify::height-request",
		     G_CALLBACK(ags_level_box_notify_height_request_callback), level_box);
    
    gtk_box_append((GtkBox *) level_box,
		   (GtkWidget *) level);
  }else{
    g_warning("level already added to level box");
  }
}

/**
 * ags_level_box_remove_level:
 * @level_box: the #AgsLevelBox
 * @level: the #AgsLevel
 * 
 * Remove level at @position of @level_box.
 * 
 * Since: 4.0.0
 */
void
ags_level_box_remove_level(AgsLevelBox *level_box,
			   AgsLevel *level)
{  
  g_return_if_fail(AGS_IS_LEVEL_BOX(level_box));
  g_return_if_fail(AGS_IS_LEVEL(level));

  if(g_list_find(level_box->level, level) != NULL){
    g_object_disconnect(level,
			"any_signal::notify::width-request",
			G_CALLBACK(ags_level_box_notify_width_request_callback),
			level_box,
			"any_signal::notify::height-request",
			G_CALLBACK(ags_level_box_notify_height_request_callback),
			level_box,
			NULL);
    
    level_box->level = g_list_remove(level_box->level,
				     level);

    gtk_box_remove((GtkBox *) level_box,
		   (GtkWidget *) level);
  }else{
    g_warning("level not packed by level box");
  }
}

/**
 * ags_level_box_child_width_request:
 * @level_box: the #AgsLevelBox
 * @level: the #AgsLevel
 * @width_request: the level's width-request
 * 
 * Notify about child level width request.
 * 
 * Since: 4.0.0
 */
void
ags_level_box_child_width_request(AgsLevelBox *level_box,
				  GtkWidget *level,
				  gint width_request)
{
  g_return_if_fail(AGS_IS_LEVEL_BOX(level_box));

  g_object_ref((GObject *) level_box);
  g_signal_emit(G_OBJECT(level_box),
		level_box_signals[CHILD_WIDTH_REQUEST], 0,
		level, width_request);
  g_object_unref((GObject *) level_box);
}

/**
 * ags_level_box_child_height_request:
 * @level_box: the #AgsLevelBox
 * @level: the #AgsLevel
 * @height_request: the level's height-request
 * 
 * Notify about child level height request.
 * 
 * Since: 4.0.0
 */
void
ags_level_box_child_height_request(AgsLevelBox *level_box,
				   GtkWidget *level,
				   gint height_request)
{
  g_return_if_fail(AGS_IS_LEVEL_BOX(level_box));

  g_object_ref((GObject *) level_box);
  g_signal_emit(G_OBJECT(level_box),
		level_box_signals[CHILD_HEIGHT_REQUEST], 0,
		level, height_request);
  g_object_unref((GObject *) level_box);
}

/**
 * ags_level_box_new:
 * @orientation: the #GtkOrientation
 * 
 * Create a new instance of #AgsLevelBox.
 * 
 * Returns: the new #AgsLevelBox instance
 * 
 * Since: 3.0.0
 */
AgsLevelBox*
ags_level_box_new(GtkOrientation orientation)
{
  AgsLevelBox *level_box;

  level_box = (AgsLevelBox *) g_object_new(AGS_TYPE_LEVEL_BOX,
					   "orientation", orientation,
					   NULL);
  
  return(level_box);
}
