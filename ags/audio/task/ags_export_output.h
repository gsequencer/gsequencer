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

#ifndef __AGS_EXPORT_OUTPUT_H__
#define __AGS_EXPORT_OUTPUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/thread/ags_export_thread.h>

G_BEGIN_DECLS

#define AGS_TYPE_EXPORT_OUTPUT                (ags_export_output_get_type())
#define AGS_EXPORT_OUTPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_OUTPUT, AgsExportOutput))
#define AGS_EXPORT_OUTPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EXPORT_OUTPUT, AgsExportOutputClass))
#define AGS_IS_EXPORT_OUTPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EXPORT_OUTPUT))
#define AGS_IS_EXPORT_OUTPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EXPORT_OUTPUT))
#define AGS_EXPORT_OUTPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EXPORT_OUTPUT, AgsExportOutputClass))

typedef struct _AgsExportOutput AgsExportOutput;
typedef struct _AgsExportOutputClass AgsExportOutputClass;

/**
 * AgsExportOutputFormat:
 * @AGS_EXPORT_OUTPUT_FORMAT_WAV: use WAV format
 * @AGS_EXPORT_OUTPUT_FORMAT_OGG: use OGG format
 * @AGS_EXPORT_OUTPUT_FORMAT_FLAC: use FLAC format
 * @AGS_EXPORT_OUTPUT_FORMAT_AIFF: use AIFF format
 * 
 * Enum values to control output format of #AgsExportOutput.
 */
typedef enum{
  AGS_EXPORT_OUTPUT_FORMAT_WAV    =  1,
  AGS_EXPORT_OUTPUT_FORMAT_OGG    =  1 <<  1,
  AGS_EXPORT_OUTPUT_FORMAT_FLAC   =  1 <<  2,
  AGS_EXPORT_OUTPUT_FORMAT_AIFF   =  1 <<  3,
}AgsExportOutputFormat;

struct _AgsExportOutput
{
  AgsTask task;

  AgsExportThread *export_thread;
  GObject *soundcard;

  gchar *filename;
  AgsExportOutputFormat format;
  
  guint tic;
  gboolean live_performance;
};

struct _AgsExportOutputClass
{
  AgsTaskClass task;
};

GType ags_export_output_get_type();

AgsExportOutput* ags_export_output_new(AgsExportThread *export_thread,
				       GObject *soundcard,
				       gchar *filename,
				       guint tic,
				       gboolean live_performance);
G_END_DECLS

#endif /*__AGS_EXPORT_OUTPUT_H__*/
