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

#ifndef __AGS_EXPORT_OUTPUT_H__
#define __AGS_EXPORT_OUTPUT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_export_thread.h>

#include <ags/X/ags_window.h>

#define AGS_TYPE_EXPORT_OUTPUT                (ags_export_output_get_type())
#define AGS_EXPORT_OUTPUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_OUTPUT, AgsExportOutput))
#define AGS_EXPORT_OUTPUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EXPORT_OUTPUT, AgsExportOutputClass))
#define AGS_IS_EXPORT_OUTPUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EXPORT_OUTPUT))
#define AGS_IS_EXPORT_OUTPUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EXPORT_OUTPUT))
#define AGS_EXPORT_OUTPUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EXPORT_OUTPUT, AgsExportOutputClass))

typedef struct _AgsExportOutput AgsExportOutput;
typedef struct _AgsExportOutputClass AgsExportOutputClass;

struct _AgsExportOutput
{
  AgsTask task;

  AgsExportThread *export_thread;
  AgsDevout *devout;
  gchar *filename;
  gboolean live_performance;
};

struct _AgsExportOutputClass
{
  AgsTaskClass task;
};

GType ags_export_output_get_type();

AgsExportOutput* ags_export_output_new(AgsExportThread *export_thread,
				       AgsDevout *devout,
				       gchar *filename,
				       gboolean live_performance);

#endif /*__AGS_EXPORT_OUTPUT_H__*/
