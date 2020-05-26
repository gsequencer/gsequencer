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

#include <ags/audio/fx/ags_fx_buffer_recycling.h>

#include <ags/audio/fx/ags_fx_buffer_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_buffer_recycling_class_init(AgsFxBufferRecyclingClass *fx_buffer_recycling);
void ags_fx_buffer_recycling_init(AgsFxBufferRecycling *fx_buffer_recycling);
void ags_fx_buffer_recycling_dispose(GObject *gobject);
void ags_fx_buffer_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_buffer_recycling
 * @short_description: fx buffer recycling
 * @title: AgsFxBufferRecycling
 * @section_id:
 * @include: ags/audio/fx/ags_fx_buffer_recycling.h
 *
 * The #AgsFxBufferRecycling class provides ports to the effect processor.
 */

static gpointer ags_fx_buffer_recycling_parent_class = NULL;

const gchar *ags_fx_buffer_recycling_plugin_name = "ags-fx-buffer";

GType
ags_fx_buffer_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_buffer_recycling = 0;

    static const GTypeInfo ags_fx_buffer_recycling_info = {
      sizeof (AgsFxBufferRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_buffer_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_recycling */
      sizeof (AgsFxBufferRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_buffer_recycling_init,
    };

    ags_type_fx_buffer_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							  "AgsFxBufferRecycling",
							  &ags_fx_buffer_recycling_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_buffer_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_buffer_recycling_class_init(AgsFxBufferRecyclingClass *fx_buffer_recycling)
{
  GObjectClass *gobject;

  ags_fx_buffer_recycling_parent_class = g_type_class_peek_parent(fx_buffer_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_buffer_recycling;

  gobject->dispose = ags_fx_buffer_recycling_dispose;
  gobject->finalize = ags_fx_buffer_recycling_finalize;
}

void
ags_fx_buffer_recycling_init(AgsFxBufferRecycling *fx_buffer_recycling)
{
  AGS_RECALL(fx_buffer_recycling)->name = "ags-fx-buffer";
  AGS_RECALL(fx_buffer_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_buffer_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_buffer_recycling)->xml_type = "ags-fx-buffer-recycling";

  AGS_RECALL(fx_buffer_recycling)->child_type = AGS_TYPE_FX_BUFFER_AUDIO_SIGNAL;
}

void
ags_fx_buffer_recycling_dispose(GObject *gobject)
{
  AgsFxBufferRecycling *fx_buffer_recycling;
  
  fx_buffer_recycling = AGS_FX_BUFFER_RECYCLING(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_recycling_parent_class)->dispose(gobject);
}

void
ags_fx_buffer_recycling_finalize(GObject *gobject)
{
  AgsFxBufferRecycling *fx_buffer_recycling;
  
  fx_buffer_recycling = AGS_FX_BUFFER_RECYCLING(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_fx_buffer_recycling_new:
 * @recycling: the #AgsRecycling
 *
 * Create a new instance of #AgsFxBufferRecycling
 *
 * Returns: the new #AgsFxBufferRecycling
 *
 * Since: 3.3.0
 */
AgsFxBufferRecycling*
ags_fx_buffer_recycling_new(AgsRecycling *recycling)
{
  AgsFxBufferRecycling *fx_buffer_recycling;

  fx_buffer_recycling = (AgsFxBufferRecycling *) g_object_new(AGS_TYPE_FX_BUFFER_RECYCLING,
							      "source", recycling,
							      NULL);

  return(fx_buffer_recycling);
}
