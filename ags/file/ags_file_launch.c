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

#include <ags/file/ags_file_launch.h>

#include <ags/i18n.h>

void ags_file_launch_class_init(AgsFileLaunchClass *file_launch);
void ags_file_launch_init (AgsFileLaunch *file_launch);
void ags_file_launch_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_file_launch_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_file_launch_finalize(GObject *gobject);

/**
 * SECTION:ags_file_launch
 * @short_description: launch objects read of file.
 * @title: AgsFileLaunch
 * @section_id:
 * @include: ags/file/ags_file_launch.h
 *
 * The #AgsFileLaunch launches read objects of file.
 */

enum{
  START,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_REFERENCE,
  PROP_NODE,
  PROP_FILE,
};

static gpointer ags_file_launch_parent_class = NULL;
static guint file_launch_signals[LAST_SIGNAL];

GType
ags_file_launch_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_file_launch = 0;

    static const GTypeInfo ags_file_launch_info = {
      sizeof (AgsFileLaunchClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_launch_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileLaunch),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_launch_init,
    };

    ags_type_file_launch = g_type_register_static(G_TYPE_OBJECT,
						  "AgsFileLaunch",
						  &ags_file_launch_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_file_launch);
  }

  return g_define_type_id__volatile;
}

void
ags_file_launch_class_init(AgsFileLaunchClass *file_launch)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_file_launch_parent_class = g_type_class_peek_parent(file_launch);

  /* GObjectClass */
  gobject = (GObjectClass *) file_launch;

  gobject->set_property = ags_file_launch_set_property;
  gobject->get_property = ags_file_launch_get_property;

  gobject->finalize = ags_file_launch_finalize;

  /* properties */
  /**
   * AgsFileLaunch:node:
   *
   * The assigned xmlNode being referred by this #AgsFileLaunch.
   *
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("node",
				    i18n_pspec("the node"),
				    i18n_pspec("The node to find the element"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NODE,
				  param_spec);

  /**
   * AgsFileLaunch:reference:
   *
   * The object referred by this #AgsFileLaunch.
   *
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("reference",
				    i18n_pspec("the reference"),
				    i18n_pspec("The reference to find the element"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REFERENCE,
				  param_spec);

  /**
   * AgsFileLaunch:file:
   *
   * The #AgsFile this #AgsFileLaunch belongs to.
   *
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("file",
				   i18n_pspec("file assigned to"),
				   i18n_pspec("The entire file assigned to"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  /* AgsFileLaunchClass */
  file_launch->start = NULL;

  /**
   * AgsFileLaunch::start:
   * @file_id_ref: the #AgsFileLaunch
   * 
   * Signal ::start to notify about start :reference.
   *
   * Since: 2.0.0
   */
  file_launch_signals[START] =
    g_signal_new("start",
		 G_TYPE_FROM_CLASS(file_launch),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileLaunchClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_file_launch_init(AgsFileLaunch *file_launch)
{
  /* file launch mutex */
  g_rec_mutex_init(&(file_launch->obj_mutex));

  file_launch->reference = NULL;
  
  file_launch->node = NULL;
  file_launch->file = NULL;
}

void
ags_file_launch_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileLaunch *file_launch;

  GRecMutex *file_launch_mutex;

  file_launch = AGS_FILE_LAUNCH(gobject);

  /* get file id ref mutex */
  file_launch_mutex = AGS_FILE_LAUNCH_GET_OBJ_MUTEX(file_launch);
  
  switch(prop_id){
  case PROP_NODE:
    {
      xmlNode *node;

      node = (xmlNode *) g_value_get_pointer(value);

      g_rec_mutex_lock(file_launch_mutex);

      file_launch->node = node;

      g_rec_mutex_unlock(file_launch_mutex);
    }
    break;
  case PROP_REFERENCE:
    {
      gpointer ref;

      ref = g_value_get_pointer(value);

      g_rec_mutex_lock(file_launch_mutex);

      file_launch->reference = ref;

      g_rec_mutex_unlock(file_launch_mutex);
    }
    break;
  case PROP_FILE:
    {
      GObject *file;

      file = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(file_launch_mutex);

      if(file_launch->file != NULL){
	g_object_unref(file_launch->file);
      }
      
      if(file != NULL){
	g_object_ref(file);
      }
      
      file_launch->file = file;

      g_rec_mutex_unlock(file_launch_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_launch_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileLaunch *file_launch;

  GRecMutex *file_launch_mutex;

  file_launch = AGS_FILE_LAUNCH(gobject);

  /* get file id ref mutex */
  file_launch_mutex = AGS_FILE_LAUNCH_GET_OBJ_MUTEX(file_launch);
    
  switch(prop_id){
  case PROP_NODE:
    {
      g_rec_mutex_lock(file_launch_mutex);

      g_value_set_pointer(value, file_launch->node);

      g_rec_mutex_unlock(file_launch_mutex);
    }
    break;
  case PROP_REFERENCE:
    {
      g_rec_mutex_lock(file_launch_mutex);

      g_value_set_pointer(value, file_launch->reference);

      g_rec_mutex_unlock(file_launch_mutex);
    }
    break;
  case PROP_FILE:
    {
      g_rec_mutex_lock(file_launch_mutex);

      g_value_set_object(value, file_launch->file);

      g_rec_mutex_unlock(file_launch_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_launch_finalize(GObject *gobject)
{
  AgsFileLaunch *file_launch;

  file_launch = AGS_FILE_LAUNCH(gobject);

  if(file_launch->file != NULL){
    g_object_unref(file_launch->file);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_file_launch_parent_class)->finalize(gobject);
}

/**
 * ags_file_launch_start:
 * @file_launch: the #AgsFileLaunch
 * 
 * Start #AgsFileLaunch to fulfill a task.
 * 
 * Since: 2.0.0 
 */
void
ags_file_launch_start(AgsFileLaunch *file_launch)
{
  g_return_if_fail(AGS_IS_FILE_LAUNCH(file_launch));

  g_object_ref((GObject *) file_launch);
  g_signal_emit(G_OBJECT(file_launch),
		file_launch_signals[START], 0);
  g_object_unref((GObject *) file_launch);
}

/**
 * ags_file_launch_new:
 *
 * Create a new instance of #AgsFileLaunch
 *
 * Returns: the new #AgsFileLaunch
 *
 * Since: 2.0.0
 */
AgsFileLaunch*
ags_file_launch_new()
{
  AgsFileLaunch *file_launch;

  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       NULL);

  return(file_launch);
}
