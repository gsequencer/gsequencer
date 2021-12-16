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

#ifndef __AGS_HYBRID_SYNTH_H__
#define __AGS_HYBRID_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_HYBRID_SYNTH                (ags_hybrid_synth_get_type())
#define AGS_HYBRID_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_HYBRID_SYNTH, AgsHybridSynth))
#define AGS_HYBRID_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_HYBRID_SYNTH, AgsHybridSynthClass))
#define AGS_IS_HYBRID_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_HYBRID_SYNTH))
#define AGS_IS_HYBRID_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_HYBRID_SYNTH))
#define AGS_HYBRID_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_HYBRID_SYNTH, AgsHybridSynthClass))

typedef struct _AgsHybridSynth AgsHybridSynth;
typedef struct _AgsHybridSynthClass AgsHybridSynthClass;

struct _AgsHybridSynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *synth_play_container;
  AgsRecallContainer *synth_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;
};

struct _AgsHybridSynthClass
{
  AgsMachineClass machine;
};

GType ags_hybrid_synth_get_type(void);

AgsHybridSynth* ags_hybrid_synth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_HYBRID_SYNTH_H__*/
