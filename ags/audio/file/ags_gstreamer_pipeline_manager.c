/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/audio/file/ags_gstreamer_pipeline_manager.h>

#include <ags/audio/file/ags_gstreamer_pipeline_helper.h>
#include <ags/audio/file/ags_gstreamer_file.h>

#include <ags/libags.h>

#include <stdlib.h>

#include <ags/i18n.h>

static void ags_gstreamer_pipeline_manager_class_init(AgsGstreamerPipelineManagerClass *gstreamer_pipeline_manager);
static void ags_gstreamer_pipeline_manager_init(AgsGstreamerPipelineManager *gstreamer_pipeline_manager);
static void ags_gstreamer_pipeline_manager_set_property(GObject *gobject,
							guint prop_id,
							const GValue *value,
							GParamSpec *param_spec);
static void ags_gstreamer_pipeline_manager_get_property(GObject *gobject,
							guint prop_id,
							GValue *value,
							GParamSpec *param_spec);
static void ags_gstreamer_pipeline_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_gstreamer_pipeline_manager
 * @short_description: The audio file manager
 * @title: AgsGstreamerPipelineManager
 * @section_id:
 * @include: ags/gstreamer_pipeline/ags_gstreamer_pipeline_manager.h
 *
 * #AgsGstreamerPipelineManager stores audio files.
 */

enum{
  PROP_0,
};

static gpointer ags_gstreamer_pipeline_manager_parent_class = NULL;

static AgsGstreamerPipelineManager *ags_gstreamer_pipeline_manager = NULL;

GType
ags_gstreamer_pipeline_manager_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_gstreamer_pipeline_manager = 0;

    static const GTypeInfo ags_gstreamer_pipeline_manager_info = {
      sizeof (AgsGstreamerPipelineManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_pipeline_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGstreamerPipelineManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_pipeline_manager_init,
    };

    ags_type_gstreamer_pipeline_manager = g_type_register_static(G_TYPE_OBJECT,
								 "AgsGstreamerPipelineManager", &ags_gstreamer_pipeline_manager_info,
								 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_gstreamer_pipeline_manager);
  }

  return(g_define_type_id__static);
}

void
ags_gstreamer_pipeline_manager_class_init(AgsGstreamerPipelineManagerClass *gstreamer_pipeline_manager)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_gstreamer_pipeline_manager_parent_class = g_type_class_peek_parent(gstreamer_pipeline_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) gstreamer_pipeline_manager;

  gobject->set_property = ags_gstreamer_pipeline_manager_set_property;
  gobject->get_property = ags_gstreamer_pipeline_manager_get_property;

  gobject->finalize = ags_gstreamer_pipeline_manager_finalize;

  /* properties */
}

void
ags_gstreamer_pipeline_manager_init(AgsGstreamerPipelineManager *gstreamer_pipeline_manager)
{
  gstreamer_pipeline_manager->gstreamer_pipeline = NULL;
}

void
ags_gstreamer_pipeline_manager_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec)
{
  AgsGstreamerPipelineManager *gstreamer_pipeline_manager;

  GRecMutex *gstreamer_pipeline_manager_mutex;

  gstreamer_pipeline_manager = AGS_GSTREAMER_PIPELINE_MANAGER(gobject);

  /* get audio file manager mutex */
  gstreamer_pipeline_manager_mutex = AGS_GSTREAMER_PIPELINE_MANAGER_GET_OBJ_MUTEX(gstreamer_pipeline_manager);
}

void
ags_gstreamer_pipeline_manager_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec)
{
  AgsGstreamerPipelineManager *gstreamer_pipeline_manager;

  GRecMutex *gstreamer_pipeline_manager_mutex;

  gstreamer_pipeline_manager = AGS_GSTREAMER_PIPELINE_MANAGER(gobject);

  /* get audio file manager mutex */
  gstreamer_pipeline_manager_mutex = AGS_GSTREAMER_PIPELINE_MANAGER_GET_OBJ_MUTEX(gstreamer_pipeline_manager);
}

void
ags_gstreamer_pipeline_manager_finalize(GObject *gobject)
{
  AgsGstreamerPipelineManager *gstreamer_pipeline_manager;

  gstreamer_pipeline_manager = (AgsGstreamerPipelineManager *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_pipeline_manager_parent_class)->finalize(gobject);
}

GRecMutex*
ags_gstreamer_pipeline_manager_get_obj_mutex(AgsGstreamerPipelineManager *gstreamer_pipeline_manager)
{
  return(&(gstreamer_pipeline_manager->obj_mutex));
}

