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

#include <ags/audio/fx/ags_fx_sf2_synth_recycling.h>

#include <ags/audio/fx/ags_fx_sf2_synth_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_sf2_synth_recycling_class_init(AgsFxSF2SynthRecyclingClass *fx_sf2_synth_recycling);
void ags_fx_sf2_synth_recycling_init(AgsFxSF2SynthRecycling *fx_sf2_synth_recycling);
void ags_fx_sf2_synth_recycling_dispose(GObject *gobject);
void ags_fx_sf2_synth_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_sf2_synth_recycling
 * @short_description: fx Soundfont2 synth recycling
 * @title: AgsFxSF2SynthRecycling
 * @section_id:
 * @include: ags/audio/fx/ags_fx_sf2_synth_recycling.h
 *
 * The #AgsFxSF2SynthRecycling class provides ports to the effect processor.
 */

static gpointer ags_fx_sf2_synth_recycling_parent_class = NULL;

const gchar *ags_fx_sf2_synth_recycling_plugin_name = "ags-fx-sf2-synth";

GType
ags_fx_sf2_synth_recycling_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_sf2_synth_recycling = 0;

    static const GTypeInfo ags_fx_sf2_synth_recycling_info = {
      sizeof (AgsFxSF2SynthRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_sf2_synth_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_recycling */
      sizeof (AgsFxSF2SynthRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_sf2_synth_recycling_init,
    };

    ags_type_fx_sf2_synth_recycling = g_type_register_static(AGS_TYPE_FX_NOTATION_RECYCLING,
							     "AgsFxSF2SynthRecycling",
							     &ags_fx_sf2_synth_recycling_info,
							     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_sf2_synth_recycling);
  }

  return(g_define_type_id__static);
}

void
ags_fx_sf2_synth_recycling_class_init(AgsFxSF2SynthRecyclingClass *fx_sf2_synth_recycling)
{
  GObjectClass *gobject;

  ags_fx_sf2_synth_recycling_parent_class = g_type_class_peek_parent(fx_sf2_synth_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_sf2_synth_recycling;

  gobject->dispose = ags_fx_sf2_synth_recycling_dispose;
  gobject->finalize = ags_fx_sf2_synth_recycling_finalize;
}

void
ags_fx_sf2_synth_recycling_init(AgsFxSF2SynthRecycling *fx_sf2_synth_recycling)
{
  AGS_RECALL(fx_sf2_synth_recycling)->name = "ags-fx-sf2-synth";
  AGS_RECALL(fx_sf2_synth_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_sf2_synth_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_sf2_synth_recycling)->xml_type = "ags-fx-sf2-synth-recycling";

  AGS_RECALL(fx_sf2_synth_recycling)->child_type = AGS_TYPE_FX_SF2_SYNTH_AUDIO_SIGNAL;
}

void
ags_fx_sf2_synth_recycling_dispose(GObject *gobject)
{
  AgsFxSF2SynthRecycling *fx_sf2_synth_recycling;
  
  fx_sf2_synth_recycling = AGS_FX_SF2_SYNTH_RECYCLING(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_sf2_synth_recycling_parent_class)->dispose(gobject);
}

void
ags_fx_sf2_synth_recycling_finalize(GObject *gobject)
{
  AgsFxSF2SynthRecycling *fx_sf2_synth_recycling;
  
  fx_sf2_synth_recycling = AGS_FX_SF2_SYNTH_RECYCLING(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_sf2_synth_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_fx_sf2_synth_recycling_new:
 * @recycling: the #AgsRecycling
 *
 * Create a new instance of #AgsFxSF2SynthRecycling
 *
 * Returns: the new #AgsFxSF2SynthRecycling
 *
 * Since: 3.16.0
 */
AgsFxSF2SynthRecycling*
ags_fx_sf2_synth_recycling_new(AgsRecycling *recycling)
{
  AgsFxSF2SynthRecycling *fx_sf2_synth_recycling;

  fx_sf2_synth_recycling = (AgsFxSF2SynthRecycling *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_RECYCLING,
								   "source", recycling,
								   NULL);

  return(fx_sf2_synth_recycling);
}
