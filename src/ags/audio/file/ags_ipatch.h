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

#ifndef __AGS_IPATCH_H__
#define __AGS_IPATCH_H__

#include <glib.h>
#include <glib-object.h>
#include <libinstpatch/libinstpatch.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_IPATCH                (ags_ipatch_get_type())
#define AGS_IPATCH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_IPATCH, AgsIpatch))
#define AGS_IPATCH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_IPATCH, AgsIpatchClass))
#define AGS_IS_IPATCH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_IPATCH))
#define AGS_IS_IPATCH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_IPATCH))
#define AGS_IPATCH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_IPATCH, AgsIpatchClass))

#define AGS_IPATCH_READ "r"
#define AGS_IPATCH_WRITE "w"

typedef struct _AgsIpatch AgsIpatch;
typedef struct _AgsIpatchClass AgsIpatchClass;

struct _AgsIpatch
{
  GObject object;

  AgsDevout *devout;
  GList *audio_signal;

  IpatchFile *file;
  char *filename;
  char *mode;

  IpatchFileHandle *handle;
  GError *error;

  IpatchContainer *container;
  GObject *reader;
};

struct _AgsIpatchClass
{
  GObjectClass object;
};

GType ags_ipatch_get_type();

void ags_ipatch_read_audio_signal(AgsIpatch *ipatch);

gboolean ags_iofuncs_open(IpatchFileHandle *handle, const char *mode, GError **err);
void ags_iofuncs_close(IpatchFileHandle *handle);
GIOStatus ags_iofuncs_read(IpatchFileHandle *handle, gpointer buf, guint size,
			   guint *bytes_read, GError **err);
GIOStatus ags_iofuncs_write(IpatchFileHandle *handle, gconstpointer buf, guint size,
			    GError **err);
GIOStatus ags_iofuncs_seek(IpatchFileHandle *handle, int offset, GSeekType type, GError **err);
int ags_iofuncs_getfd(IpatchFileHandle *handle);
int ags_iofuncs_get_size(IpatchFile *file, GError **err);

AgsIpatch* ags_ipatch_new();

#endif /*__AGS_IPATCH_H__*/
