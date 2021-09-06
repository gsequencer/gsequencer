/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_WRITE_VST3_PORT_H__
#define __AGS_WRITE_VST3_PORT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/fx/ags_fx_vst3_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_WRITE_VST3_PORT                (ags_write_vst3_port_get_type())
#define AGS_WRITE_VST3_PORT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WRITE_VST3_PORT, AgsWriteVst3Port))
#define AGS_WRITE_VST3_PORT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WRITE_VST3_PORT, AgsWriteVst3PortClass))
#define AGS_IS_WRITE_VST3_PORT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_WRITE_VST3_PORT))
#define AGS_IS_WRITE_VST3_PORT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_WRITE_VST3_PORT))
#define AGS_WRITE_VST3_PORT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_WRITE_VST3_PORT, AgsWriteVst3PortClass))

typedef struct _AgsWriteVst3Port AgsWriteVst3Port;
typedef struct _AgsWriteVst3PortClass AgsWriteVst3PortClass;

struct _AgsWriteVst3Port
{
  AgsTask task;

  AgsFxVst3Audio *fx_vst3_audio;

  AgsPort *port;

  gdouble value;

  gint sound_scope;
  gint audio_channel;

  gboolean do_replace;
};

struct _AgsWriteVst3PortClass
{
  AgsTaskClass task;
};

GType ags_write_vst3_port_get_type();

AgsWriteVst3Port* ags_write_vst3_port_new(AgsFxVst3Audio *fx_vst3_audio,
					  AgsPort *port,
					  gdouble value,
					  gint sound_scope,
					  gint audio_channel,
					  gboolean do_replace);

G_END_DECLS

#endif /*__AGS_WRITE_VST3_PORT_H__*/
