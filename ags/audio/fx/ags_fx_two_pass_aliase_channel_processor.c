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

#include <ags/audio/fx/ags_fx_two_pass_aliase_channel_processor.h>

#include <ags/audio/fx/ags_fx_two_pass_aliase_channel.h>
#include <ags/audio/fx/ags_fx_two_pass_aliase_recycling.h>

#include <ags/i18n.h>

void ags_fx_two_pass_aliase_channel_processor_class_init(AgsFxTwoPassAliaseChannelProcessorClass *fx_two_pass_aliase_channel_processor);
void ags_fx_two_pass_aliase_channel_processor_init(AgsFxTwoPassAliaseChannelProcessor *fx_two_pass_aliase_channel_processor);
void ags_fx_two_pass_aliase_channel_processor_dispose(GObject *gobject);
void ags_fx_two_pass_aliase_channel_processor_finalize(GObject *gobject);

void ags_fx_two_pass_aliase_channel_processor_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_two_pass_aliase_channel_processor
 * @short_description: fx two pass aliase channel processor
 * @title: AgsFxTwoPassAliaseChannelProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_two_pass_aliase_channel_processor.h
 *
 * The #AgsFxTwoPassAliaseChannelProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_two_pass_aliase_channel_processor_parent_class = NULL;

const gchar *ags_fx_two_pass_aliase_channel_processor_plugin_name = "ags-fx-two-pass-aliase";

GType
ags_fx_two_pass_aliase_channel_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_two_pass_aliase_channel_processor = 0;

    static const GTypeInfo ags_fx_two_pass_aliase_channel_processor_info = {
      sizeof (AgsFxTwoPassAliaseChannelProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_two_pass_aliase_channel_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel_processor */
      sizeof (AgsFxTwoPassAliaseChannelProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_two_pass_aliase_channel_processor_init,
    };

    ags_type_fx_two_pass_aliase_channel_processor = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
									   "AgsFxTwoPassAliaseChannelProcessor",
									   &ags_fx_two_pass_aliase_channel_processor_info,
									   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_two_pass_aliase_channel_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_two_pass_aliase_channel_processor_class_init(AgsFxTwoPassAliaseChannelProcessorClass *fx_two_pass_aliase_channel_processor)
{
  GObjectClass *gobject;

  ags_fx_two_pass_aliase_channel_processor_parent_class = g_type_class_peek_parent(fx_two_pass_aliase_channel_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_two_pass_aliase_channel_processor;

  gobject->dispose = ags_fx_two_pass_aliase_channel_processor_dispose;
  gobject->finalize = ags_fx_two_pass_aliase_channel_processor_finalize;
}

void
ags_fx_two_pass_aliase_channel_processor_init(AgsFxTwoPassAliaseChannelProcessor *fx_two_pass_aliase_channel_processor)
{
  AGS_RECALL(fx_two_pass_aliase_channel_processor)->name = "ags-fx-two-pass-aliase";
  AGS_RECALL(fx_two_pass_aliase_channel_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_two_pass_aliase_channel_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_two_pass_aliase_channel_processor)->xml_type = "ags-fx-two-pass-aliase-channel-processor";

  AGS_RECALL(fx_two_pass_aliase_channel_processor)->child_type = AGS_TYPE_FX_TWO_PASS_ALIASE_RECYCLING;
}

void
ags_fx_two_pass_aliase_channel_processor_dispose(GObject *gobject)
{
  AgsFxTwoPassAliaseChannelProcessor *fx_two_pass_aliase_channel_processor;
  
  fx_two_pass_aliase_channel_processor = AGS_FX_TWO_PASS_ALIASE_CHANNEL_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_channel_processor_parent_class)->dispose(gobject);
}

void
ags_fx_two_pass_aliase_channel_processor_finalize(GObject *gobject)
{
  AgsFxTwoPassAliaseChannelProcessor *fx_two_pass_aliase_channel_processor;
  
  fx_two_pass_aliase_channel_processor = AGS_FX_TWO_PASS_ALIASE_CHANNEL_PROCESSOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_channel_processor_parent_class)->finalize(gobject);
}

/**
 * ags_fx_two_pass_aliase_channel_processor_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxTwoPassAliaseChannelProcessor
 *
 * Returns: the new #AgsFxTwoPassAliaseChannelProcessor
 *
 * Since: 3.8.0
 */
AgsFxTwoPassAliaseChannelProcessor*
ags_fx_two_pass_aliase_channel_processor_new(AgsChannel *channel)
{
  AgsFxTwoPassAliaseChannelProcessor *fx_two_pass_aliase_channel_processor;

  fx_two_pass_aliase_channel_processor = (AgsFxTwoPassAliaseChannelProcessor *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL_PROCESSOR,
											     "source", channel,
											     NULL);

  return(fx_two_pass_aliase_channel_processor);
}
