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

enum{
  START,
  LAST_SIGNAL,
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

  ags_file_launch_parent_class = g_type_class_peek_parent(file_launch);

  /* GObjectClass */
  gobject = (GObjectClass *) file_launch;

  gobject->get_property = ags_file_launch_get_property;
  gobject->set_property = ags_file_launch_set_property;

  gobject->finalize = ags_file_launch_finalize;

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
  file_launch->data = NULL;
}

void
ags_file_launch_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_file_launch_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  //TODO:JK: implement me
}

void
ags_file_launch_finalize(GObject *gobject)
{
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

AgsFileLaunch*
ags_file_launch_new()
{
  AgsFileLaunch *file_launch;

  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       NULL);

  return(file_launch);
}
