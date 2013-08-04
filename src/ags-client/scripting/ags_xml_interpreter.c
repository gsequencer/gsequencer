/* AGS Client - Advanced GTK Sequencer Client
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

#include <ags-client/scripting/ags_xml_interpreter.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags-client/scripting/ags_script.h>

#include <ags-client/scripting/mapping/ags_script_xpath.h>
#include <ags-client/scripting/mapping/ags_script_xmlrpc.h>
#include <ags-client/scripting/mapping/ags_script_scanf.h>
#include <ags-client/scripting/mapping/ags_script_pid.h>
#include <ags-client/scripting/mapping/ags_script_sscanf.h>
#include <ags-client/scripting/mapping/ags_script_sutex.h>
#include <ags-client/scripting/mapping/ags_script_break.h>
#include <ags-client/scripting/mapping/ags_script_controller.h>
#include <ags-client/scripting/mapping/ags_script_printf.h>
#include <ags-client/scripting/mapping/ags_script_semaphore.h>
#include <ags-client/scripting/mapping/ags_script_pop.h>
#include <ags-client/scripting/mapping/ags_script_function.h>
#include <ags-client/scripting/mapping/ags_script_do_while.h>
#include <ags-client/scripting/mapping/ags_script_host.h>
#include <ags-client/scripting/mapping/ags_script_switch.h>
#include <ags-client/scripting/mapping/ags_script_nanosleep.h>
#include <ags-client/scripting/mapping/ags_script_if.h>
#include <ags-client/scripting/mapping/ags_script_stack.h>
#include <ags-client/scripting/mapping/ags_script_for.h>
#include <ags-client/scripting/mapping/ags_script_memcpy.h>
#include <ags-client/scripting/mapping/ags_script_while.h>
#include <ags-client/scripting/mapping/ags_script_tostring.h>
#include <ags-client/scripting/mapping/ags_script_object.h>
#include <ags-client/scripting/mapping/ags_script_array.h>
#include <ags-client/scripting/mapping/ags_script_valueof.h>
#include <ags-client/scripting/mapping/ags_script_set.h>
#include <ags-client/scripting/mapping/ags_script_fprintf.h>
#include <ags-client/scripting/mapping/ags_script_var.h>
#include <ags-client/scripting/mapping/ags_script_push.h>
#include <ags-client/scripting/mapping/ags_script_fscanf.h>

void ags_xml_interpreter_class_init(AgsXmlInterpreterClass *xml_interpreter);
void ags_xml_interpreter_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_xml_interpreter_init(AgsXmlInterpreter *xml_interpreter);
void ags_xml_interpreter_connect(AgsConnectable *connectable);
void ags_xml_interpreter_disconnect(AgsConnectable *connectable);
void ags_xml_interpreter_finalize(GObject *gobject);

void ags_xml_interpreter_real_start(AgsXmlInterpreter *xml_interpreter);
void* ags_xml_interpreter_thread(void *ptr);

void ags_xml_interpreter_load_script(AgsXmlInterpreter *xml_interpreter,
				     AgsScript *script);
void ags_xml_interpreter_unload_script(AgsXmlInterpreter *xml_interpreter,
				       AgsScript *script);

enum{
  START,
  LAST_SIGNAL,
};

static gpointer ags_xml_interpreter_parent_class = NULL;
static guint xml_interpreter_signals[LAST_SIGNAL];

GType
ags_xml_interpreter_get_type()
{
  static GType ags_type_xml_interpreter = 0;

  if(!ags_type_xml_interpreter){
    static const GTypeInfo ags_xml_interpreter_info = {
      sizeof (AgsXmlInterpreterClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xml_interpreter_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXmlInterpreter),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xml_interpreter_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_xml_interpreter_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_xml_interpreter = g_type_register_static(G_TYPE_OBJECT,
						      "AgsXmlInterpreter\0",
						      &ags_xml_interpreter_info,
						      0);
    
    g_type_add_interface_static(ags_type_xml_interpreter,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_xml_interpreter);
}

void
ags_xml_interpreter_class_init(AgsXmlInterpreterClass *xml_interpreter)
{
  GObjectClass *gobject;

  ags_xml_interpreter_parent_class = g_type_class_peek_parent(xml_interpreter);

  /* GObjectClass */
  gobject = (GObjectClass *) xml_interpreter;

  gobject->finalize = ags_xml_interpreter_finalize;

  /* signals */
  xml_interpreter_signals[START] =
    g_signal_new("start\0",
		 G_TYPE_FROM_CLASS(xml_interpreter),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsXmlInterpreterClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_xml_interpreter_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_xml_interpreter_connect;
  connectable->disconnect = ags_xml_interpreter_disconnect;
}

GQuark
ags_xml_interpreter_error_quark()
{
  return(g_quark_from_static_string("ags-xml-interpreter-error-quark\0"));
}

