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

#include <ags/audio/fx/ags_fx_eq10_channel.h>

#include <ags/i18n.h>

void ags_fx_eq10_channel_class_init(AgsFxEq10ChannelClass *fx_eq10_channel);
void ags_fx_eq10_channel_init(AgsFxEq10Channel *fx_eq10_channel);
void ags_fx_eq10_channel_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_fx_eq10_channel_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_fx_eq10_channel_dispose(GObject *gobject);
void ags_fx_eq10_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_eq10_channel
 * @short_description: fx eq10 channel
 * @title: AgsFxEq10Channel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_eq10_channel.h
 *
 * The #AgsFxEq10Channel class provides ports to the effect processor.
 */

static gpointer ags_fx_eq10_channel_parent_class = NULL;

static const gchar *ags_fx_eq10_channel_plugin_name = "ags-fx-eq10";

static const gchar *ags_fx_eq10_channel_specifier[] = {
  NULL,
};

static const gchar *ags_fx_eq10_channel_control_port[] = {
  NULL,
};

enum{
  PROP_0,
};

GType
ags_fx_eq10_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_eq10_channel = 0;

    static const GTypeInfo ags_fx_eq10_channel_info = {
      sizeof (AgsFxEq10ChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_eq10_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxEq10Channel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_eq10_channel_init,
    };

    ags_type_fx_eq10_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						      "AgsFxEq10Channel",
						      &ags_fx_eq10_channel_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_eq10_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_eq10_channel_class_init(AgsFxEq10ChannelClass *fx_eq10_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_eq10_channel_parent_class = g_type_class_peek_parent(fx_eq10_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_eq10_channel;

  gobject->set_property = ags_fx_eq10_channel_set_property;
  gobject->get_property = ags_fx_eq10_channel_get_property;

  gobject->dispose = ags_fx_eq10_channel_dispose;
  gobject->finalize = ags_fx_eq10_channel_finalize;

  /* properties */
}

void
ags_fx_eq10_channel_init(AgsFxEq10Channel *fx_eq10_channel)
{
  AGS_RECALL(fx_eq10_channel)->name = "ags-fx-eq10";
  AGS_RECALL(fx_eq10_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_eq10_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_eq10_channel)->xml_type = "ags-fx-eq10-channel";
}

void
ags_fx_eq10_channel_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxEq10Channel *fx_eq10_channel;

  GRecMutex *recall_mutex;

  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_eq10_channel);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_eq10_channel_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsFxEq10Channel *fx_eq10_channel;

  GRecMutex *recall_mutex;

  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_eq10_channel);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_eq10_channel_dispose(GObject *gobject)
{
  AgsFxEq10Channel *fx_eq10_channel;
  
  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_eq10_channel_parent_class)->dispose(gobject);
}

void
ags_fx_eq10_channel_finalize(GObject *gobject)
{
  AgsFxEq10Channel *fx_eq10_channel;
  
  fx_eq10_channel = AGS_FX_EQ10_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_eq10_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_eq10_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxEq10Channel
 *
 * Returns: the new #AgsFxEq10Channel
 *
 * Since: 3.3.0
 */
AgsFxEq10Channel*
ags_fx_eq10_channel_new(AgsChannel *channel)
{
  AgsFxEq10Channel *fx_eq10_channel;

  fx_eq10_channel = (AgsFxEq10Channel *) g_object_new(AGS_TYPE_FX_EQ10_CHANNEL,
						      "source", channel,
						      NULL);

  return(fx_eq10_channel);
}
