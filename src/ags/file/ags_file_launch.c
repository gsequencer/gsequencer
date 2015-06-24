/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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
  PROP_NODE,
  PROP_FILE,
  PROP_MAIN,
};

static gpointer ags_file_launch_parent_class = NULL;
static guint file_launch_signals[LAST_SIGNAL];

GType
ags_file_launch_get_type (void)
{
  static GType ags_type_file_launch = 0;

  if(!ags_type_file_launch){
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
						  "AgsFileLaunch\0",
						  &ags_file_launch_info,
						  0);
  }

  return (ags_type_file_launch);
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
  param_spec = g_param_spec_pointer("node\0",
				    "the node\0",
				    "The node to find the element\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NODE,
				  param_spec);

  param_spec = g_param_spec_object("file\0",
				   "file assigned to\0",
				   "The entire file assigned to\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  param_spec = g_param_spec_object("main\0",
				   "main access\0",
				   "The main object to access the tree\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN,
				  param_spec);

  /* AgsFileLaunchClass */
  file_launch->start = NULL;

  file_launch_signals[START] =
    g_signal_new("start\0",
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
  file_launch->ags_main = NULL;
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

  file_launch = AGS_FILE_LAUNCH(gobject);
  
  switch(prop_id){
  case PROP_NODE:
    {
      xmlNode *node;

      node = (xmlNode *) g_value_get_pointer(value);

      file_launch->node = node;
    }
    break;
  case PROP_FILE:
    {
      GObject *file;

      file = (GObject *) g_value_get_object(value);

      if(file_launch->file != NULL)
	g_object_unref(file_launch->file);

      if(file != NULL)
	g_object_ref(file);

      file_launch->file = file;
    }
    break;
  case PROP_MAIN:
    {
      GObject *ags_main;

      ags_main = (GObject *) g_value_get_object(value);

      if(file_launch->ags_main != NULL)
	g_object_unref(file_launch->ags_main);

      if(ags_main != NULL)
	g_object_ref(ags_main);

      file_launch->ags_main = ags_main;
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

  file_launch = AGS_FILE_LAUNCH(gobject);
  
  switch(prop_id){
  case PROP_NODE:
    {
      g_value_set_pointer(value, file_launch->node);
    }
    break;
  case PROP_FILE:
    {
      g_value_set_object(value, file_launch->file);
    }
    break;
  case PROP_MAIN:
    {
      g_value_set_object(value, file_launch->ags_main);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_launch_start(AgsFileLaunch *file_launch)
{
  g_return_if_fail(AGS_IS_FILE_LAUNCH(file_launch));

  g_object_ref((GObject *) file_launch);
  g_signal_emit(G_OBJECT(file_launch),
		file_launch_signals[START], 0);
  g_object_unref((GObject *) file_launch);
}

void
ags_file_launch_finalize(GObject *gobject)
{
  AgsFileLaunch *file_launch;

  file_launch = AGS_FILE_LAUNCH(gobject);

  if(file_launch->file != NULL){
    g_object_unref(file_launch->file);
  }

  if(file_launch->ags_main != NULL){
    g_object_unref(file_launch->ags_main);
  }

  G_OBJECT_CLASS(ags_file_launch_parent_class)->finalize(gobject);
}

/**
 * ags_file_launch_new:
 *
 * Creates an #AgsFileLaunch
 *
 * Returns: a new #AgsFileLaunch
 *
 * Since: 0.4
 */
AgsFileLaunch*
ags_file_launch_new()
{
  AgsFileLaunch *file_launch;

  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       NULL);

  return(file_launch);
}
