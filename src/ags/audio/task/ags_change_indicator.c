/* This file is part of GSequencer.
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

#include <ags/audio/task/ags_change_indicator.h>

#include <ags-lib/object/ags_connectable.h>

void ags_change_indicator_class_init(AgsChangeIndicatorClass *change_indicator);
void ags_change_indicator_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_change_indicator_init(AgsChangeIndicator *change_indicator);
void ags_change_indicator_connect(AgsConnectable *connectable);
void ags_change_indicator_disconnect(AgsConnectable *connectable);
void ags_change_indicator_finalize(GObject *gobject);
void ags_change_indicator_launch(AgsTask *task);

/**
 * SECTION:ags_change_indicator
 * @short_description: change indicator object
 * @title: AgsChangeIndicator
 * @section_id:
 * @include: ags/audio/task/ags_change_indicator.h
 *
 * The #AgsChangeIndicator task changes #AgsIndicator and the GUI is updated.
 */

static gpointer ags_change_indicator_parent_class = NULL;
static AgsConnectableInterface *ags_change_indicator_parent_connectable_interface;

GType
ags_change_indicator_get_type()
{
  static GType ags_type_change_indicator = 0;

  if(!ags_type_change_indicator){
    static const GTypeInfo ags_change_indicator_info = {
      sizeof (AgsChangeIndicatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_change_indicator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChangeIndicator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_change_indicator_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_change_indicator_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_change_indicator = g_type_register_static(AGS_TYPE_TASK,
						 "AgsChangeIndicator\0",
						 &ags_change_indicator_info,
						 0);

    g_type_add_interface_static(ags_type_change_indicator,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_change_indicator);
}

void
ags_change_indicator_class_init(AgsChangeIndicatorClass *change_indicator)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_change_indicator_parent_class = g_type_class_peek_parent(change_indicator);

  /* GObject */
  gobject = (GObjectClass *) change_indicator;

  gobject->finalize = ags_change_indicator_finalize;

  /* AgsTask */
  task = (AgsTaskClass *) change_indicator;

  task->launch = ags_change_indicator_launch;
}

void
ags_change_indicator_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_change_indicator_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_change_indicator_connect;
  connectable->disconnect = ags_change_indicator_disconnect;
}

void
ags_change_indicator_init(AgsChangeIndicator *change_indicator)
{  
  change_indicator->indicator = NULL;
  change_indicator->value = 0.0;
}

void
ags_change_indicator_connect(AgsConnectable *connectable)
{
  ags_change_indicator_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_change_indicator_disconnect(AgsConnectable *connectable)
{
  ags_change_indicator_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_change_indicator_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_change_indicator_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_change_indicator_launch(AgsTask *task)
{
  AgsChangeIndicator *change_indicator;
  GtkAdjustment *adjustment;

  change_indicator = AGS_CHANGE_INDICATOR(task);

  g_object_get(change_indicator->indicator,
	       "adjustment\0", &adjustment,
	       NULL);

  gtk_adjustment_set_value(adjustment,
			   change_indicator->value);

  gtk_widget_queue_draw((GtkWidget *) change_indicator->indicator);
}

/**
 * ags_change_indicator_new:
 * @indicator: the #AgsIndicator to change
 * @value: the value to apply
 *
 * Creates an #AgsChangeIndicator.
 *
 * Returns: an new #AgsChangeIndicator.
 *
 * Since: 0.4
 */
AgsChangeIndicator*
ags_change_indicator_new(AgsIndicator *indicator,
			 gdouble value)
{
  AgsChangeIndicator *change_indicator;

  change_indicator = (AgsChangeIndicator *) g_object_new(AGS_TYPE_CHANGE_INDICATOR,
							 NULL);
  
  change_indicator->indicator = indicator;
  change_indicator->value = value;

  return(change_indicator);
}
