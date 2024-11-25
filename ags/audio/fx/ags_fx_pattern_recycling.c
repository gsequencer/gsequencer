/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_pattern_recycling.h>

#include <ags/audio/fx/ags_fx_pattern_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_pattern_recycling_class_init(AgsFxPatternRecyclingClass *fx_pattern_recycling);
void ags_fx_pattern_recycling_init(AgsFxPatternRecycling *fx_pattern_recycling);
void ags_fx_pattern_recycling_dispose(GObject *gobject);
void ags_fx_pattern_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_pattern_recycling
 * @short_description: fx pattern recycling
 * @title: AgsFxPatternRecycling
 * @section_id:
 * @include: ags/audio/fx/ags_fx_pattern_recycling.h
 *
 * The #AgsFxPatternRecycling class provides ports to the effect processor.
 */

static gpointer ags_fx_pattern_recycling_parent_class = NULL;

const gchar *ags_fx_pattern_recycling_plugin_name = "ags-fx-pattern";

GType
ags_fx_pattern_recycling_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_pattern_recycling = 0;

    static const GTypeInfo ags_fx_pattern_recycling_info = {
      sizeof (AgsFxPatternRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_pattern_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_recycling */
      sizeof (AgsFxPatternRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_pattern_recycling_init,
    };

    ags_type_fx_pattern_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							   "AgsFxPatternRecycling",
							   &ags_fx_pattern_recycling_info,
							   0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_pattern_recycling);
  }

  return g_define_type_id__static;
}

void
ags_fx_pattern_recycling_class_init(AgsFxPatternRecyclingClass *fx_pattern_recycling)
{
  GObjectClass *gobject;

  ags_fx_pattern_recycling_parent_class = g_type_class_peek_parent(fx_pattern_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_pattern_recycling;

  gobject->dispose = ags_fx_pattern_recycling_dispose;
  gobject->finalize = ags_fx_pattern_recycling_finalize;
}

void
ags_fx_pattern_recycling_init(AgsFxPatternRecycling *fx_pattern_recycling)
{
  AGS_RECALL(fx_pattern_recycling)->name = "ags-fx-pattern";
  AGS_RECALL(fx_pattern_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_pattern_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_pattern_recycling)->xml_type = "ags-fx-pattern-recycling";

  AGS_RECALL(fx_pattern_recycling)->child_type = AGS_TYPE_FX_PATTERN_AUDIO_SIGNAL;
}

void
ags_fx_pattern_recycling_dispose(GObject *gobject)
{
  AgsFxPatternRecycling *fx_pattern_recycling;
  
  fx_pattern_recycling = AGS_FX_PATTERN_RECYCLING(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_pattern_recycling_parent_class)->dispose(gobject);
}

void
ags_fx_pattern_recycling_finalize(GObject *gobject)
{
  AgsFxPatternRecycling *fx_pattern_recycling;
  
  fx_pattern_recycling = AGS_FX_PATTERN_RECYCLING(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_pattern_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_fx_pattern_recycling_new:
 * @recycling: the #AgsRecycling
 *
 * Create a new instance of #AgsFxPatternRecycling
 *
 * Returns: the new #AgsFxPatternRecycling
 *
 * Since: 3.3.0
 */
AgsFxPatternRecycling*
ags_fx_pattern_recycling_new(AgsRecycling *recycling)
{
  AgsFxPatternRecycling *fx_pattern_recycling;

  fx_pattern_recycling = (AgsFxPatternRecycling *) g_object_new(AGS_TYPE_FX_PATTERN_RECYCLING,
								"source", recycling,
								NULL);

  return(fx_pattern_recycling);
}
