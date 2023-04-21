/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_IPATCH_SAMPLE_H__
#define __AGS_IPATCH_SAMPLE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/ags_api_config.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

G_BEGIN_DECLS

#define AGS_TYPE_IPATCH_SAMPLE                (ags_ipatch_sample_get_type())
#define AGS_IPATCH_SAMPLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_IPATCH_SAMPLE, AgsIpatchSample))
#define AGS_IPATCH_SAMPLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_IPATCH_SAMPLE, AgsIpatchSampleClass))
#define AGS_IS_IPATCH_SAMPLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_IPATCH_SAMPLE))
#define AGS_IS_IPATCH_SAMPLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_IPATCH_SAMPLE))
#define AGS_IPATCH_SAMPLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_IPATCH_SAMPLE, AgsIpatchSampleClass))

#define AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(obj) (&(((AgsIpatchSample *) obj)->obj_mutex))

typedef struct _AgsIpatchSample AgsIpatchSample;
typedef struct _AgsIpatchSampleClass AgsIpatchSampleClass;

struct _AgsIpatchSample
{
  GObject gobject;

  guint flags;
  AgsConnectableFlags connectable_flags;
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint audio_channels;
  gint64 *audio_channel_written;

  guint buffer_size;
  AgsSoundcardFormat format;

  guint offset;
  guint64 buffer_offset;

  void *full_buffer;
  void *buffer;

  guchar *pointer;
  guchar *current;
  gsize length;
  
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchContainer *sample;
#else
  gpointer sample;
#endif
};

struct _AgsIpatchSampleClass
{
  GObjectClass gobject;
};

GType ags_ipatch_sample_get_type();

/* instantiate */
AgsIpatchSample* ags_ipatch_sample_new();

G_END_DECLS

#endif /*__AGS_IPATCH_SAMPLE_H__*/