void
ags_gstreamer_pipeline_manager_create_ro_pipeline(AgsGstreamerPipelineManager *gstreamer_pipeline_manager,
						  GObject *gstreamer_file)
{
  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  
  _Atomic gboolean create_pipeline_completed;

  ags_atomic_boolean_set(&create_pipeline_completed,
			 FALSE);	
  
  message_delivery = ags_message_delivery_get_instance();
  
  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   BAD_CAST "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       BAD_CAST "method",
	       BAD_CAST "AgsGstreamerPipelineManager::create-ro-pipeline");

    /* add message */
    message = ags_message_envelope_new((GObject *) gstreamer_pipeline_manager,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* sound scope */
    message->parameter_name[0] = "gstreamer-file";
    g_value_init(&(message->value[0]),
		 G_TYPE_OBJECT);
    g_value_set_object(&(message->value[0]),
		       gstreamer_file);

    /* recall id */
    message->parameter_name[1] = "create-pipeline-completed";
    g_value_init(&(message->value[1]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(message->value[1]),
			&create_pipeline_completed);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      (GObject *) message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }else{
    ags_gstreamer_pipeline_helper_create_ro_pipeline(gstreamer_file);
    
    ags_atomic_boolean_set(&create_pipeline_completed,
			   TRUE);
  }
  
  while(!ags_atomic_boolean_get(&create_pipeline_completed)){
    g_usleep(4);
  }
}

void
ags_gstreamer_pipeline_manager_create_rw_pipeline(AgsGstreamerPipelineManager *gstreamer_pipeline_manager,
						  GObject *gstreamer_file)
{
  AgsMessageDelivery *message_delivery;

  GList *start_message_queue;
  
  _Atomic gboolean create_pipeline_completed;

  ags_atomic_boolean_set(&create_pipeline_completed,
			 FALSE);	
  
  message_delivery = ags_message_delivery_get_instance();
  
  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   BAD_CAST "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       BAD_CAST "method",
	       BAD_CAST "AgsGstreamerPipelineManager::create-rw-pipeline");

    /* add message */
    message = ags_message_envelope_new((GObject *) gstreamer_pipeline_manager,
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 2;

    message->parameter_name = (gchar **) malloc(3 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    2);

    /* sound scope */
    message->parameter_name[0] = "gstreamer-file";
    g_value_init(&(message->value[0]),
		 G_TYPE_OBJECT);
    g_value_set_object(&(message->value[0]),
		       gstreamer_file);

    /* recall id */
    message->parameter_name[1] = "create-pipeline-completed";
    g_value_init(&(message->value[1]),
		 G_TYPE_POINTER);
    g_value_set_pointer(&(message->value[1]),
			&create_pipeline_completed);

    /* terminate string vector */
    message->parameter_name[2] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      (GObject *) message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }else{
    ags_gstreamer_pipeline_helper_create_rw_pipeline(gstreamer_file);
    
    ags_atomic_boolean_set(&create_pipeline_completed,
			   TRUE);
  }
  
  while(!ags_atomic_boolean_get(&create_pipeline_completed)){
    g_usleep(4);
  }
}

/**
 * ags_gstreamer_pipeline_manager_get_instance:
 * 
 * Get ags audio file manager instance.
 *
 * Returns: (transfer none): the #AgsGstreamerPipelineManager singleton
 * 
 * Since: 9.1.0
 */
AgsGstreamerPipelineManager*
ags_gstreamer_pipeline_manager_get_instance()
{
  static GMutex mutex = {0,};

  g_mutex_lock(&mutex);

  if(ags_gstreamer_pipeline_manager == NULL){
    ags_gstreamer_pipeline_manager = ags_gstreamer_pipeline_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_gstreamer_pipeline_manager);
}

/**
 * ags_gstreamer_pipeline_manager_new:
 *
 * Creates an #AgsGstreamerPipelineManager
 *
 * Returns: a new #AgsGstreamerPipelineManager
 *
 * Since: 9.1.0
 */
AgsGstreamerPipelineManager*
ags_gstreamer_pipeline_manager_new()
{
  AgsGstreamerPipelineManager *gstreamer_pipeline_manager;

  gstreamer_pipeline_manager = (AgsGstreamerPipelineManager *) g_object_new(AGS_TYPE_GSTREAMER_PIPELINE_MANAGER,
									    NULL);
  
  return(gstreamer_pipeline_manager);
}
