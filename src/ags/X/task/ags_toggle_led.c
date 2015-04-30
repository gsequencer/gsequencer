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

#include <ags/X/task/ags_toggle_led.h>

#include <ags/object/ags_connectable.h>

#include <ags/widget/ags_led.h>

void ags_toggle_led_class_init(AgsToggleLedClass *toggle_led);
void ags_toggle_led_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_toggle_led_init(AgsToggleLed *toggle_led);
void ags_toggle_led_connect(AgsConnectable *connectable);
void ags_toggle_led_disconnect(AgsConnectable *connectable);
void ags_toggle_led_finalize(GObject *gobject);
void ags_toggle_led_launch(AgsTask *task);

/**
 * SECTION:ags_toggle_led
 * @short_description: toggle led object
 * @title: AgsToggleLed
 * @section_id:
 * @include: ags/audio/task/ags_toggle_led.h
 *
 * The #AgsToggleLed task toggles #AgsLed and the GUI is updated.
 */

static gpointer ags_toggle_led_parent_class = NULL;
static AgsConnectableInterface *ags_toggle_led_parent_connectable_interface;

GType
ags_toggle_led_get_type()
{
  static GType ags_type_toggle_led = 0;

  if(!ags_type_toggle_led){
    static const GTypeInfo ags_toggle_led_info = {
      sizeof (AgsToggleLedClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_toggle_led_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsToggleLed),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_toggle_led_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_toggle_led_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_toggle_led = g_type_register_static(AGS_TYPE_TASK,
						 "AgsToggleLed\0",
						 &ags_toggle_led_info,
						 0);

    g_type_add_interface_static(ags_type_toggle_led,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_toggle_led);
}

void
ags_toggle_led_class_init(AgsToggleLedClass *toggle_led)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_toggle_led_parent_class = g_type_class_peek_parent(toggle_led);

  /* GObject */
  gobject = (GObjectClass *) toggle_led;

  gobject->finalize = ags_toggle_led_finalize;

  /* AgsTask */
  task = (AgsTaskClass *) toggle_led;

  task->launch = ags_toggle_led_launch;
}

void
ags_toggle_led_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_toggle_led_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_toggle_led_connect;
  connectable->disconnect = ags_toggle_led_disconnect;
}

void
ags_toggle_led_init(AgsToggleLed *toggle_led)
{
  toggle_led->led = NULL;
  toggle_led->set_active = 0; 
  toggle_led->unset_active = 0;
}

void
ags_toggle_led_connect(AgsConnectable *connectable)
{
  ags_toggle_led_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_toggle_led_disconnect(AgsConnectable *connectable)
{
  ags_toggle_led_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_toggle_led_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_toggle_led_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_toggle_led_launch(AgsTask *task)
{
  AgsToggleLed *toggle_led;
  GList *list, *active;
  guint i;

  toggle_led = AGS_TOGGLE_LED(task);
  list = toggle_led->led;

  for(i = 0; list != NULL; i++){
    if(i == toggle_led->set_active){
      active = list;
      list = list->next;
      
      continue;
    }

    ags_led_unset_active(AGS_LED(list->data));

    list = list->next;
  }

  ags_led_set_active(AGS_LED(active->data));
}

/**
 * ags_toggle_led_new:
 * @led: the #AgsLed to toggle
 * @set_active: the nth led to activate
 * @unset_active: the nth led to deactivate
 *
 * Creates an #AgsToggleLed.
 *
 * Returns: an new #AgsToggleLed.
 *
 * Since: 0.4
 */
AgsToggleLed*
ags_toggle_led_new(GList *led,
		   guint set_active,
		   guint unset_active)
{
  AgsToggleLed *toggle_led;

  toggle_led = (AgsToggleLed *) g_object_new(AGS_TYPE_TOGGLE_LED,
					     NULL);

  toggle_led->led = led;
  toggle_led->set_active = set_active;
  toggle_led->unset_active = unset_active;

  return(toggle_led);
}
