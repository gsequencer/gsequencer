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

#include <ags/audio/fx/ags_fx_dssi_recycling.h>

#include <ags/audio/fx/ags_fx_dssi_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_dssi_recycling_class_init(AgsFxDssiRecyclingClass *fx_dssi_recycling);
void ags_fx_dssi_recycling_init(AgsFxDssiRecycling *fx_dssi_recycling);
void ags_fx_dssi_recycling_dispose(GObject *gobject);
void ags_fx_dssi_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_dssi_recycling
 * @short_description: fx dssi recycling
 * @title: AgsFxDssiRecycling
 * @section_id:
 * @include: ags/audio/fx/ags_fx_dssi_recycling.h
 *
 * The #AgsFxDssiRecycling class provides ports to the effect processor.
 */

static gpointer ags_fx_dssi_recycling_parent_class = NULL;

const gchar *ags_fx_dssi_recycling_plugin_name = "ags-fx-dssi";

GType
ags_fx_dssi_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_dssi_recycling = 0;

    static const GTypeInfo ags_fx_dssi_recycling_info = {
      sizeof (AgsFxDssiRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_dssi_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_recycling */
      sizeof (AgsFxDssiRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_dssi_recycling_init,
    };

    ags_type_fx_dssi_recycling = g_type_register_static(AGS_TYPE_FX_NOTATION_RECYCLING,
							"AgsFxDssiRecycling",
							&ags_fx_dssi_recycling_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_dssi_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_dssi_recycling_class_init(AgsFxDssiRecyclingClass *fx_dssi_recycling)
{
  GObjectClass *gobject;

  ags_fx_dssi_recycling_parent_class = g_type_class_peek_parent(fx_dssi_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_dssi_recycling;

  gobject->dispose = ags_fx_dssi_recycling_dispose;
  gobject->finalize = ags_fx_dssi_recycling_finalize;
}

void
ags_fx_dssi_recycling_init(AgsFxDssiRecycling *fx_dssi_recycling)
{
  AGS_RECALL(fx_dssi_recycling)->name = "ags-fx-dssi";
  AGS_RECALL(fx_dssi_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_dssi_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_dssi_recycling)->xml_type = "ags-fx-dssi-recycling";

  AGS_RECALL(fx_dssi_recycling)->child_type = AGS_TYPE_FX_DSSI_AUDIO_SIGNAL;
}

void
ags_fx_dssi_recycling_dispose(GObject *gobject)
{
  AgsFxDssiRecycling *fx_dssi_recycling;
  
  fx_dssi_recycling = AGS_FX_DSSI_RECYCLING(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_recycling_parent_class)->dispose(gobject);
}

void
ags_fx_dssi_recycling_finalize(GObject *gobject)
{
  AgsFxDssiRecycling *fx_dssi_recycling;
  
  fx_dssi_recycling = AGS_FX_DSSI_RECYCLING(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_fx_dssi_recycling_new:
 * @recycling: the #AgsRecycling
 *
 * Create a new instance of #AgsFxDssiRecycling
 *
 * Returns: the new #AgsFxDssiRecycling
 *
 * Since: 3.3.0
 */
AgsFxDssiRecycling*
ags_fx_dssi_recycling_new(AgsRecycling *recycling)
{
  AgsFxDssiRecycling *fx_dssi_recycling;

  fx_dssi_recycling = (AgsFxDssiRecycling *) g_object_new(AGS_TYPE_FX_DSSI_RECYCLING,
							  "source", recycling,
							  NULL);

  return(fx_dssi_recycling);
}