void
ags_xml_interpreter_init(AgsXmlInterpreter *xml_interpreter)
{
  xml_interpreter->flags = 0;

  pthread_mutex_init(&(xml_interpreter->mutex), NULL);
  pthread_cond_init(&(xml_interpreter->cond), NULL);

  xml_interpreter->default_stack = ags_script_stack_new();
  xml_interpreter->stack_size = AGS_XML_INTERPRETER_DEFAULT_STACK_SIZE;

  xml_interpreter->script = NULL;
  pthread_mutex_init(&(xml_interpreter->script_mutex), NULL);
}

void
ags_xml_interpreter_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_xml_interpreter_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_xml_interpreter_finalize(GObject *gobject)
{
  AgsXmlInterpreter *xml_interpreter;

  xml_interpreter = AGS_XML_INTERPRETER(gobject);

  G_OBJECT_CLASS(ags_xml_interpreter_parent_class)->finalize(gobject);
}

void
ags_xml_interpreter_real_start(AgsXmlInterpreter *xml_interpreter)
{
  //TODO:JK: implement me
}

void
ags_xml_interpreter_start(AgsXmlInterpreter *xml_interpreter)
{
  g_return_if_fail(AGS_IS_XML_INTERPRETER(xml_interpreter));

  g_object_ref(G_OBJECT(xml_interpreter));
  g_signal_emit(G_OBJECT(xml_interpreter),
		xml_interpreter_signals[START], 0);
  g_object_unref(G_OBJECT(xml_interpreter));
}

void*
ags_xml_interpreter_thread(void *ptr)
{
  AgsXmlInterpreter *xml_interpreter;
  AgsScript *script;
  AgsScriptObject *current;
  GList *current;

  static struct timespec poll_interval;
  static gboolean initialized = FALSE;

  auto AgsScriptObject* ags_xml_interpreter_thread_recursive_launch(AgsScriptObject *script_object);

  AgsScriptObject* ags_xml_interpreter_thread_recursive_launch(AgsScriptObject *script_object){
    AgsScriptObject *next;

    next = script_object->retval;

    if(strtoul(xmlGetProp(next, "z_index\0"), NULL, 10) > strtoul(xmlGetProp(script_object, "z_index\0"), NULL, 10)){
      next = ags_xml_interpreter_thread_recursive_launch(next);
    }
    
    ags_script_object_launch(script_object);
    ags_xml_interpreter_run_snipped(xml_interpreter,
				    AGS_SCRIPT_OBJECT(xml_interpreter->default_stack)->node);

    return(next);
  }

  if(!initialized){
    initialized = TRUE;

    poll_interval.tv_sec = 0;
    poll_interval.tv_nsec = 500000;
  }

  xml_interpreter = AGS_XML_INTERPRETER(ptr);

  while((AGS_XML_INTERPRETER_RUNNING & (xml_interpreter->flags)) != 0){
    pthread_mutex_lock(&(xml_interpreter->script_mutex));

    current = g_list_reverse(xml_interpreter->script);
    xml_interpreter->script = NULL;

    pthread_mutex_unlock(&(xml_interpreter->script_mutex));

    while(current != NULL){
      script = AGS_SCRIPT(current->data);

      /* run script */
      ags_xml_interpreter_load_script(xml_interpreter,
				      script);

      current = AGS_SCRIPT_OBJECT(script->default_stack);

      while(current != NULL){
	current = ags_xml_interpreter_thread_recursive_launch(current);
      }

      ags_xml_interpreter_unload_script(xml_interpreter,
					script);

      /* iterate */
      current = current->next;
    }

    nanosleep(&poll_interval, NULL);
  }

  pthread_exit(NULL);
}

void
ags_xml_interpreter_load_script(AgsXmlInterpreter *xml_interpreter,
				AgsScript *script)
{
  AgsXmlScriptFactory *xml_script_factory;
  AgsScriptStack *default_stack;
  AgsScriptObject *current;
  AgsScriptObject *retval;
  xmlNode *root_node;
  xmlNode *current_node;

  auto AgsScriptObject* ags_xml_interpreter_load_script_recursive(xmlNode *node, guint z_index);

  AgsScriptObject* ags_xml_interpreter_load_script_recursive(xmlNode *node, guint z_index){
    AgsScriptObject *script_object;
    GType type;
    AgsScriptObject *current;
    AgsScriptObject *retval;
    xmlNode *current_node;
    xmlNode *mapped_node;
    GError *error;

    type = ags_xml_interpreter_type_from_name((gchar *) node->name);
    script_object = (AgsScriptObject *) g_object_new(type,
						     NULL);

    error = NULL;
    script_object->node =
      mapped_node = ags_xml_script_factory_map(xml_script_factory,
					       (gchar *) node->name,
					       &error);
    xmlSetProp(mapped_node, "z_index\0", g_strdup_printf("%d\0", z_index));
    ags_script_object_mapped_xml(script_object);


    if(error != NULL){
      g_warning(error->message);
    }
 
    current_node = node->children;

    while(current_node != NULL){
      if(current_node->type == XML_ELEMENT_NODE){
	retval = ags_xml_interpreter_load_script_recursive(current_node, z_index + 1);

	current->retval = retval;

	current = retval;
      }

      current_node = current_node->next;
    }

    return(script_object);
  }

  xml_script_factory = script->xml_script_factory;

  default_stack = xml_interpreter->default_stack;

  root_node = xmlDocGetRootElement(script->script);

  current_node = root_node->children;

  while(current_node != NULL){
    if(current_node->type == XML_ELEMENT_NODE){
      retval = ags_xml_interpreter_load_script_recursive(current_node, 0);

      current->retval = retval;

      current = retval;
    }

    current_node = current_node->next;
  }
}

