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

#ifndef __AGS_OPEN_SF2_INSTRUMENT_H__
#define __AGS_OPEN_SF2_INSTRUMENT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/audio/file/ags_ipatch.h>

G_BEGIN_DECLS

#define AGS_TYPE_OPEN_SF2_INSTRUMENT                (ags_open_sf2_instrument_get_type())
#define AGS_OPEN_SF2_INSTRUMENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OPEN_SF2_INSTRUMENT, AgsOpenSF2Instrument))
#define AGS_OPEN_SF2_INSTRUMENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OPEN_SF2_INSTRUMENT, AgsOpenSF2InstrumentClass))
#define AGS_IS_OPEN_SF2_INSTRUMENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OPEN_SF2_INSTRUMENT))
#define AGS_IS_OPEN_SF2_INSTRUMENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OPEN_SF2_INSTRUMENT))
#define AGS_OPEN_SF2_INSTRUMENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OPEN_SF2_INSTRUMENT, AgsOpenSF2InstrumentClass))

typedef struct _AgsOpenSF2Instrument AgsOpenSF2Instrument;
typedef struct _AgsOpenSF2InstrumentClass AgsOpenSF2InstrumentClass;

struct _AgsOpenSF2Instrument
{
  AgsTask task;

  AgsAudio *audio;

  AgsIpatch *ipatch;
  
  gchar *filename;
  
  gchar *preset;
  gchar *instrument;

  guint start_pad;
};

struct _AgsOpenSF2InstrumentClass
{
  AgsTaskClass task;
};

GType ags_open_sf2_instrument_get_type();

AgsOpenSF2Instrument* ags_open_sf2_instrument_new(AgsAudio *audio,
						  AgsIpatch *ipatch,
						  gchar *filename,
						  gchar *preset,
						  gchar *instrument,
						  guint start_pad);

G_END_DECLS

#endif /*__AGS_OPEN_SF2_INSTRUMENT_H__*/
