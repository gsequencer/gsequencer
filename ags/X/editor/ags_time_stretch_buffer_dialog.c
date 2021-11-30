/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/editor/ags_time_stretch_buffer_dialog.h>
#include <ags/X/editor/ags_time_stretch_buffer_dialog_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_wave_window.h>
#include <ags/X/ags_wave_editor.h>
#include <ags/X/ags_machine.h>

#include <ags/i18n.h>

void ags_time_stretch_buffer_dialog_class_init(AgsTimeStretchBufferDialogClass *time_stretch_buffer_dialog);
void ags_time_stretch_buffer_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_time_stretch_buffer_dialog_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_time_stretch_buffer_dialog_init(AgsTimeStretchBufferDialog *time_stretch_buffer_dialog);
void ags_time_stretch_buffer_dialog_finalize(GObject *gobject);

void ags_time_stretch_buffer_dialog_connect(AgsConnectable *connectable);
void ags_time_stretch_buffer_dialog_disconnect(AgsConnectable *connectable);

void ags_time_stretch_buffer_dialog_set_update(AgsApplicable *applicable, gboolean update);
void ags_time_stretch_buffer_dialog_apply(AgsApplicable *applicable);
void ags_time_stretch_buffer_dialog_reset(AgsApplicable *applicable);
gboolean ags_time_stretch_buffer_dialog_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_time_stretch_buffer_dialog
 * @short_description: time_stretch tool
 * @title: AgsTimeStretchBufferDialog
 * @section_id:
 * @include: ags/X/editor/ags_time_stretch_buffer_dialog.h
 *
 * The #AgsTimeStretchBufferDialog lets you time_stretch buffers.
 */

static gpointer ags_time_stretch_buffer_dialog_parent_class = NULL;

GType
ags_time_stretch_buffer_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_time_stretch_buffer_dialog = 0;

    static const GTypeInfo ags_time_stretch_buffer_dialog_info = {
      sizeof (AgsTimeStretchBufferDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_time_stretch_buffer_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTimeStretchBufferDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_time_stretch_buffer_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_time_stretch_buffer_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_time_stretch_buffer_dialog_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_time_stretch_buffer_dialog = g_type_register_static(GTK_TYPE_DIALOG,
								 "AgsTimeStretchBufferDialog", &ags_time_stretch_buffer_dialog_info,
								 0);
    
    g_type_add_interface_static(ags_type_time_stretch_buffer_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_time_stretch_buffer_dialog,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_time_stretch_buffer_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_time_stretch_buffer_dialog_class_init(AgsTimeStretchBufferDialogClass *time_stretch_buffer_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_time_stretch_buffer_dialog_parent_class = g_type_class_peek_parent(time_stretch_buffer_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) time_stretch_buffer_dialog;

  gobject->finalize = ags_time_stretch_buffer_dialog_finalize;

  /* properties */

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) time_stretch_buffer_dialog;

  widget->delete_event = ags_time_stretch_buffer_dialog_delete_event;
}

void
ags_time_stretch_buffer_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_time_stretch_buffer_dialog_connect;
  connectable->disconnect = ags_time_stretch_buffer_dialog_disconnect;
}

void
ags_time_stretch_buffer_dialog_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_time_stretch_buffer_dialog_set_update;
  applicable->apply = ags_time_stretch_buffer_dialog_apply;
  applicable->reset = ags_time_stretch_buffer_dialog_reset;
}

