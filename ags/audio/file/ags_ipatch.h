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

#ifndef __AGS_IPATCH_H__
#define __AGS_IPATCH_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/config.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <ags/object/ags_soundcard.h>

#define AGS_TYPE_IPATCH                (ags_ipatch_get_type())
#define AGS_IPATCH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_IPATCH, AgsIpatch))
#define AGS_IPATCH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_IPATCH, AgsIpatchClass))
#define AGS_IS_IPATCH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_IPATCH))
#define AGS_IS_IPATCH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_IPATCH))
#define AGS_IPATCH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_IPATCH, AgsIpatchClass))

#define AGS_IPATCH_DEFAULT_CHANNELS 2

#define AGS_IPATCH_READ "r"
#define AGS_IPATCH_WRITE "w"

typedef struct _AgsIpatch AgsIpatch;
typedef struct _AgsIpatchClass AgsIpatchClass;

typedef enum{
  AGS_IPATCH_DLS2   = 1,
  AGS_IPATCH_SF2    = 1 << 1,
  AGS_IPATCH_GIG    = 1 << 2,
}AgsIpatchFlags;

struct _AgsIpatch
{
  GObject object;

  guint flags;

  GObject *soundcard;
  GList *audio_signal;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchFile *file;
#else
  gpointer file;
#endif
  
  char *filename;
  char *mode;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchFileHandle *handle;
#else
  gpointer handle;
#endif
  
  GError *error;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchBase *base;
#else
  gpointer base;
#endif
  
  GObject *reader;

#ifdef AGS_WITH_LIBINSTPATCH
  IpatchList *samples;
#else
  gpointer samples;
#endif

  GList *iter;

  guint nth_level;
};

struct _AgsIpatchClass
{
  GObjectClass object;
};

GType ags_ipatch_get_type();

gboolean ags_ipatch_check_suffix(gchar *filename);

AgsIpatch* ags_ipatch_new();

#endif /*__AGS_IPATCH_H__*/
