/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/config.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#define AGS_TYPE_IPATCH_SAMPLE                (ags_ipatch_sample_get_type())
#define AGS_IPATCH_SAMPLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_IPATCH_SAMPLE, AgsIpatchSample))
#define AGS_IPATCH_SAMPLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_IPATCH_SAMPLE, AgsIpatchSampleClass))
#define AGS_IS_IPATCH_SAMPLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_IPATCH_SAMPLE))
#define AGS_IS_IPATCH_SAMPLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_IPATCH_SAMPLE))
#define AGS_IPATCH_SAMPLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_IPATCH_SAMPLE, AgsIpatchSampleClass))

typedef struct _AgsIpatchSample AgsIpatchSample;
typedef struct _AgsIpatchSampleClass AgsIpatchSampleClass;

struct _AgsIpatchSample
{
  GObject object;

  guint buffer_size;
  guint format;
  
  void *buffer;
  
#ifdef AGS_WITH_LIBINSTPATCH
  IpatchContainer *sample;
#else
  gpointer sample;
#endif
};

struct _AgsIpatchSampleClass
{
  GObjectClass object;
};

GType ags_ipatch_sample_get_type();

/* instantiate */
AgsIpatchSample* ags_ipatch_sample_new();

#endif /*__AGS_IPATCH_SAMPLE_H__*/
