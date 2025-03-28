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

#include <ags/audio/fx/ags_fx_notation_channel_processor.h>

#include <ags/audio/fx/ags_fx_notation_recycling.h>

#include <ags/i18n.h>

void ags_fx_notation_channel_processor_class_init(AgsFxNotationChannelProcessorClass *fx_notation_channel_processor);
void ags_fx_notation_channel_processor_init(AgsFxNotationChannelProcessor *fx_notation_channel_processor);
void ags_fx_notation_channel_processor_dispose(GObject *gobject);
void ags_fx_notation_channel_processor_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_notation_channel_processor
 * @short_description: fx notation channel processor
 * @title: AgsFxNotationChannelProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_notation_channel_processor.h
 *
 * The #AgsFxNotationChannelProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_notation_channel_processor_parent_class = NULL;

const gchar *ags_fx_notation_channel_processor_plugin_name = "ags-fx-notation";

GType
ags_fx_notation_channel_processor_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_notation_channel_processor = 0;

    static const GTypeInfo ags_fx_notation_channel_processor_info = {
      sizeof (AgsFxNotationChannelProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_notation_channel_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel_processor */
      sizeof (AgsFxNotationChannelProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_notation_channel_processor_init,
    };

    ags_type_fx_notation_channel_processor = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
								    "AgsFxNotationChannelProcessor",
								    &ags_fx_notation_channel_processor_info,
								    0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_notation_channel_processor);
  }

  return(g_define_type_id__static);
}

void
ags_fx_notation_channel_processor_class_init(AgsFxNotationChannelProcessorClass *fx_notation_channel_processor)
{
  GObjectClass *gobject;

  ags_fx_notation_channel_processor_parent_class = g_type_class_peek_parent(fx_notation_channel_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_notation_channel_processor;

  gobject->dispose = ags_fx_notation_channel_processor_dispose;
  gobject->finalize = ags_fx_notation_channel_processor_finalize;
}

void
ags_fx_notation_channel_processor_init(AgsFxNotationChannelProcessor *fx_notation_channel_processor)
{
  AGS_RECALL(fx_notation_channel_processor)->name = "ags-fx-notation";
  AGS_RECALL(fx_notation_channel_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_notation_channel_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_notation_channel_processor)->xml_type = "ags-fx-notation-channel-processor";

  AGS_RECALL(fx_notation_channel_processor)->child_type = AGS_TYPE_FX_NOTATION_RECYCLING;
}

void
ags_fx_notation_channel_processor_dispose(GObject *gobject)
{
  AgsFxNotationChannelProcessor *fx_notation_channel_processor;
  
  fx_notation_channel_processor = AGS_FX_NOTATION_CHANNEL_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_channel_processor_parent_class)->dispose(gobject);
}

void
ags_fx_notation_channel_processor_finalize(GObject *gobject)
{
  AgsFxNotationChannelProcessor *fx_notation_channel_processor;
  
  fx_notation_channel_processor = AGS_FX_NOTATION_CHANNEL_PROCESSOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_channel_processor_parent_class)->finalize(gobject);
}

/**
 * ags_fx_notation_channel_processor_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxNotationChannelProcessor
 *
 * Returns: the new #AgsFxNotationChannelProcessor
 *
 * Since: 3.3.0
 */
AgsFxNotationChannelProcessor*
ags_fx_notation_channel_processor_new(AgsChannel *channel)
{
  AgsFxNotationChannelProcessor *fx_notation_channel_processor;

  fx_notation_channel_processor = (AgsFxNotationChannelProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_CHANNEL_PROCESSOR,
										 "source", channel,
										 NULL);

  return(fx_notation_channel_processor);
}
