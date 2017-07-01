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

#include <ags/thread/file/ags_thread_file_xml.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_main_loop.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>

#include <ags/thread/ags_thread_application_context.h>
#include <ags/thread/ags_returnable_thread.h>

void ags_file_read_thread_start(AgsFileLaunch *file_launch, AgsThread *thread);
void ags_file_read_thread_pool_start(AgsFileLaunch *file_launch, AgsThreadPool *thread_pool);

void
ags_file_read_thread(AgsFile *file, xmlNode *node, AgsThread **thread)
{
  AgsFileLaunch *file_launch;

  AgsThread *gobject;

  xmlNode *child;
  xmlChar *type_name;

  guint orig_flags;
  
  static gboolean thread_type_is_registered = FALSE;

  if(*thread != NULL &&
     AGS_IS_RETURNABLE_THREAD(*thread)){
    return;
  }

  if(*thread == NULL){
    GType type;

    if(!thread_type_is_registered){
      ags_thread_application_context_register_types(AGS_APPLICATION_CONTEXT(file->application_context));

      thread_type_is_registered = TRUE;
    }

    type_name = xmlGetProp(node,
			   AGS_FILE_TYPE_PROP);

    type = g_type_from_name(type_name);

    if(type == AGS_TYPE_RETURNABLE_THREAD){
      return;
    }

    gobject = g_object_new(type,
			   NULL);

    *thread = gobject;
  }else{
    gobject = *thread;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  orig_flags = (guint) g_ascii_strtoull(xmlGetProp(node,
						   AGS_FILE_FLAGS_PROP),
					NULL,
					16);
  
  g_atomic_int_set(&(gobject->flags),
		   ((~(AGS_THREAD_RUNNING)) & (guint) g_ascii_strtoull(xmlGetProp(node,
										  AGS_FILE_FLAGS_PROP),
								       NULL,
								       16)));
  
  g_atomic_int_set(&(gobject->sync_flags),
		   ((~(AGS_THREAD_WAIT_0 |
		       AGS_THREAD_WAIT_1 |
		       AGS_THREAD_WAIT_2)) & (guint) g_ascii_strtoull(xmlGetProp(node,
										 "sync-flags"),
								      NULL,
								      16)));

  /* start */
  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(orig_flags)))) != 0){
    file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
						 NULL);
    ags_file_add_launch(file, (GObject *) file_launch);
    g_signal_connect(G_OBJECT(file_launch), "start",
		     G_CALLBACK(ags_file_read_thread_start), gobject);
  }

  /* read children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-thread-list",
		     16)){
	GList *list;
	
	list = NULL;

	ags_file_read_thread_list(file,
				  child,
				  &list);
	
	while(list != NULL){
	  ags_thread_add_child(gobject,
			       list->data);

	  list = list->next;
	}
      }
    }

    child = child->next;
  }
}

void
ags_file_read_thread_start(AgsFileLaunch *file_launch, AgsThread *thread)
{
  g_atomic_int_and(&(thread->flags),
		   ~AGS_THREAD_RUNNING);
  ags_thread_start(thread);

  /* wait thread */
  pthread_mutex_lock(thread->start_mutex);

  g_atomic_int_set(&(thread->start_wait),
		   TRUE);
	
  if(g_atomic_int_get(&(thread->start_wait)) == TRUE &&
     g_atomic_int_get(&(thread->start_done)) == FALSE){
    while(g_atomic_int_get(&(thread->start_wait)) == TRUE &&
	  g_atomic_int_get(&(thread->start_done)) == FALSE){
      pthread_cond_wait(thread->start_cond,
			thread->start_mutex);
    }
  }
	
  pthread_mutex_unlock(thread->start_mutex);
}

xmlNode*
ags_file_write_thread(AgsFile *file, xmlNode *parent, AgsThread *thread)
{
  AgsThread *current;
  xmlNode *node, *child;
  gchar *id;

  if(AGS_IS_RETURNABLE_THREAD(thread)){
    return(NULL);
  }

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-thread");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", thread,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_TYPE_PROP,
	     G_OBJECT_TYPE_NAME(thread));

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x", g_atomic_int_get(&(thread->flags))));

  xmlNewProp(node,
	     "sync-flags",
	     g_strdup_printf("%x", g_atomic_int_get(&(thread->sync_flags))));

  /* add to parent */
  xmlAddChild(parent,
	      node);

  /* child elements */
  current = g_atomic_pointer_get(&(thread->children));

  child = xmlNewNode(NULL,
		     "ags-thread-list");
  xmlAddChild(node,
	      child);

  while(current != NULL){
    ags_file_write_thread(file,
			  child,
			  current);
    
    current = g_atomic_pointer_get(&(current->next));
  }
}

