/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/task/ags_link_channel.h>

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_link_channel_class_init(AgsLinkChannelClass *link_channel);
void ags_link_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_link_channel_init(AgsLinkChannel *link_channel);
void ags_link_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_link_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_link_channel_connect(AgsConnectable *connectable);
void ags_link_channel_disconnect(AgsConnectable *connectable);
void ags_link_channel_dispose(GObject *gobject);
void ags_link_channel_finalize(GObject *gobject);

void ags_link_channel_launch(AgsTask *task);

/**
 * SECTION:ags_link_channel
 * @short_description: link channel task
 * @title: AgsLinkChannel
 * @section_id:
 * @include: ags/audio/task/ags_link_channel.h
 *
 * The #AgsLinkChannel task links #AgsChannel.
 */

static gpointer ags_link_channel_parent_class = NULL;
static AgsConnectableInterface *ags_link_channel_parent_connectable_interface;

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_LINK,
  PROP_ERROR,
};

GType
ags_link_channel_get_type()
{
  static GType ags_type_link_channel = 0;

  if(!ags_type_link_channel){
    static const GTypeInfo ags_link_channel_info = {
      sizeof (AgsLinkChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_link_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLinkChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_link_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_link_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_link_channel = g_type_register_static(AGS_TYPE_TASK,
						   "AgsLinkChannel",
						   &ags_link_channel_info,
						   0);

    g_type_add_interface_static(ags_type_link_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_link_channel);
}

void
ags_link_channel_class_init(AgsLinkChannelClass *link_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_link_channel_parent_class = g_type_class_peek_parent(link_channel);

  /* GObject */
  gobject = (GObjectClass *) link_channel;

  gobject->set_property = ags_link_channel_set_property;
  gobject->get_property = ags_link_channel_get_property;

  gobject->dispose = ags_link_channel_dispose;
  gobject->finalize = ags_link_channel_finalize;

  /* properties */
  /**
   * AgsLinkChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of link channel"),
				   i18n_pspec("The channel of link channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsLinkChannel:link:
   *
   * The assigned #AgsChannel link
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("link",
				   i18n_pspec("link of link channel"),
				   i18n_pspec("The link of link channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINK,
				  param_spec);

  /**
   * AgsLinkChannel:error:
   *
   * The assigned #GError-struct
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("error",
				    i18n_pspec("error of link channel"),
				    i18n_pspec("The error of link channel task"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ERROR,
				  param_spec);

  /* AgsTask */
  task = (AgsTaskClass *) link_channel;

  task->launch = ags_link_channel_launch;
}

void
ags_link_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_link_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_link_channel_connect;
  connectable->disconnect = ags_link_channel_disconnect;
}

void
ags_link_channel_init(AgsLinkChannel *link_channel)
{
  link_channel->channel = NULL;
  link_channel->link = NULL;

  link_channel->error = NULL;
}

void
ags_link_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsLinkChannel *link_channel;

  link_channel = AGS_LINK_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(link_channel->channel == (GObject *) channel){
	return;
      }

      if(link_channel->channel != NULL){
	g_object_unref(link_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      link_channel->channel = (GObject *) channel;
    }
    break;
  case PROP_LINK:
    {
      AgsChannel *link;

      link = (AgsChannel *) g_value_get_object(value);

      if(link_channel->link == (GObject *) link){
	return;
      }

      if(link_channel->link != NULL){
	g_object_unref(link_channel->link);
      }

      if(link != NULL){
	g_object_ref(link);
      }

      link_channel->link = (GObject *) link;
    }
    break;
  case PROP_ERROR:
    {
      GError *error;

      error = g_value_get_pointer(value);

      link_channel->error = error;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_link_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsLinkChannel *link_channel;

  link_channel = AGS_LINK_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, link_channel->channel);
    }
    break;
  case PROP_LINK:
    {
      g_value_set_object(value, link_channel->link);
    }
    break;
  case PROP_ERROR:
    {
      g_value_set_pointer(value, link_channel->error);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_link_channel_connect(AgsConnectable *connectable)
{
  ags_link_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_link_channel_disconnect(AgsConnectable *connectable)
{
  ags_link_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_link_channel_dispose(GObject *gobject)
{
  AgsLinkChannel *link_channel;

  link_channel = AGS_LINK_CHANNEL(gobject);

  if(link_channel->channel != NULL){
    g_object_unref(link_channel->channel);

    link_channel->channel = NULL;
  }

  if(link_channel->link != NULL){
    g_object_unref(link_channel->link);

    link_channel->link = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_link_channel_parent_class)->dispose(gobject);
}

void
ags_link_channel_finalize(GObject *gobject)
{
  AgsLinkChannel *link_channel;

  link_channel = AGS_LINK_CHANNEL(gobject);

  if(link_channel->channel != NULL){
    g_object_unref(link_channel->channel);
  }

  if(link_channel->link != NULL){
    g_object_unref(link_channel->link);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_link_channel_parent_class)->finalize(gobject);
}

void
ags_link_channel_launch(AgsTask *task)
{
  AgsLinkChannel *link_channel;

  link_channel = AGS_LINK_CHANNEL(task);

  /* link channel */
  g_object_set(link_channel->channel,
	       "file-link", NULL,
	       NULL);
  ags_channel_set_link(link_channel->channel, link_channel->link,
  		       &(link_channel->error));

  if(link_channel->error != NULL){
    g_message("%s", link_channel->error->message);
  }
}

/**
 * ags_link_channel_new:
 * @channel: the #AgsChannel
 * @link: the #AgsChannel to be linked, may be %NULL
 *
 * Creates an #AgsLinkChannel.
 *
 * Returns: an new #AgsLinkChannel.
 *
 * Since: 1.0.0
 */
AgsLinkChannel*
ags_link_channel_new(AgsChannel *channel, AgsChannel *link)
{
  AgsLinkChannel *link_channel;

  link_channel = (AgsLinkChannel *) g_object_new(AGS_TYPE_LINK_CHANNEL,
						 "channel", channel,
						 "link", link,
						 NULL);

  return(link_channel);
}