void
ags_time_stretch_buffer_dialog_init(AgsTimeStretchBufferDialog *time_stretch_buffer_dialog)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;

  time_stretch_buffer_dialog->flags = 0;

  g_object_set(time_stretch_buffer_dialog,
	       "title", i18n("time stretch buffers"),
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(time_stretch_buffer_dialog),
		     GTK_WIDGET(vbox),
		     FALSE, FALSE,
		     0);  
  
  /* frequency - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* frequency - label */
  label = (GtkLabel *) gtk_label_new(i18n("frequency"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* frequency - spin button */
  time_stretch_buffer_dialog->frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											   AGS_TIME_STRETCH_BUFFER_MAX_FREQUENCY,
											   0.001);
  gtk_spin_button_set_digits(time_stretch_buffer_dialog->frequency,
			     3);
  gtk_spin_button_set_value(time_stretch_buffer_dialog->frequency,
			    0.0);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) time_stretch_buffer_dialog->frequency,
		     FALSE, FALSE,
		     0);
  
  /* orig BPM - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* orig BPM - label */
  label = (GtkLabel *) gtk_label_new(i18n("orig BPM"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* orig BPM - spin button */
  time_stretch_buffer_dialog->orig_bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											  AGS_TIME_STRETCH_BUFFER_MAX_BPM,
											  1.0);
  gtk_spin_button_set_digits(time_stretch_buffer_dialog->orig_bpm,
			     2);
  gtk_spin_button_set_value(time_stretch_buffer_dialog->orig_bpm,
			    AGS_TIME_STRETCH_BUFFER_DEFAULT_BPM);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) time_stretch_buffer_dialog->orig_bpm,
		     FALSE, FALSE,
		     0);
  
  /* new BPM - hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* new BPM - label */
  label = (GtkLabel *) gtk_label_new(i18n("new BPM"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* new BPM - spin button */
  time_stretch_buffer_dialog->new_bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											 AGS_TIME_STRETCH_BUFFER_MAX_BPM,
											 1.0);
  gtk_spin_button_set_digits(time_stretch_buffer_dialog->new_bpm,
			     2);
  gtk_spin_button_set_value(time_stretch_buffer_dialog->new_bpm,
			    AGS_TIME_STRETCH_BUFFER_DEFAULT_BPM);
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) time_stretch_buffer_dialog->new_bpm,
		     FALSE, FALSE,
		     0);

  /* dialog buttons */
  gtk_dialog_add_buttons((GtkDialog *) time_stretch_buffer_dialog,
			 i18n("_Apply"), GTK_RESPONSE_APPLY,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_time_stretch_buffer_dialog_connect(AgsConnectable *connectable)
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;

  time_stretch_buffer_dialog = AGS_TIME_STRETCH_BUFFER_DIALOG(connectable);

  if((AGS_TIME_STRETCH_BUFFER_DIALOG_CONNECTED & (time_stretch_buffer_dialog->flags)) != 0){
    return;
  }

  time_stretch_buffer_dialog->flags |= AGS_TIME_STRETCH_BUFFER_DIALOG_CONNECTED;

  g_signal_connect(time_stretch_buffer_dialog, "response",
		   G_CALLBACK(ags_time_stretch_buffer_dialog_response_callback), time_stretch_buffer_dialog);
}

void
ags_time_stretch_buffer_dialog_disconnect(AgsConnectable *connectable)
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;

  time_stretch_buffer_dialog = AGS_TIME_STRETCH_BUFFER_DIALOG(connectable);

  if((AGS_TIME_STRETCH_BUFFER_DIALOG_CONNECTED & (time_stretch_buffer_dialog->flags)) == 0){
    return;
  }

  time_stretch_buffer_dialog->flags &= (~AGS_TIME_STRETCH_BUFFER_DIALOG_CONNECTED);

  g_object_disconnect(G_OBJECT(time_stretch_buffer_dialog),
		      "any_signal::response",
		      G_CALLBACK(ags_time_stretch_buffer_dialog_response_callback),
		      time_stretch_buffer_dialog,
		      NULL);
}

void
ags_time_stretch_buffer_dialog_finalize(GObject *gobject)
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;

  time_stretch_buffer_dialog = (AgsTimeStretchBufferDialog *) gobject;
  
  G_OBJECT_CLASS(ags_time_stretch_buffer_dialog_parent_class)->finalize(gobject);
}

void
ags_time_stretch_buffer_dialog_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_time_stretch_buffer_dialog_apply(AgsApplicable *applicable)
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;
    
  time_stretch_buffer_dialog = AGS_TIME_STRETCH_BUFFER_DIALOG(applicable);

  //TODO:JK: implement me
}

void
ags_time_stretch_buffer_dialog_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_time_stretch_buffer_dialog_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_time_stretch_buffer_dialog_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_time_stretch_buffer_dialog_new:
 *
 * Create a new #AgsTimeStretchBufferDialog.
 *
 * Returns: a new #AgsTimeStretchBufferDialog
 *
 * Since: 3.13.0
 */
AgsTimeStretchBufferDialog*
ags_time_stretch_buffer_dialog_new()
{
  AgsTimeStretchBufferDialog *time_stretch_buffer_dialog;

  time_stretch_buffer_dialog = (AgsTimeStretchBufferDialog *) g_object_new(AGS_TYPE_TIME_STRETCH_BUFFER_DIALOG,
									   NULL);

  return(time_stretch_buffer_dialog);
}