void
ags_xml_interpreter_unload_script(AgsXmlInterpreter *xml_interpreter,
				  AgsScript *script)
{
  AgsScriptStack *default_stack;
  xmlNode *node;

  default_stack = xml_interpreter->default_stack;
  node = AGS_SCRIPT_OBJECT(default_stack)->node;

  g_object_unref(G_OBJECT(AGS_SCRIPT_OBJECT(default_stack)->retval));
  AGS_SCRIPT_OBJECT(default_stack)->retval = NULL;

  node->children = NULL;
  xmlSetProp(node, "value\0", NULL);
}

void
ags_xml_interpreter_run_snipped(AgsXmlInterpreter *xml_interpreter,
				xmlNode *snipped)
{
  //TODO:JK: implement me
}

GType
ags_xml_interpreter_type_from_name(gchar *name)
{
  if(!strncmp("ags-semaphore\0", name, 14)){
    return(AGS_TYPE_SCRIPT_SEMAPHORE);
  }else if(!strncmp("ags-var\0", name, 8)){
    return(AGS_TYPE_SCRIPT_VAR);
  }else if(!strncmp("ags-array\0", name, 10)){
    return(AGS_TYPE_SCRIPT_ARRAY);
  }else if(!strncmp("ags-stack\0", name, 10)){
    return(AGS_TYPE_SCRIPT_STACK);
  }else if(!strncmp("ags-controller\0", name, 15)){
    return(AGS_TYPE_SCRIPT_CONTROLLER);
  }else if(!strncmp("ags-set\0", name, 8)){
    return(AGS_TYPE_SCRIPT_SET);
  }else if(!strncmp("ags-push\0", name, 9)){
    return(AGS_TYPE_SCRIPT_PUSH);
  }else if(!strncmp("ags-pop\0", name, 8)){
    return(AGS_TYPE_SCRIPT_POP);
  }else if(!strncmp("ags-if\0", name, 7)){
    return(AGS_TYPE_SCRIPT_IF);
  }else if(!strncmp("ags-break\0", name, 9)){
    return(AGS_TYPE_SCRIPT_BREAK);
  }else if(!strncmp("ags-while\0", name, 10)){
    return(AGS_TYPE_SCRIPT_WHILE);
  }else if(!strncmp("ags-do_while\0", name, 13)){
    return(AGS_TYPE_SCRIPT_DO_WHILE);
  }else if(!strncmp("ags-for\0", name, 8)){
    return(AGS_TYPE_SCRIPT_FOR);
  }else if(!strncmp("ags-printf\0", name, 11)){
    return(AGS_TYPE_SCRIPT_PRINTF);
  }else if(!strncmp("ags-fprintf\0", name, 12)){
    return(AGS_TYPE_SCRIPT_FPRINTF);
  }else if(!strncmp("ags-scanf\0", name, 10)){
    return(AGS_TYPE_SCRIPT_SCANF);
  }else if(!strncmp("ags-fscanf\0", name, 11)){
    return(AGS_TYPE_SCRIPT_FSCANF);
  }else if(!strncmp("ags-tostring\0", name, 13)){
    return(AGS_TYPE_SCRIPT_TOSTRING);
  }else if(!strncmp("ags-valueof\0", name, 12)){
    return(AGS_TYPE_SCRIPT_VALUEOF);
  }else if(!strncmp("ags-memcpy\0", name, 11)){
    return(AGS_TYPE_SCRIPT_MEMCPY);
  }else if(!strncmp("ags-nanosleep\0", name, 14)){
    return(AGS_TYPE_SCRIPT_NANOSLEEP);
  }else if(!strncmp("ags-xmlrpc\0", name, 11)){
    return(AGS_TYPE_SCRIPT_XMLRPC);
  }

  return(G_TYPE_NONE);
}

AgsXmlInterpreter*
ags_xml_interpreter_new()
{
  AgsXmlInterpreter *xml_interpreter;

  xml_interpreter = (AgsXmlInterpreter *) g_object_new(AGS_TYPE_XML_INTERPRETER,
						       NULL);
  
  return(xml_interpreter);
}
