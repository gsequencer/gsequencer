/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/ags_xorg_application_context.h>

#include <ags/object/ags_connectable.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_gui.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/thread/ags_gui_thread.h>
#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_thread-posix.h>
#include <ags/thread/ags_thread_pool.h>

#include <ags/audio/ags_sound_provider.h>

void ags_xorg_application_context_class_init(AgsXorgApplicationContextClass *xorg_application_context);
void ags_xorg_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_xorg_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void ags_xorg_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider);
void ags_xorg_application_context_init(AgsXorgApplicationContext *xorg_application_context);
void ags_xorg_application_context_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void ags_xorg_application_context_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);
void ags_xorg_application_context_connect(AgsConnectable *connectable);
void ags_xorg_application_context_disconnect(AgsConnectable *connectable);
AgsThread* ags_xorg_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
AgsThreadPool* ags_xorg_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
GList* ags_xorg_application_context_get_soundcard(AgsSoundProvider *sound_provider);
void ags_xorg_application_context_finalize(GObject *gobject);

void ags_xorg_application_context_load_config(AgsApplicationContext *application_context);
void ags_xorg_application_context_register_types(AgsApplicationContext *application_context);

void ags_xorg_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context);
xmlNode* ags_xorg_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context);

static gpointer ags_xorg_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_xorg_application_context_parent_connectable_interface;

enum{
  PROP_0,
  PROP_WINDOW,
};

extern AgsXorgApplicationContext *ags_xorg_application_context;

GType
ags_xorg_application_context_get_type()
{
  static GType ags_type_xorg_application_context = 0;

  if(!ags_type_xorg_application_context){
    static const GTypeInfo ags_xorg_application_context_info = {
      sizeof (AgsXorgApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xorg_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXorgApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xorg_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_xorg_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrency_provider_interface_info = {
      (GInterfaceInitFunc) ags_xorg_application_context_concurrency_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_provider_interface_info = {
      (GInterfaceInitFunc) ags_xorg_application_context_sound_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_xorg_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
							       "AgsXorgApplicationContext\0",
							       &ags_xorg_application_context_info,
							       0);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_CONCURRENCY_PROVIDER,
				&ags_concurrency_provider_interface_info);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_SOUND_PROVIDER,
				&ags_sound_provider_interface_info);
  }

  return (ags_type_xorg_application_context);
}

void
ags_xorg_application_context_class_init(AgsXorgApplicationContextClass *xorg_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;
  GParamSpec *param_spec;

  ags_xorg_application_context_parent_class = g_type_class_peek_parent(xorg_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) xorg_application_context;

  gobject->set_property = ags_xorg_application_context_set_property;
  gobject->get_property = ags_xorg_application_context_get_property;

  gobject->finalize = ags_xorg_application_context_finalize;
  
  /**
   * AgsXorgApplicationContext:window:
   *
   * The assigned window.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("window\0",
				   "window of xorg application context\0",
				   "The window which this xorg application context assigned to\0",
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WINDOW,
				  param_spec);

  /* AgsXorgApplicationContextClass */
  application_context = (AgsApplicationContextClass *) xorg_application_context;
  
  application_context->load_config = ags_xorg_application_context_load_config;
  application_context->register_types = ags_xorg_application_context_register_types;
}

void
ags_xorg_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_xorg_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_xorg_application_context_connect;
  connectable->disconnect = ags_xorg_application_context_disconnect;
}

void
ags_xorg_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider)
{
  concurrency_provider->get_main_loop = ags_xorg_application_context_get_main_loop;
  concurrency_provider->get_thread_pool = ags_xorg_application_context_get_thread_pool;
}

void
ags_xorg_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider)
{
  sound_provider->get_soundcard = ags_xorg_application_context_get_soundcard;
}

void
ags_xorg_application_context_init(AgsXorgApplicationContext *xorg_application_context)
{
  xorg_application_context->thread_pool = NULL;
  xorg_application_context->server = NULL;
  xorg_application_context->soundcard = NULL;
  xorg_application_context->window = NULL;
}

