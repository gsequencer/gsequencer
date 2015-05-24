/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/thread/file/ags_thread_file_xml.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

void ags_file_read_thread_start(AgsFileLaunch *file_launch, AgsThread *thread);

void ags_file_read_thread_pool_start(AgsFileLaunch *file_launch, AgsThreadPool *thread_pool);

void
ags_file_read_thread(AgsFile *file, xmlNode *node, AgsThread **thread)
{
  AgsFileLookup *file_lookup;
  AgsFileLaunch *file_launch;
  AgsThread *gobject;
  xmlNode *child;
  xmlChar *type_name;

  if(*thread != NULL &&
     AGS_IS_RETURNABLE_THREAD(*thread)){
    return;
  }

  if(*thread == NULL){
    GType type;

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

  g_message(G_OBJECT_TYPE_NAME(gobject));

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  g_atomic_int_set(&(gobject->flags),
		   ((~(AGS_THREAD_WAIT_0 |
		       AGS_THREAD_WAIT_1 |
		       AGS_THREAD_WAIT_2 |
		       AGS_THREAD_RUNNING)) & (guint) g_ascii_strtoull(xmlGetProp(node,
										  AGS_FILE_FLAGS_PROP),
								       NULL,
								       16)));

  /* start */
  if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(gobject->flags)))) != 0){
    //FIXME:JK: workaround file setting AGS_THREAD_RUNNING is just ignored
    if(AGS_IS_MAIN_LOOP(gobject)){
      file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
						   NULL);
      ags_file_add_launch(file, (GObject *) file_launch);
      g_signal_connect(G_OBJECT(file_launch), "start\0",
		       G_CALLBACK(ags_file_read_thread_start), gobject);
    }
  }

  /* read children */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(child->name,
		     "ags-thread-list\0",
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
	}else if(!xmlStrncmp("ags-main-loop\0",
		     child->name,
		     11)){
	ags_file_read_main_loop(file,
				child,
				gobject);
      }
    }

    child = child->next;
  }
}

void
ags_file_read_thread_start(AgsFileLaunch *file_launch, AgsThread *thread)
{
  thread->flags &= (~AGS_THREAD_RUNNING);
  ags_thread_start(thread);

  pthread_mutex_lock(&(thread->start_mutex));

  while((AGS_THREAD_INITIAL_RUN & (g_atomic_int_get(&(thread->flags)))) == 0){
    pthread_cond_wait(&(thread->start_cond),
		      &(thread->start_mutex));
    
  }

  pthread_mutex_unlock(&(thread->start_mutex));
}

xmlNode*
ags_file_write_thread(AgsFile *file, xmlNode *parent, AgsThread *thread)
{
  AgsFileLookup *file_lookup;
  AgsThread *current;
  xmlNode *node, *child;
  gchar *id;

  if(AGS_IS_RETURNABLE_THREAD(thread)){
    return;
  }

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-thread\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);
 
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", thread,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_TYPE_PROP,
	     G_OBJECT_TYPE_NAME(thread));

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", thread->flags));

  xmlAddChild(parent,
	      node);

  /* child elements */
  if(AGS_IS_MAIN_LOOP(thread)){
    ags_file_write_main_loop(file,
			      node,
			      thread);
  }

  current = thread->children;

  child = xmlNewNode(NULL,
		     "ags-thread-list\0");
  xmlAddChild(node,
	      child);

  while(current != NULL){
    ags_file_write_thread(file,
			  child,
			  current);
    current = current->next;
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
		     "ags-thread\0",
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
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
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
		    "ags-thread-list\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", list,
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
  //	       "ags-main\0", file->application_context,
  //	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
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
		    "ags-thread_pool\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", thread_pool,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", thread_pool->flags));
}

void
ags_file_read_thread_pool_start(AgsFileLaunch *file_launch, AgsThreadPool *thread_pool)
{
  ags_thread_pool_start(thread_pool);
}

void
ags_file_read_main_loop(AgsFile *file, xmlNode *node, AgsThread *main_loop)
{
  AgsFileLookup *file_lookup;
  xmlNode *child;

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", main_loop,
				   NULL));

  ags_main_loop_set_application_context(AGS_MAIN_LOOP(main_loop),
					file->application_context);
}

xmlNode*
ags_file_write_main_loop(AgsFile *file, xmlNode *parent, AgsThread *main_loop)
{
  AgsFileLookup *file_lookup;
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-main-loop\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", main_loop,
				   NULL));

  xmlAddChild(parent,
	      node);
}
