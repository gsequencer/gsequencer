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

#include <ags/audio/fx/ags_fx_lfo_recycling.h>

#include <ags/audio/fx/ags_fx_lfo_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_lfo_recycling_class_init(AgsFxLfoRecyclingClass *fx_lfo_recycling);
void ags_fx_lfo_recycling_init(AgsFxLfoRecycling *fx_lfo_recycling);
void ags_fx_lfo_recycling_dispose(GObject *gobject);
void ags_fx_lfo_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_lfo_recycling
 * @short_description: fx lfo recycling
 * @title: AgsFxLfoRecycling
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lfo_recycling.h
 *
 * The #AgsFxLfoRecycling class provides ports to the effect processor.
 */

static gpointer ags_fx_lfo_recycling_parent_class = NULL;

const gchar *ags_fx_lfo_recycling_plugin_name = "ags-fx-lfo";

GType
ags_fx_lfo_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lfo_recycling = 0;

    static const GTypeInfo ags_fx_lfo_recycling_info = {
      sizeof (AgsFxLfoRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lfo_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_recycling */
      sizeof (AgsFxLfoRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lfo_recycling_init,
    };

    ags_type_fx_lfo_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						       "AgsFxLfoRecycling",
						       &ags_fx_lfo_recycling_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lfo_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lfo_recycling_class_init(AgsFxLfoRecyclingClass *fx_lfo_recycling)
{
  GObjectClass *gobject;

  ags_fx_lfo_recycling_parent_class = g_type_class_peek_parent(fx_lfo_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lfo_recycling;

  gobject->dispose = ags_fx_lfo_recycling_dispose;
  gobject->finalize = ags_fx_lfo_recycling_finalize;
}

void
ags_fx_lfo_recycling_init(AgsFxLfoRecycling *fx_lfo_recycling)
{
  AGS_RECALL(fx_lfo_recycling)->name = "ags-fx-lfo";
  AGS_RECALL(fx_lfo_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lfo_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lfo_recycling)->xml_type = "ags-fx-lfo-recycling";

  AGS_RECALL(fx_lfo_recycling)->child_type = AGS_TYPE_FX_LFO_AUDIO_SIGNAL;
}

void
ags_fx_lfo_recycling_dispose(GObject *gobject)
{
  AgsFxLfoRecycling *fx_lfo_recycling;
  
  fx_lfo_recycling = AGS_FX_LFO_RECYCLING(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lfo_recycling_parent_class)->dispose(gobject);
}

void
ags_fx_lfo_recycling_finalize(GObject *gobject)
{
  AgsFxLfoRecycling *fx_lfo_recycling;
  
  fx_lfo_recycling = AGS_FX_LFO_RECYCLING(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lfo_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_fx_lfo_recycling_new:
 * @recycling: the #AgsRecycling
 *
 * Create a new instance of #AgsFxLfoRecycling
 *
 * Returns: the new #AgsFxLfoRecycling
 *
 * Since: 3.8.0
 */
AgsFxLfoRecycling*
ags_fx_lfo_recycling_new(AgsRecycling *recycling)
{
  AgsFxLfoRecycling *fx_lfo_recycling;

  fx_lfo_recycling = (AgsFxLfoRecycling *) g_object_new(AGS_TYPE_FX_LFO_RECYCLING,
							"source", recycling,
							NULL);

  return(fx_lfo_recycling);
}
