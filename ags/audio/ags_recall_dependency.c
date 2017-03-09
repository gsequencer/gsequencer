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

#include <ags/audio/ags_recall_dependency.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <stdio.h>

void ags_recall_dependency_class_init(AgsRecallDependencyClass *recall_dependency);
void ags_recall_dependency_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_dependency_init(AgsRecallDependency *recall_dependency);
void ags_recall_dependency_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_recall_dependency_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_recall_dependency_connect(AgsConnectable *connectable);
void ags_recall_dependency_disconnect(AgsConnectable *connectable);
void ags_recall_dependency_dispose(GObject *gobject);
void ags_recall_dependency_finalize(GObject *gobject);

/**
 * SECTION:ags_recall_dependency
 * @short_description: Object specifing dependency
 * @title: AgsRecallDependency
 * @section_id:
 * @include: ags/audio/ags_recall_dependency.h
 *
 * #AgsRecallDependency specifies dependencies on other recalls. Dependencies
 * are resolved during initialization.
 */

static gpointer ags_recall_dependency_parent_class = NULL;

enum{
  PROP_0,
  PROP_DEPENDENCY,
};

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
  GParamSpec *param_spec;
  
  ags_recall_dependency_parent_class = g_type_class_peek_parent(recall_dependency);

  gobject = (GObjectClass *) recall_dependency;

  gobject->set_property = ags_recall_dependency_set_property;
  gobject->get_property = ags_recall_dependency_get_property;
  
  gobject->dispose = ags_recall_dependency_dispose;
  gobject->finalize = ags_recall_dependency_finalize;

  /* properties */
  /**
   * AgsRecallDependency:dependency:
   *
   * The dependency.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("dependency\0",
				   "dependency of recall\0",
				   "A dependency of the recall\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEPENDENCY,
				  param_spec);
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
ags_recall_dependency_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = AGS_RECALL_DEPENDENCY(gobject);
  
  switch(prop_id){
  case PROP_DEPENDENCY:
    {
      AgsRecall *dependency;
      
      dependency = (AgsRecall *) g_value_get_object(value);
      
      if(recall_dependency->dependency == dependency){
	return;
      }
	
      if(recall_dependency->dependency != NULL){
	g_object_unref(G_OBJECT(recall_dependency->dependency));
      }

      if(dependency != NULL){
	g_object_ref(G_OBJECT(dependency));
      }

      recall_dependency->dependency = dependency;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_dependency_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = AGS_RECALL_DEPENDENCY(gobject);

  switch(prop_id){
  case PROP_DEPENDENCY:
    {
      g_value_set_object(value, recall_dependency->dependency);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
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
ags_recall_dependency_dispose(GObject *gobject)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = AGS_RECALL_DEPENDENCY(gobject);

  /* dependency */
  if(recall_dependency->dependency != NULL){
    g_object_unref(recall_dependency->dependency);

    recall_dependency->dependency = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_dependency_parent_class)->dispose(gobject);
}

void
ags_recall_dependency_finalize(GObject *gobject)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = AGS_RECALL_DEPENDENCY(gobject);

  /* dependency */
  if(recall_dependency->dependency != NULL){
    g_object_unref(recall_dependency->dependency);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_dependency_parent_class)->finalize(gobject);
}

/**
 * ags_recall_dependency_find_dependency:
 * @recall_dependencies: a #GList containing  #AgsRecallDependency
 * @dependency: the #AgsRecall depending on
 *
 * Retrieve dependency.
 *
 * Returns: Next match.
 * 
 * Since: 0.4.0
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
 * @recall_dependencies: a #GList containing  #AgsRecallDependency
 * @provider: the object providing recall, like #AgsAudio or #AgsChannel
 *
 * Retrieve dependency by provider.
 *
 * Returns: Next match.
 * 
 * Since: 0.4.0
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
 * @recall_dependency: an #AgsRecallDependency
 * @recall_id: the #AgsRecallID refering to
 *
 * Resolve dependency.
 *
 * Returns: the #AgsRecall dependency.
 * 
 * Since: 0.4.0
 */
GObject*
ags_recall_dependency_resolve(AgsRecallDependency *recall_dependency, AgsRecallID *recall_id)
{
  AgsRecallContainer *recall_container;
  AgsRecall *dependency;
  
  if(recall_dependency->dependency == NULL ||
     AGS_RECALL(recall_dependency->dependency)->container == NULL){
    g_message("dependency resolve: container == NULL");
    return(NULL);
  }

  dependency = AGS_RECALL(recall_dependency->dependency);
  recall_container = AGS_RECALL_CONTAINER(dependency->container);
  
  if(AGS_IS_RECALL_AUDIO(dependency)){
    return((GObject *) recall_container->recall_audio);
  }else if(AGS_IS_RECALL_AUDIO_RUN(dependency)){
    GList *recall_list;

    if(recall_id == NULL){
      g_message("dependency resolve: recall_id == NULL");
      return(NULL);
    }

    recall_list = ags_recall_find_recycling_context(recall_container->recall_audio_run,
						    (GObject *) recall_id->recycling_context);

    if(recall_list != NULL){
      return(G_OBJECT(recall_list->data));
    }else if(recall_id->recycling_context->parent != NULL){
      recall_list = ags_recall_find_recycling_context(recall_container->recall_audio_run,
						      (GObject *) recall_id->recycling_context->parent);

      if(recall_list != NULL){
	return(G_OBJECT(recall_list->data));
      }else{
	g_warning("dependency not found!\0");
      }
    }
  }else if(AGS_IS_RECALL_CHANNEL(dependency)){
    GList *recall_list;

    recall_list = ags_recall_find_provider(recall_container->recall_channel,
					   (GObject *) AGS_RECALL_CHANNEL(dependency)->source);

    if(recall_list != NULL){
      return(G_OBJECT(recall_list->data));
    }
  }else if(AGS_IS_RECALL_CHANNEL_RUN(dependency)){
    GList *recall_list;

    if(recall_id == NULL){
      g_message("dependency resolve: recall_id == NULL");
      return(NULL);
    }

    recall_list = ags_recall_find_provider_with_recycling_context(recall_container->recall_channel_run,
								  (GObject *) AGS_RECALL_CHANNEL_RUN(dependency)->recall_channel->source,
								  (GObject *) recall_id->recycling_context);

    if(recall_list != NULL){
      return(G_OBJECT(recall_list->data));
    }else if(recall_id->recycling_context->parent != NULL){
      recall_list = ags_recall_find_provider_with_recycling_context(recall_container->recall_channel_run,
								    (GObject *) AGS_RECALL_CHANNEL_RUN(dependency)->recall_channel->source,
								    (GObject *) recall_id->recycling_context->parent);

      if(recall_list != NULL){
	return(G_OBJECT(recall_list->data));
      }      
    }
  }

  return(NULL);
}

/**
 * ags_recall_dependency_new:
 * @dependency: the #AgsRecall depending on
 *
 * Creates a #AgsRecallDependency
 *
 * Returns: a new #AgsRecallDependency
 * 
 * Since: 0.4.0
 */
AgsRecallDependency*
ags_recall_dependency_new(GObject *dependency)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = (AgsRecallDependency *) g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
							   "dependency\0", dependency,
							   NULL);

  return(recall_dependency);
}
