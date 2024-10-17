/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_OPEN_SF2_SAMPLE_H__
#define __AGS_OPEN_SF2_SAMPLE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>

#include <ags/audio/file/ags_ipatch_sample.h>

G_BEGIN_DECLS

#define AGS_TYPE_OPEN_SF2_SAMPLE                (ags_open_sf2_sample_get_type())
#define AGS_OPEN_SF2_SAMPLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OPEN_SF2_SAMPLE, AgsOpenSF2Sample))
#define AGS_OPEN_SF2_SAMPLE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OPEN_SF2_SAMPLE, AgsOpenSF2SampleClass))
#define AGS_IS_OPEN_SF2_SAMPLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OPEN_SF2_SAMPLE))
#define AGS_IS_OPEN_SF2_SAMPLE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OPEN_SF2_SAMPLE))
#define AGS_OPEN_SF2_SAMPLE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OPEN_SF2_SAMPLE, AgsOpenSF2SampleClass))

typedef struct _AgsOpenSF2Sample AgsOpenSF2Sample;
typedef struct _AgsOpenSF2SampleClass AgsOpenSF2SampleClass;

struct _AgsOpenSF2Sample
{
  AgsTask task;

  AgsChannel *channel;

  AgsIpatchSample *ipatch_sample;
  
  gchar *filename;
  
  gchar *preset;
  gchar *instrument;
  gchar *sample;

  guint audio_channel;
};

struct _AgsOpenSF2SampleClass
{
  AgsTaskClass task;
};

GType ags_open_sf2_sample_get_type();

AgsOpenSF2Sample* ags_open_sf2_sample_new(AgsChannel *channel,
					  AgsIpatchSample *ipatch_sample,
					  gchar *filename,
					  gchar *preset,
					  gchar *instrument,
					  gchar *sample,
					  guint audio_channel);

G_END_DECLS

#endif /*__AGS_OPEN_SF2_SAMPLE_H__*/
