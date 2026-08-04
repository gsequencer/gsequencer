/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_GSTREAMER_PIPELINE_MANAGER_H__
#define __AGS_GSTREAMER_PIPELINE_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_PIPELINE_MANAGER                (ags_gstreamer_pipeline_manager_get_type())
#define AGS_GSTREAMER_PIPELINE_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_PIPELINE_MANAGER, AgsGstreamerPipelineManager))
#define AGS_GSTREAMER_PIPELINE_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GSTREAMER_PIPELINE_MANAGER, AgsGstreamerPipelineManagerClass))
#define AGS_IS_GSTREAMER_PIPELINE_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GSTREAMER_PIPELINE_MANAGER))
#define AGS_IS_GSTREAMER_PIPELINE_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GSTREAMER_PIPELINE_MANAGER))
#define AGS_GSTREAMER_PIPELINE_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GSTREAMER_PIPELINE_MANAGER, AgsGstreamerPipelineManagerClass))

#define AGS_GSTREAMER_PIPELINE_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerPipelineManager *) obj)->obj_mutex))

typedef struct _AgsGstreamerPipelineManager AgsGstreamerPipelineManager;
typedef struct _AgsGstreamerPipelineManagerClass AgsGstreamerPipelineManagerClass;

struct _AgsGstreamerPipelineManager
{
  GObject gobject;
  
  GRecMutex obj_mutex;

  GList *gstreamer_pipeline;
};

struct _AgsGstreamerPipelineManagerClass
{
  GObjectClass gobject;
};

GType ags_gstreamer_pipeline_manager_get_type();

GRecMutex* ags_gstreamer_pipeline_manager_get_obj_mutex(AgsGstreamerPipelineManager *gstreamer_pipeline_manager);

void ags_gstreamer_pipeline_manager_create_ro_pipeline(AgsGstreamerPipelineManager *gstreamer_pipeline_manager,
						       GObject *gstreamer_file);
void ags_gstreamer_pipeline_manager_create_rw_pipeline(AgsGstreamerPipelineManager *gstreamer_pipeline_manager,
						       GObject *gstreamer_file);

/* instance */
AgsGstreamerPipelineManager* ags_gstreamer_pipeline_manager_get_instance();

AgsGstreamerPipelineManager* ags_gstreamer_pipeline_manager_new();

G_END_DECLS

#endif /*__AGS_GSTREAMER_PIPELINE_MANAGER_H__*/
