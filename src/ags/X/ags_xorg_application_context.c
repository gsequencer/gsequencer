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

#include <ags-lib/object/ags_connectable.h>

#include <ags/xorg/ags_xorg_loop.h>
#include <ags/xorg/ags_gui_xorg.h>
#include <ags/xorg/ags_autosave_xorg.h>
#include <ags/xorg/ags_single_xorg.h>

void ags_xorg_application_context_class_init(AgsXorgApplicationContextClass *xorg_application_context);
void ags_xorg_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
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
void ags_xorg_application_context_finalize(GObject *gobject);

static gpointer ags_xorg_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_xorg_application_context_parent_connectable_interface;

enum{
  PROP_0,
  PROP_WINDOW,
};

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

    ags_type_xorg_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
							       "AgsXorgApplicationContext\0",
							       &ags_xorg_application_context_info,
							       0);

    g_type_add_interface_static(ags_type_xorg_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_xorg_application_context);
}

void
ags_xorg_application_context_class_init(AgsXorgApplicationContextClass *xorg_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;

  ags_xorg_application_context_parent_class = g_type_class_peek_parent(ags_xorg_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) ags_xorg_application_context;

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
  application_context = (AgsApplicationContextClass *) xorg_application_context_class;
  
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
ags_xorg_application_context_init(AgsXorgApplicationContext *xorg_application_context)
{
  xorg_application_context->flags = 0;

  application_context->window = NULL;
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

  if((AGS_XORG_APPLICATION_CONTEXT_CONNECTED & (xorg_application_context->flags)) != 0){
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

  if((AGS_XORG_APPLICATION_CONTEXT_CONNECTED & (xorg_application_context->flags)) == 0){
    return;
  }

  ags_xorg_application_context_parent_connectable_interface->disconnect(connectable);
}

void
ags_xorg_application_context_finalize(GObject *gobject)
{
  AgsXorgApplicationContext *xorg_application_context;

  G_OBJECT_CLASS(ags_xorg_application_context_parent_class)->finalize(gobject);

  xorg_application_context = AGS_XORG_APPLICATION_CONTEXT(gobject);
}

AgsXorgApplicationContext*
ags_xorg_application_context_new(AgsMainLoop *main_loop,
				 AgsConfig *config)
{
  AgsXorgApplicationContext *xorg_application_context;

  xorg_application_context = (AgsXorgApplicationContext *) g_object_new(AGS_TYPE_XORG_APPLICATION_CONTEXT,
									"main-loop\0", main_loop,
									"config\0", config,
									NULL);

  return(xorg_application_context);
}


