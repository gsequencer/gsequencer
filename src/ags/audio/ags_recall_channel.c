/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_recall_channel.h>

#include <ags/lib/ags_list.h>
#include <ags/lib/ags_parameter.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_packable.h>

#include <ags/audio/ags_recall_container.h>

void ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel);
void ags_recall_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_packable_interface_init(AgsPackableInterface *packable);
void ags_recall_channel_init(AgsRecallChannel *recall_channel);
void ags_recall_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_recall_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_recall_channel_connect(AgsConnectable *connectable);
void ags_recall_channel_disconnect(AgsConnectable *connectable);
gboolean ags_recall_channel_pack(AgsPackable *packable, GObject *container);
gboolean ags_recall_channel_unpack(AgsPackable *packable);
void ags_recall_channel_finalize(GObject *gobject);

AgsRecall* ags_recall_channel_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint *n_params, GParameter *parameter);

enum{
  PROP_0,
  PROP_DESTINATION,
  PROP_SOURCE,
};

static gpointer ags_recall_channel_parent_class = NULL;
static AgsConnectableInterface* ags_recall_channel_parent_connectable_interface;
static AgsPackableInterface* ags_recall_channel_parent_packable_interface;

GType
ags_recall_channel_get_type()
{
  static GType ags_type_recall_channel = 0;

  if(!ags_type_recall_channel){
    static const GTypeInfo ags_recall_channel_info = {
      sizeof (AgsRecallChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_packable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_packable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_channel = g_type_register_static(AGS_TYPE_RECALL,
						     "AgsRecallChannel\0",
						     &ags_recall_channel_info,
						     0);

    g_type_add_interface_static(ags_type_recall_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_channel,
				AGS_TYPE_PACKABLE,
				&ags_packable_interface_info);
  }

  return(ags_type_recall_channel);
}

void
ags_recall_channel_class_init(AgsRecallChannelClass *recall_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_channel_parent_class = g_type_class_peek_parent(recall_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_channel;

  gobject->set_property = ags_recall_channel_set_property;
  gobject->get_property = ags_recall_channel_get_property;

  gobject->finalize = ags_recall_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("destination\0",
				   "assigned destination channel\0",
				   "The destination channel object it is assigned to\0",
				    AGS_TYPE_CHANNEL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
   g_object_class_install_property(gobject,
				   PROP_DESTINATION,
				   param_spec);

   param_spec = g_param_spec_object("source\0",
				    "assigned source channel\0",
				    "The source channel object it is assigned to\0",
				    AGS_TYPE_CHANNEL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
   g_object_class_install_property(gobject,
				   PROP_SOURCE,
				   param_spec);

   /* AgsRecallClass */
   recall = (AgsRecallClass *) recall_channel;

   recall->duplicate = ags_recall_channel_duplicate;
 }

 void
 ags_recall_channel_connectable_interface_init(AgsConnectableInterface *connectable)
 {
   AgsConnectableInterface *ags_recall_channel_connectable_parent_interface;

   ags_recall_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

   connectable->connect = ags_recall_channel_connect;
   connectable->disconnect = ags_recall_channel_disconnect;
 }

 void
 ags_recall_channel_packable_interface_init(AgsPackableInterface *packable)
 {
   ags_recall_channel_parent_packable_interface = g_type_interface_peek_parent(packable);

   packable->pack = ags_recall_channel_pack;
   packable->unpack = ags_recall_channel_unpack;
 }

 void
 ags_recall_channel_init(AgsRecallChannel *recall_channel)
 {
   recall_channel->destination = NULL;
   recall_channel->source = NULL;
 }

 void
 ags_recall_channel_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
 {
   AgsRecallChannel *recall_channel;

   recall_channel = AGS_RECALL_CHANNEL(gobject);

   switch(prop_id){
   case PROP_DESTINATION:
     {
       AgsChannel *destination;

       destination = (AgsChannel *) g_value_get_object(value);

       if(recall_channel->destination == destination)
	 return;

       if(recall_channel->destination != NULL)
	 g_object_unref(recall_channel->destination);

       if(destination != NULL)
	 g_object_ref(destination);

       recall_channel->destination = destination;
     }
     break;
   case PROP_SOURCE:
     {
       AgsChannel *source;

       source = (AgsChannel *) g_value_get_object(value);

       if(recall_channel->source == source)
	 return;

       if(recall_channel->source != NULL)
	 g_object_unref(recall_channel->source);

      if(source != NULL)
	g_object_ref(source);

      recall_channel->source = source;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_channel_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_DESTINATION:
    g_value_set_object(value, recall_channel->destination);
    break;
  case PROP_SOURCE:
    g_value_set_object(value, recall_channel->source);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_channel_finalize(GObject *gobject)
{
  AgsRecallChannel *recall_channel;

  recall_channel = AGS_RECALL_CHANNEL(gobject);

  if(recall_channel->source != NULL)
    g_object_unref(recall_channel->source);

  if(recall_channel->destination != NULL)
    g_object_unref(G_OBJECT(recall_channel->destination));

  G_OBJECT_CLASS(ags_recall_channel_parent_class)->finalize(gobject);
}

void
ags_recall_channel_connect(AgsConnectable *connectable)
{
  ags_recall_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_channel_disconnect(AgsConnectable *connectable)
{
  ags_recall_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

gboolean
ags_recall_channel_pack(AgsPackable *packable, GObject *container)
{
  AgsRecallContainer *recall_container;
  AgsRecallChannel *recall_channel;
  GList *list;

  if(ags_recall_channel_parent_packable_interface->pack(packable, container))
    return(TRUE);

  recall_container = AGS_RECALL_CONTAINER(container);
  recall_channel = AGS_RECALL_CHANNEL(packable);

  g_object_set(container,
	       "recall_channel\0", recall_channel,
	       NULL);

  /* set in AgsRecallChannelRun */
  list = recall_container->recall_channel_run;

  g_message("debug\0");

  while((list = ags_recall_find_provider(list, G_OBJECT(recall_channel->source))) != NULL){
    g_object_set(G_OBJECT(list->data),
		 "recall_channel\0", recall_channel,
		 NULL);

    list = list->next;
  }

  return(FALSE);
}

gboolean
ags_recall_channel_unpack(AgsPackable *packable)
{
  AgsRecall *recall;
  AgsRecallContainer *recall_container;
  GList *list;
  
  recall = AGS_RECALL(packable);

  if(recall == NULL)
    return(TRUE);

  recall_container = AGS_RECALL_CONTAINER(recall->container);

  if(recall_container == NULL)
    return(TRUE);

  /* ref */
  g_object_ref(recall);
  g_object_ref(recall_container);

  /* unset in AgsRecallChannelRun */
  list = recall_container->recall_channel_run;

  while((list = ags_recall_find_provider(list, G_OBJECT(AGS_RECALL_CHANNEL(packable)->source))) != NULL){
    g_object_set(G_OBJECT(list->data),
		 "recall_channel\0", NULL,
		 NULL);

    list = list->next;
  }

  /* call parent */
  if(ags_recall_channel_parent_packable_interface->unpack(packable)){
    g_object_unref(recall);
    g_object_unref(recall_container);
  
    return(TRUE);
  }

  /* remove from list */
  recall_container->recall_channel = g_list_remove(recall_container->recall_channel,
						   recall);

  /* unref */
  g_object_unref(recall);
  g_object_unref(recall_container);

  return(FALSE);
}

AgsRecall*
ags_recall_channel_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, GParameter *parameter)
{
  AgsRecallChannel *recall_channel, *copy;

  recall_channel = AGS_RECALL_CHANNEL(recall);

  parameter = ags_parameter_grow(G_OBJECT_TYPE(recall),
				 parameter, n_params,
				 "source\0", recall_channel->source,
				 "destination\0", recall_channel->destination,
				 NULL);

  copy = AGS_RECALL_CHANNEL(AGS_RECALL_CLASS(ags_recall_channel_parent_class)->duplicate(recall,
											 recall_id,
											 n_params, parameter));

  g_message("ags warning - ags_recall_channel_duplicate: you shouldn't do this %s\n\0", G_OBJECT_TYPE_NAME(recall));

  return((AgsRecall *) copy);
}

GList*
ags_recall_channel_find_channel(GList *recall_channel_i, AgsChannel *source)
{
  AgsRecallChannel *recall_channel;

  while(recall_channel_i != NULL){
    recall_channel = AGS_RECALL_CHANNEL(recall_channel_i->data);

    if(recall_channel->source == source)
      return(recall_channel_i);

    recall_channel_i = recall_channel_i->next;
  }

  return(NULL);
}

AgsRecallChannel*
ags_recall_channel_new()
{
  AgsRecallChannel *recall_channel;

  recall_channel = (AgsRecallChannel *) g_object_new(AGS_TYPE_RECALL_CHANNEL,
						     NULL);

  return(recall_channel);
}