void
ags_file_read_thread_list(AgsFile *file, xmlNode *node, GList **thread)
{
  AgsThread *current;
  GList *list;
  xmlNode *child;
  xmlChar *id;

  id = xmlGetProp(node, AGS_FILE_ID_PROP);

  child = node->children;
  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-thread",
		     11)){
	current = NULL;
	ags_file_read_thread(file, child, &current);
	list = g_list_prepend(list, current);
      }
    }

    child = child->next;
  }

  //  list = g_list_reverse(list);
  *thread = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", list,
				   NULL));
}

xmlNode*
ags_file_write_thread_list(AgsFile *file, xmlNode *parent, GList *thread)
{
  AgsThread *current;
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-thread-list");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", list,
				   NULL));

  xmlAddChild(parent,
	      node);

  //TODO:JK: generate id and add id ref

  list = thread;

  while(list != NULL){
    ags_file_write_thread(file, node, AGS_THREAD(list->data));

    list = list->next;
  }

  return(node);
}

void
ags_file_read_thread_pool(AgsFile *file, xmlNode *node, AgsThreadPool **thread_pool)
{
  AgsThreadPool *gobject;
  AgsFileLookup *file_lookup;
  xmlNode *child;
  xmlChar *prop, *content;

  if(*thread_pool == NULL){
    gobject = g_object_new(AGS_TYPE_THREAD_POOL,
			   NULL);
    *thread_pool = gobject;
  }else{
    gobject = *thread_pool;
  }

  //TODO:JK: implement me
  //  g_object_set(G_OBJECT(gobject),
  //	       "ags-main", file->application_context,
  //	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  gobject->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);
}

xmlNode*
ags_file_write_thread_pool(AgsFile *file, xmlNode *parent, AgsThreadPool *thread_pool)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  gchar *id;
  guint i;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-thread_pool");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", thread_pool,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x", thread_pool->flags));
}

void
ags_file_read_thread_pool_start(AgsFileLaunch *file_launch, AgsThreadPool *thread_pool)
{
  ags_thread_pool_start(thread_pool);
}

void
ags_file_read_timestamp(AgsFile *file, xmlNode *node, AgsTimestamp **timestamp)
{
  AgsTimestamp *gobject;

  if(*timestamp == NULL){
    gobject = (AgsTimestamp *) g_object_new(AGS_TYPE_TIMESTAMP,
					    NULL);

    *timestamp = gobject;
  }else{
    gobject = *timestamp;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));
  
  gobject->flags = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  gobject->delay = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								 (xmlChar *) "delay"),
					    NULL,
					    10);
  
  gobject->attack = (guint) g_ascii_strtoull((gchar *) xmlGetProp(node,
								  (xmlChar *) "attack"),
					     NULL,
					     10);

  gobject->timer.unix_time.time_val = (guint) g_ascii_strtoull((gchar *) node->content,
							       NULL,
							       10);
}

xmlNode*
ags_file_write_timestamp(AgsFile *file, xmlNode *parent, AgsTimestamp *timestamp)
{
  xmlNode *node;
  gchar *id;

  if(timestamp == NULL){
    return(NULL);
  }

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-timestamp");
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", timestamp,
				   NULL));
  
  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_FLAGS_PROP,
	     (xmlChar *) g_strdup_printf("%x", timestamp->flags));

  xmlNewProp(node,
	     (xmlChar *) "delay",
	     (xmlChar *) g_strdup_printf("%d", timestamp->delay));

  xmlNewProp(node,
	     (xmlChar *) "attack",
	     (xmlChar *) g_strdup_printf("%d", timestamp->attack));

  xmlAddChild(parent,
	      node);

  xmlNodeAddContent(node,
		    (xmlChar *) g_strdup_printf("%li", timestamp->timer.unix_time.time_val));

  return(node);
}

void
ags_file_read_timestamp_list(AgsFile *file, xmlNode *node, GList **timestamp)
{
  AgsTimestamp *current;
  xmlNode *child;
  GList *list;

  child = node->children;

  list = NULL;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     (xmlChar *) "ags-timestamp",
		     14)){
	current = NULL;
    
	ags_file_read_timestamp(file, child,
				&current);
    
	list = g_list_prepend(list,
			      current);
      }
    }
    
    child = child->next;
  }
  
  list = g_list_reverse(list);

  *timestamp = list;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", (gchar *) xmlGetProp(node,
													    (xmlChar *) AGS_FILE_ID_PROP)),
				   "reference", list,
				   NULL));
}

xmlNode*
ags_file_write_timestamp_list(AgsFile *file, xmlNode *parent, GList *timestamp)
{
  xmlNode *node;
  GList *list;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    (xmlChar *) "ags-timestamp-list");

  xmlNewProp(node,
	     (xmlChar *) AGS_FILE_ID_PROP,
	     (xmlChar *) id);
  
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", timestamp,
				   NULL));

  xmlAddChild(parent,
	      node);

  list = timestamp;
  
  while(list != NULL){
    ags_file_write_timestamp(file,
			     node,
			     AGS_TIMESTAMP(list->data));
    
    list = list->next;
  }

  return(node);
}
