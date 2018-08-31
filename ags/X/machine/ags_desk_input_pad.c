/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/X/machine/ags_desk_input_pad.h>
#include <ags/X/machine/ags_desk_input_pad_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_desk.h>

#include <math.h>

#include <ags/i18n.h>

void ags_desk_input_pad_class_init(AgsDeskInputPadClass *desk_input_pad);
void ags_desk_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_desk_input_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_desk_input_pad_init(AgsDeskInputPad *desk_input_pad);
static void ags_desk_input_pad_finalize(GObject *gobject);

void ags_desk_input_pad_connect(AgsConnectable *connectable);
void ags_desk_input_pad_disconnect(AgsConnectable *connectable);

gchar* ags_desk_input_pad_get_name(AgsPlugin *plugin);
void ags_desk_input_pad_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_desk_input_pad_get_xml_type(AgsPlugin *plugin);
void ags_desk_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_desk_input_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_desk_input_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);


/**
 * SECTION:ags_desk_input_pad
 * @short_description: desk sequencer input pad
 * @title: AgsDeskInputPad
 * @section_id:
 * @include: ags/X/machine/ags_desk_input_pad.h
 *
 * The #AgsDeskInputPad is a composite widget to act as desk sequencer input pad.
 */

static gpointer ags_desk_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_desk_input_pad_parent_connectable_interface;

GType
ags_desk_input_pad_get_type()
{
  static GType ags_type_desk_input_pad = 0;

  if(!ags_type_desk_input_pad){
    static const GTypeInfo ags_desk_input_pad_info = {
      sizeof(AgsDeskInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_desk_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDeskInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_desk_input_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_desk_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_desk_input_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_desk_input_pad = g_type_register_static(GTK_TYPE_VBOX,
						     "AgsDeskInputPad", &ags_desk_input_pad_info,
						     0);

    g_type_add_interface_static(ags_type_desk_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_desk_input_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_desk_input_pad);
}

void
ags_desk_input_pad_class_init(AgsDeskInputPadClass *desk_input_pad)
{
  GObjectClass *gobject;
  
  ags_desk_input_pad_parent_class = g_type_class_peek_parent(desk_input_pad);

  /*  */
  gobject = (GObjectClass *) desk_input_pad;

  gobject->finalize = ags_desk_input_pad_finalize;
}

void
ags_desk_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_desk_input_pad_connect;
  connectable->disconnect = ags_desk_input_pad_disconnect;
}

void
ags_desk_input_pad_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_desk_input_pad_get_name;
  plugin->set_name = ags_desk_input_pad_set_name;
  plugin->get_xml_type = ags_desk_input_pad_get_xml_type;
  plugin->set_xml_type = ags_desk_input_pad_set_xml_type;
  plugin->read = ags_desk_input_pad_read;
  plugin->write = ags_desk_input_pad_write;
}

void
ags_desk_input_pad_init(AgsDeskInputPad *desk_input_pad)
{
  GtkHBox *hbox;

  desk_input_pad->name = NULL;
  desk_input_pad->xml_type = "ags-desk-input-pad";

  /* position */
  desk_input_pad->position_time = (GtkLabel *) gtk_label_new("00:00.000");
  gtk_box_pack_start((GtkBox *) desk_input_pad,
		     (GtkWidget *) desk_input_pad->position_time,
		     FALSE, FALSE,
		     0);
  
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) desk_input_pad,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);
  
  desk_input_pad->position = (GtkScale *) gtk_hscale_new_with_range(0.0, 1.0, 0.001);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) desk_input_pad->position,
		     TRUE, TRUE,
		     0);

  /* filename */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) desk_input_pad,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* play */
  desk_input_pad->play = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
							  "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON),
							  NULL);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) desk_input_pad->play,
		     FALSE, FALSE,
		     0);

  desk_input_pad->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) desk_input_pad->filename,
		     FALSE, FALSE,
		     0);
  
  desk_input_pad->grab_filename = (GtkButton *) gtk_button_new_with_label(i18n("grab"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) desk_input_pad->grab_filename,
		     FALSE, FALSE,
		     0);

  /* controls */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) desk_input_pad,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* indicator */
  desk_input_pad->indicator = (AgsIndicator *) ags_vindicator_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) desk_input_pad->indicator,
		     FALSE, FALSE,
		     0);

  /* volume */
  desk_input_pad->volume = (GtkScale *) gtk_vscale_new_with_range(0.0, 2.0, 0.1);
  gtk_range_set_inverted(desk_input_pad->volume,
			 TRUE);
  gtk_range_set_value(desk_input_pad->volume,
		      1.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) desk_input_pad->volume,
		     FALSE, FALSE,
		     0);
}

static void
ags_desk_input_pad_finalize(GObject *gobject)
{
  AgsDeskInputPad *desk_input_pad;

  desk_input_pad = AGS_DESK_INPUT_PAD(gobject);

  G_OBJECT_CLASS(ags_desk_input_pad_parent_class)->finalize(gobject);
}

void
ags_desk_input_pad_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_desk_input_pad_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_desk_input_pad_get_name(AgsPlugin *plugin)
{
  return(AGS_DESK_INPUT_PAD(plugin)->name);
}

void
ags_desk_input_pad_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_DESK_INPUT_PAD(plugin)->name = name;
}

gchar*
ags_desk_input_pad_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_DESK_INPUT_PAD(plugin)->xml_type);
}

void
ags_desk_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_DESK_INPUT_PAD(plugin)->xml_type = xml_type;
}

void
ags_desk_input_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsDeskInputPad *gobject;

  gobject = AGS_DESK_INPUT_PAD(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));
}

xmlNode*
ags_desk_input_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsDeskInputPad *desk_input_pad;
  xmlNode *node;
  gchar *id;

  desk_input_pad = AGS_DESK_INPUT_PAD(plugin);
  node = NULL;

  return(node);
}

/**
 * ags_desk_input_pad_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsDeskInputPad
 *
 * Returns: a new #AgsDeskInputPad
 *
 * Since: 1.5.0
 */
AgsDeskInputPad*
ags_desk_input_pad_new(AgsChannel *channel)
{
  AgsDeskInputPad *desk_input_pad;

  desk_input_pad = (AgsDeskInputPad *) g_object_new(AGS_TYPE_DESK_INPUT_PAD,
						    "channel", channel,
						    NULL);

  return(desk_input_pad);
}
