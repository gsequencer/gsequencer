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

#include <ags/audio/ags_recall_dependency.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <stdio.h>

void ags_recall_dependency_class_init(AgsRecallDependencyClass *recall_dependency);
void ags_recall_dependency_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_dependency_init(AgsRecallDependency *recall_dependency);
void ags_recall_dependency_connect(AgsConnectable *connectable);
void ags_recall_dependency_disconnect(AgsConnectable *connectable);
void ags_recall_dependency_finalize(GObject *gobject);

/**
 * SECTION:agsrecalldependency
 * @Short_description: Object specifing dependency
 * @Title: AgsRecallDependency
 *
 * #AgsRecallDependency specifies dependencies on other recalls. Dependencies
 * are resolved during initialization.
 */

static gpointer ags_recall_dependency_parent_class = NULL;

GType
ags_recall_dependency_get_type(void)
{
  static GType ags_type_recall_dependency = 0;

  if(!ags_type_recall_dependency){
    static const GTypeInfo ags_recall_dependency_info = {
      sizeof (AgsRecallDependencyClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_dependency_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallDependency),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_dependency_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_dependency_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_dependency = g_type_register_static(G_TYPE_OBJECT,
							"AgsRecallDependency\0",
							&ags_recall_dependency_info,
							0);
    
    g_type_add_interface_static(ags_type_recall_dependency,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_recall_dependency);
}

void
ags_recall_dependency_class_init(AgsRecallDependencyClass *recall_dependency)
{
  GObjectClass *gobject;

  ags_recall_dependency_parent_class = g_type_class_peek_parent(recall_dependency);

  gobject = (GObjectClass *) recall_dependency;
  gobject->finalize = ags_recall_dependency_finalize;
}

void
ags_recall_dependency_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recall_dependency_connect;
  connectable->disconnect = ags_recall_dependency_disconnect;
}

void
ags_recall_dependency_init(AgsRecallDependency *recall_dependency)
{
  recall_dependency->dependency = NULL;
}

void
ags_recall_dependency_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_recall_dependency_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_recall_dependency_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_dependency_parent_class)->finalize(gobject);
}

/**
 * ags_recall_dependency_find_dependency:
 * @recall_dependencies a #GList containing  #AgsRecallDependency
 * @dependency the #AgsRecall depending on
 *
 * Retrieve dependency.
 *
 * Returns: Next match.
 */
GList*
ags_recall_dependency_find_dependency(GList *recall_dependencies, GObject *dependency)
{
  AgsRecallDependency *recall_dependency;

  while(recall_dependencies != NULL){
    recall_dependency = AGS_RECALL_DEPENDENCY(recall_dependencies->data);

    if(recall_dependency->dependency == dependency){
      return(recall_dependencies);
    }

    recall_dependencies = recall_dependencies->next;
  }

  return(NULL);
}

/**
 * ags_recall_dependency_find_dependency_by_provider:
 * @recall_dependencies a #GList containing  #AgsRecallDependency
 * @provider the object providing recall, like #AgsAudio or #AgsChannel
 *
 * Retrieve dependency by provider.
 *
 * Returns: Next match.
 */
GList*
ags_recall_dependency_find_dependency_by_provider(GList *recall_dependencies,
						  GObject *provider)
{
  AgsRecallDependency *recall_dependency;

  while(recall_dependencies != NULL){
    recall_dependency = AGS_RECALL_DEPENDENCY(recall_dependencies->data);

    if((AGS_IS_CHANNEL(provider) &&
	AGS_IS_RECALL_CHANNEL_RUN(recall_dependency->dependency) &&
	(AGS_RECALL_CHANNEL_RUN(recall_dependency->dependency)->source == AGS_CHANNEL(provider))) ||
       (AGS_IS_AUDIO(provider) &&
	AGS_IS_RECALL_AUDIO_RUN(recall_dependency->dependency) &&
	(AGS_RECALL_AUDIO_RUN(recall_dependency->dependency)->recall_audio->audio == AGS_AUDIO(provider)))){
      return(recall_dependencies);
    }

    recall_dependencies = recall_dependencies->next;
  }

  return(NULL);
}

/**
 * ags_recall_dependency_resolve:
 * @recall_dependeny an #AgsRecallDependency
 * @recall_id the #AgsRecallID refering to
 *
 * Resolve dependency.
 *
 * Returns: the #AgsRecall dependency.
 */
GObject*
ags_recall_dependency_resolve(AgsRecallDependency *recall_dependency, AgsRecallID *recall_id)
{
  AgsRecallContainer *recall_container;
  AgsRecall *dependency;
  
  if(recall_dependency->dependency == NULL ||
     AGS_RECALL(recall_dependency->dependency)->container == NULL){
    return(NULL);
  }

  dependency = AGS_RECALL(recall_dependency->dependency);
  recall_container = AGS_RECALL_CONTAINER(AGS_RECALL(recall_dependency->dependency)->container);
  
  if(AGS_IS_RECALL_AUDIO(dependency)){
    return((GObject *) recall_container->recall_audio);
  }else if(AGS_IS_RECALL_AUDIO_RUN(dependency)){
    GList *recall_list;

    recall_list = ags_recall_find_recycling_container(recall_container->recall_audio_run,
						      recall_id->recycling_container);

    if(recall_list != NULL)
      return(G_OBJECT(recall_list->data));
  }else if(AGS_IS_RECALL_CHANNEL(dependency)){
    GList *recall_list;

    recall_list = ags_recall_find_provider(recall_container->recall_channel,
					   (GObject *) AGS_RECALL_CHANNEL(dependency)->source);

    if(recall_list != NULL)
      return(G_OBJECT(recall_list->data));
  }else if(AGS_IS_RECALL_CHANNEL_RUN(dependency)){
    GList *recall_list;

    recall_list = ags_recall_find_provider_with_recycling_container(recall_container->recall_channel_run,
								    (GObject *) AGS_RECALL_CHANNEL_RUN(dependency)->recall_channel->source,
								    recall_id->recycling_container);

    if(recall_list != NULL)
      return(G_OBJECT(recall_list->data));
  }

  return(NULL);
}

/**
 * ags_recall_dependency_new:
 * @dependency the #AgsRecall depending on
 *
 * Creates a #AgsRecallDependency
 *
 * Returns: a new #AgsRecallDependency
 */
AgsRecallDependency*
ags_recall_dependency_new(GObject *dependency)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = (AgsRecallDependency *) g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
							   NULL);

  recall_dependency->dependency = dependency;

  return(recall_dependency);
}