void
ags_xorg_application_context_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_WINDOW:
    {
      AgsWindow *window;
      
      window = (AgsWindow *) g_value_get_object(value);

      if(window == xorg_application_context->window)
	return;

      if(xorg_application_context->window != NULL)
	g_object_unref(xorg_application_context->window);

      if(window != NULL)
	g_object_ref(G_OBJECT(window));

      xorg_application_context->window = window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_xorg_application_context_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_WINDOW:
    {
      g_value_set_object(value, xorg_application_context->window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_xorg_application_context_connect(AgsConnectable *connectable)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(xorg_application_context)->flags)) != 0){
    return;
  }

  ags_xorg_application_context_parent_connectable_interface->connect(connectable);

  g_message("connecting gui\0");

  ags_connectable_connect(AGS_CONNECTABLE(xorg_application_context->window));
}

void
ags_xorg_application_context_disconnect(AgsConnectable *connectable)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(xorg_application_context)->flags)) == 0){
    return;
  }

  ags_xorg_application_context_parent_connectable_interface->disconnect(connectable);
}

AgsThread*
ags_xorg_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_APPLICATION_CONTEXT(concurrency_provider)->main_loop);
}

AgsThreadPool*
ags_xorg_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(concurrency_provider)->thread_pool);
}

GList*
ags_xorg_application_context_get_soundcard(AgsSoundProvider *sound_provider)
{
  return(AGS_XORG_APPLICATION_CONTEXT(sound_provider)->soundcard);
}

void
ags_xorg_application_context_finalize(GObject *gobject)
{
  AgsXorgApplicationContext *xorg_application_context;

  G_OBJECT_CLASS(ags_xorg_application_context_parent_class)->finalize(gobject);

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);
}

void
ags_xorg_application_context_load_config(AgsApplicationContext *application_context)
{
  //TODO:JK: implement me
}

void
ags_xorg_application_context_register_types(AgsApplicationContext *application_context)
{
  ags_gui_thread_get_type();

  /* gui */
  //TODO:JK: move me
  ags_dial_get_type();

  /* register machine */
  ags_panel_get_type();
  ags_panel_input_pad_get_type();
  ags_panel_input_line_get_type();

  ags_mixer_get_type();
  ags_mixer_input_pad_get_type();
  ags_mixer_input_line_get_type();

  ags_drum_get_type();
  ags_drum_output_pad_get_type();
  ags_drum_output_line_get_type();
  ags_drum_input_pad_get_type();
  ags_drum_input_line_get_type();

  ags_matrix_get_type();

  ags_synth_get_type();
  ags_synth_input_pad_get_type();
  ags_synth_input_line_get_type();

  ags_ffplayer_get_type();
}

void
ags_xorg_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context)
{
  AgsXorgApplicationContext *gobject;
  GList *list;
  xmlNode *child;

  if(*application_context == NULL){
    gobject = g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
			   NULL);

    *application_context = (GObject *) gobject;
  }else{
    gobject = (AgsApplicationContext *) *application_context;
  }

  file->application_context = gobject;

  g_object_set(G_OBJECT(file),
	       "application-context\0", gobject,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
  
  /* properties */
  AGS_APPLICATION_CONTEXT(gobject)->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
					    NULL,
					    16);

  AGS_APPLICATION_CONTEXT(gobject)->version = xmlGetProp(node,
							 AGS_FILE_VERSION_PROP);

  AGS_APPLICATION_CONTEXT(gobject)->build_id = xmlGetProp(node,
							  AGS_FILE_BUILD_ID_PROP);

  //TODO:JK: check version compatibelity

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-window\0",
			   child->name,
			   11)){
	ags_file_read_window(file,
			     child,
			     &(gobject->window));
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_xorg_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context)
{
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-application-context\0");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", application_context,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_CONTEXT_PROP,
	     "xorg\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", ((~AGS_APPLICATION_CONTEXT_CONNECTED) & (AGS_APPLICATION_CONTEXT(application_context)->flags))));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_APPLICATION_CONTEXT(application_context)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_APPLICATION_CONTEXT(application_context)->build_id);

  /* add to parent */
  xmlAddChild(parent,
	      node);

  ags_file_write_window(file,
			node,
			AGS_XORG_APPLICATION_CONTEXT(application_context)->window);

  return(node);
}

AgsXorgApplicationContext*
ags_xorg_application_context_new(GObject *main_loop,
				 AgsConfig *config)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = (AgsXorgApplicationContext *) g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
									"main-loop\0", main_loop,
									"config\0", config,
									NULL);

  return(xorg_application_context);
}


