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

#include <ags/X/editor/ags_sf2_chooser.h>
#include <ags/X/editor/ags_sf2_chooser_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

void ags_sf2_chooser_class_init(AgsSF2ChooserClass *sf2_chooser);
void ags_sf2_chooser_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sf2_chooser_init(AgsSF2Chooser *sf2_chooser);
void ags_sf2_chooser_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_sf2_chooser_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_sf2_chooser_finalize(GObject *gobject);
void ags_sf2_chooser_connect(AgsConnectable *connectable);
void ags_sf2_chooser_disconnect(AgsConnectable *connectable);
void ags_sf2_chooser_show(GtkWidget *widget);

void ags_sf2_chooser_update(AgsSF2Chooser *sf2_chooser);

enum{
  PROP_0,
  PROP_IPATCH,
};

static gpointer ags_sf2_chooser_parent_class = NULL;

GType
ags_sf2_chooser_get_type(void)
{
  static GType ags_type_sf2_chooser = 0;

  if(!ags_type_sf2_chooser){
    static const GTypeInfo ags_sf2_chooser_info = {
      sizeof (AgsSF2ChooserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sf2_chooser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSF2Chooser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sf2_chooser_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sf2_chooser_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sf2_chooser = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsSF2Chooser\0", &ags_sf2_chooser_info,
						  0);
    
    g_type_add_interface_static(ags_type_sf2_chooser,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_sf2_chooser);
}

void
ags_sf2_chooser_class_init(AgsSF2ChooserClass *sf2_chooser)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_sf2_chooser_parent_class = g_type_class_peek_parent(sf2_chooser);

  /* GObjectClass */
  gobject = (GObjectClass *) sf2_chooser;

  gobject->set_property = ags_sf2_chooser_set_property;
  gobject->get_property = ags_sf2_chooser_get_property;

  gobject->finalize = ags_sf2_chooser_finalize;

  /* properties */
  param_spec = g_param_spec_object("ipatch\0",
				   "current ipatch\0",
				   "The current AgsIpatch\0",
				   AGS_TYPE_IPATCH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IPATCH,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) sf2_chooser;

  widget->show = ags_sf2_chooser_show;
}

void
ags_sf2_chooser_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_sf2_chooser_connect;
  connectable->disconnect = ags_sf2_chooser_disconnect;
}

void
ags_sf2_chooser_init(AgsSF2Chooser *sf2_chooser)
{
  GtkTable *table;
  GtkLabel *label;

  sf2_chooser->ipatch = NULL;

  table = (GtkTable *) gtk_table_new(2, 3, FALSE);
  gtk_box_pack_start(GTK_BOX(sf2_chooser), GTK_WIDGET(table), FALSE, FALSE, 0);

  /* first row - preset */
  label = (GtkLabel *) gtk_label_new("preset: \0");
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sf2_chooser->preset = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(sf2_chooser->preset),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* second row - instrument */
  label = (GtkLabel *) gtk_label_new("instrument: \0");
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sf2_chooser->instrument = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(sf2_chooser->instrument),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* third row - sample */
  label = (GtkLabel *) gtk_label_new("sample: \0");
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  sf2_chooser->sample = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_table_attach(table,
		   GTK_WIDGET(sf2_chooser->sample),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_sf2_chooser_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsSF2Chooser *sf2_chooser;

  sf2_chooser = AGS_SF2_CHOOSER(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    {
      AgsIpatch *ipatch;

      ipatch = (AgsIpatch *) g_value_get_object(value);

      if(sf2_chooser->ipatch == ipatch)
	return;

      if(sf2_chooser->ipatch != NULL){
	g_object_unref(sf2_chooser->ipatch);
      }
	  
      if(ipatch != NULL){
	g_object_ref(ipatch);
      }	

      sf2_chooser->ipatch = ipatch;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sf2_chooser_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsSF2Chooser *sf2_chooser;

  sf2_chooser = AGS_SF2_CHOOSER(gobject);

  switch(prop_id){
  case PROP_IPATCH:
    g_value_set_object(value, sf2_chooser->ipatch);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sf2_chooser_finalize(GObject *gobject)
{
  AgsSF2Chooser *sf2_chooser;

  sf2_chooser = (AgsSF2Chooser *) gobject;

  //TODO:JK:
  /* implement me */

  G_OBJECT_CLASS(ags_sf2_chooser_parent_class)->finalize(gobject);
}

void
ags_sf2_chooser_connect(AgsConnectable *connectable)
{
  AgsSF2Chooser *sf2_chooser;

  /* AgsSF2Chooser */
  sf2_chooser = AGS_SF2_CHOOSER(connectable);

  //TODO:JK:
  /* implement me */
}

void
ags_sf2_chooser_disconnect(AgsConnectable *connectable)
{
  //TODO:JK:
  /* implement me */
}

void
ags_sf2_chooser_show(GtkWidget *widget)
{
  AgsSF2Chooser *sf2_chooser = (AgsSF2Chooser *) widget;

  GTK_WIDGET_CLASS(ags_sf2_chooser_parent_class)->show(widget);

  //TODO:JK:
  /* perhaps empty */
}

void
ags_sf2_chooser_open(AgsSF2Chooser *sf2_chooser, gchar *filename)
{
  AgsIpatch *ipatch;

  ipatch = g_object_new(AGS_TYPE_IPATCH,
			"mode\0", "r\0",
			"filename\0", filename,
			NULL);

  g_object_set(G_OBJECT(sf2_chooser),
	       "ipatch\0", ipatch,
	       NULL);
 
  //TODO:JK: fill sf2_chooser->preset
}

void
ags_sf2_chooser_update(AgsSF2Chooser *sf2_chooser)
{
  //TODO:JK:
  /* implement me */
}

AgsSF2Chooser*
ags_sf2_chooser_new()
{
  AgsSF2Chooser *sf2_chooser;

  sf2_chooser = (AgsSF2Chooser *) g_object_new(AGS_TYPE_SF2_CHOOSER,
					       NULL);
  
  return(sf2_chooser);
}
