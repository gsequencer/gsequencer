/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_OPEN_SINGLE_FILE_H__
#define __AGS_OPEN_SINGLE_FILE_H__

#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_OPEN_SINGLE_FILE                (ags_open_single_file_get_type())
#define AGS_OPEN_SINGLE_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OPEN_SINGLE_FILE, AgsOpenSingleFile))
#define AGS_OPEN_SINGLE_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OPEN_SINGLE_FILE, AgsOpenSingleFileClass))
#define AGS_IS_OPEN_SINGLE_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OPEN_SINGLE_FILE))
#define AGS_IS_OPEN_SINGLE_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OPEN_SINGLE_FILE))
#define AGS_OPEN_SINGLE_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OPEN_SINGLE_FILE, AgsOpenSingleFileClass))

typedef struct _AgsOpenSingleFile AgsOpenSingleFile;
typedef struct _AgsOpenSingleFileClass AgsOpenSingleFileClass;

struct _AgsOpenSingleFile
{
  AgsTask task;

  AgsChannel *channel;
  AgsDevout *devout;

  gchar *filename;
  guint start_channel;
  guint audio_channels;
};

struct _AgsOpenSingleFileClass
{
  AgsTaskClass task;
};

GType ags_open_single_file_get_type();

AgsOpenSingleFile* ags_open_single_file_new(AgsChannel *channel,
					    AgsDevout *devout,
					    gchar *filename,
					    guint start_channel,
					    guint audio_channels);

#endif /*__AGS_OPEN_SINGLE_FILE_H__*/
