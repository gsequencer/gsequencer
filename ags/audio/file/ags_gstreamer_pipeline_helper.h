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

#ifndef __AGS_GSTREAMER_PIPELINE_HELPER_H__
#define __AGS_GSTREAMER_PIPELINE_HELPER_H__

#include <glib.h>
#include <glib-object.h>

#include <gst/gst.h>

#include <gst/pbutils/pbutils.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_gstreamer_file.h>

G_BEGIN_DECLS

GObject* ags_gstreamer_pipeline_helper_create_ro_pipeline(AgsGstreamerFile *gstreamer_file);
GObject* ags_gstreamer_pipeline_helper_create_rw_pipeline(AgsGstreamerFile *gstreamer_file);

G_END_DECLS

#endif /*__AGS_GSTREAMER_PIPELINE_HELPER_H__*/
