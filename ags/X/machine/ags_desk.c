/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_desk.h>
#include <ags/X/machine/ags_desk_callbacks.h>

#include <ags/i18n.h>

void ags_desk_class_init(AgsDeskClass *desk);
void ags_desk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_desk_init(AgsDesk *desk);
void ags_desk_finalize(GObject *gobject);

void ags_desk_map_recall(AgsMachine *machine);

void ags_desk_connect(AgsConnectable *connectable);
void ags_desk_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_desk
 * @short_description: desk sequencer
 * @title: AgsDesk
 * @section_id:
 * @include: ags/X/machine/ags_desk.h
 *
 * The #AgsDesk is a composite widget to act as desk sequencer.
 */

static gpointer ags_desk_parent_class = NULL;

static AgsConnectableInterface *ags_desk_parent_connectable_interface;

GType
ags_desk_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_desk = 0;

    static const GTypeInfo ags_desk_info = {
      sizeof(AgsDeskClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_desk_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDesk),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_desk_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_desk_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_desk = g_type_register_static(AGS_TYPE_MACHINE,
					   "AgsDesk", &ags_desk_info,
					   0);
    
    g_type_add_interface_static(ags_type_desk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_desk);
  }

  return g_define_type_id__volatile;
}

void
ags_desk_class_init(AgsDeskClass *desk)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_desk_parent_class = g_type_class_peek_parent(desk);

  /* GObjectClass */
  gobject = (GObjectClass *) desk;

  gobject->finalize = ags_desk_finalize;

  /*  */
  machine = (AgsMachineClass *) desk;

  machine->map_recall = ags_desk_map_recall;
}

void
ags_desk_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_desk_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_desk_connect;
  connectable->disconnect = ags_desk_disconnect;
}

void
ags_desk_init(AgsDesk *desk)
{
  GtkHBox *hbox;
  GtkAlignment *alignment;
  GtkHBox *balance_hbox;
  
  desk->name = NULL;
  desk->xml_type = "ags-desk";

  /* create widgets */
  desk->vbox = (GtkVBox *) gtk_vbox_new(FALSE,
					0);
  gtk_container_add((GtkContainer*) gtk_bin_get_child((GtkBin *) desk),
		    (GtkWidget *) desk->vbox);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) desk->vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* left pad */
  desk->left_pad = ags_desk_input_pad_new(NULL);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) desk->left_pad,
		     FALSE, FALSE,
		     0);

  /* console */
  alignment = gtk_alignment_new(0.0,
				0.0,
				0.0,
				0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) alignment,
		     FALSE, FALSE,
		     0);
  
  desk->console = (GtkVBox *) gtk_vbox_new(FALSE,
					   0);
  gtk_container_add((GtkContainer *) alignment,
		    (GtkWidget *) desk->console);
  
  balance_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
					  0);
  gtk_box_pack_start((GtkBox *) desk->console,
		     (GtkWidget *) balance_hbox,
		     FALSE, FALSE,
		     0);
  
  desk->move_left = (GtkButton *) gtk_button_new_with_label(i18n("left"));
  gtk_box_pack_start((GtkBox *) balance_hbox,
		     (GtkWidget *) desk->move_left,
		     FALSE, FALSE,
		     0);

  desk->balance = (GtkScale *) gtk_hscale_new_with_range(-1.0, 1.0, 0.1);
  gtk_widget_set_size_request((GtkWidget *) desk->balance,
			      200, -1);
  gtk_box_pack_start((GtkBox *) balance_hbox,
		     (GtkWidget *) desk->balance,
		     FALSE, FALSE,
		     0);

  desk->move_right = (GtkButton *) gtk_button_new_with_label(i18n("right"));
  gtk_box_pack_start((GtkBox *) balance_hbox,
		     (GtkWidget *) desk->move_right,
		     FALSE, FALSE,
		     0);

  /* left pad */
  desk->right_pad = ags_desk_input_pad_new(NULL);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) desk->right_pad,
		     FALSE, FALSE,
		     0);

  /* file chooser */
  desk->file_chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_widget_set_size_request((GtkWidget *) desk->file_chooser,
			      -1, 400);
  gtk_box_pack_start((GtkBox *) desk->vbox,
		     (GtkWidget *) desk->file_chooser,
		     FALSE, FALSE,
		     0);
}

void
ags_desk_finalize(GObject *gobject)
{  
  G_OBJECT_CLASS(ags_desk_parent_class)->finalize(gobject);
}

void
ags_desk_connect(AgsConnectable *connectable)
{
  AgsDesk *desk;

  int i;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  desk = AGS_DESK(connectable);

  /* call parent */
  ags_desk_parent_connectable_interface->connect(connectable);
}

void
ags_desk_disconnect(AgsConnectable *connectable)
{
  AgsDesk *desk;

  int i;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  desk = AGS_DESK(connectable);

  /* call parent */
  ags_desk_parent_connectable_interface->disconnect(connectable);
}

void
ags_desk_map_recall(AgsMachine *machine)
{

  /* call parent */
  AGS_MACHINE_CLASS(ags_desk_parent_class)->map_recall(machine);
}

/**
 * ags_desk_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsDesk
 *
 * Returns: a new #AgsDesk
 *
 * Since: 2.0.0
 */
AgsDesk*
ags_desk_new(GObject *soundcard)
{
  AgsDesk *desk;

  desk = (AgsDesk *) g_object_new(AGS_TYPE_DESK,
				  NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(desk)->audio),
	       "soundcard", soundcard,
	       NULL);

  return(desk);
}
