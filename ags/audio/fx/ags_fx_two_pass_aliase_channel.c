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

#include <ags/audio/fx/ags_fx_two_pass_aliase_channel.h>

#include <ags/audio/task/ags_reset_fx_two_pass_aliase.h>

#include <ags/i18n.h>

void ags_fx_two_pass_aliase_channel_class_init(AgsFxTwoPassAliaseChannelClass *fx_two_pass_aliase_channel);
void ags_fx_two_pass_aliase_channel_init(AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel);
void ags_fx_two_pass_aliase_channel_set_property(GObject *gobject,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *param_spec);
void ags_fx_two_pass_aliase_channel_get_property(GObject *gobject,
						 guint prop_id,
						 GValue *value,
						 GParamSpec *param_spec);
void ags_fx_two_pass_aliase_channel_dispose(GObject *gobject);
void ags_fx_two_pass_aliase_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_two_pass_aliase_channel
 * @short_description: fx two pass aliase channel
 * @title: AgsFxTwoPassAliaseChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_two_pass_aliase_channel.h
 *
 * The #AgsFxTwoPassAliaseChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_two_pass_aliase_channel_parent_class = NULL;

const gchar *ags_fx_two_pass_aliase_channel_plugin_name = "ags-fx-two-pass-aliase";

const gchar* ags_fx_two_pass_aliase_channel_specifier[] = {
  NULL,
};

const gchar* ags_fx_two_pass_aliase_channel_control_port[] = {
  NULL,
};

enum{
  PROP_0,
};

GType
ags_fx_two_pass_aliase_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_two_pass_aliase_channel = 0;

    static const GTypeInfo ags_fx_two_pass_aliase_channel_info = {
      sizeof (AgsFxTwoPassAliaseChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_two_pass_aliase_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxTwoPassAliaseChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_two_pass_aliase_channel_init,
    };

    ags_type_fx_two_pass_aliase_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
								 "AgsFxTwoPassAliaseChannel",
								 &ags_fx_two_pass_aliase_channel_info,
								 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_two_pass_aliase_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_two_pass_aliase_channel_class_init(AgsFxTwoPassAliaseChannelClass *fx_two_pass_aliase_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_two_pass_aliase_channel_parent_class = g_type_class_peek_parent(fx_two_pass_aliase_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_two_pass_aliase_channel;

  gobject->set_property = ags_fx_two_pass_aliase_channel_set_property;
  gobject->get_property = ags_fx_two_pass_aliase_channel_get_property;

  gobject->dispose = ags_fx_two_pass_aliase_channel_dispose;
  gobject->finalize = ags_fx_two_pass_aliase_channel_finalize;

  /* properties */
}

void
ags_fx_two_pass_aliase_channel_init(AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel)
{
  AGS_RECALL(fx_two_pass_aliase_channel)->name = "ags-fx-two-pass-aliase";
  AGS_RECALL(fx_two_pass_aliase_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_two_pass_aliase_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_two_pass_aliase_channel)->xml_type = "ags-fx-two-pass-aliase-channel";
}

void
ags_fx_two_pass_aliase_channel_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  GRecMutex *recall_mutex;

  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_two_pass_aliase_channel);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_two_pass_aliase_channel_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  GRecMutex *recall_mutex;

  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_two_pass_aliase_channel);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_two_pass_aliase_channel_dispose(GObject *gobject)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;
  
  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_channel_parent_class)->dispose(gobject);
}

void
ags_fx_two_pass_aliase_channel_finalize(GObject *gobject)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  fx_two_pass_aliase_channel = AGS_FX_TWO_PASS_ALIASE_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_two_pass_aliase_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_two_pass_aliase_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxTwoPassAliaseChannel
 *
 * Returns: the new #AgsFxTwoPassAliaseChannel
 *
 * Since: 3.8.0
 */
AgsFxTwoPassAliaseChannel*
ags_fx_two_pass_aliase_channel_new(AgsChannel *channel)
{
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;

  fx_two_pass_aliase_channel = (AgsFxTwoPassAliaseChannel *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL,
									  "source", channel,
									  NULL);

  return(fx_two_pass_aliase_channel);
}
