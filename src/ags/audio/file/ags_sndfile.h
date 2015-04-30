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

#ifndef __AGS_SNDFILE_H__
#define __AGS_SNDFILE_H__

#include <glib.h>
#include <glib-object.h>

#include <sndfile.h>

#define AGS_TYPE_SNDFILE                (ags_sndfile_get_type())
#define AGS_SNDFILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SNDFILE, AgsSndfile))
#define AGS_SNDFILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SNDFILE, AgsSndfileClass))
#define AGS_IS_SNDFILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SNDFILE))
#define AGS_IS_SNDFILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SNDFILE))
#define AGS_SNDFILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SNDFILE, AgsSndfileClass))

typedef struct _AgsSndfile AgsSndfile;
typedef struct _AgsSndfileClass AgsSndfileClass;

typedef enum{
  AGS_SNDFILE_ITER_START    = 1,
  AGS_SNDFILE_VIRTUAL       = 1 << 1,
}AgsSndfileFlags;

struct _AgsSndfile
{
  GObject object;

  guint flags;

  SF_INFO *info;
  SNDFILE *file;

  guchar *pointer;
  guchar *current;
  gsize length;
};

struct _AgsSndfileClass
{
  GObjectClass object;
};

GType ags_sndfile_get_type();

AgsSndfile* ags_sndfile_new();

#endif /*__AGS_SNDFILE_H__*/
