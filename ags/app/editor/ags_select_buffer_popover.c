/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/editor/ags_select_buffer_popover.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_wave_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_select_buffer_popover_class_init(AgsSelectBufferPopoverClass *select_buffer_popover);
void ags_select_buffer_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_select_buffer_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_select_buffer_popover_init(AgsSelectBufferPopover *select_buffer_popover);
void ags_select_buffer_popover_finalize(GObject *gobject);

gboolean ags_select_buffer_popover_is_connected(AgsConnectable *connectable);
void ags_select_buffer_popover_connect(AgsConnectable *connectable);
void ags_select_buffer_popover_disconnect(AgsConnectable *connectable);

void ags_select_buffer_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_select_buffer_popover_apply(AgsApplicable *applicable);
void ags_select_buffer_popover_reset(AgsApplicable *applicable);

gboolean ags_select_buffer_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
							guint keyval,
							guint keycode,
							GdkModifierType state,
							AgsSelectBufferPopover *select_buffer_popover);
void ags_select_buffer_popover_key_released_callback(GtkEventControllerKey *event_controller,
						     guint keyval,
						     guint keycode,
						     GdkModifierType state,
						     AgsSelectBufferPopover *select_buffer_popover);
gboolean ags_select_buffer_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						      GdkModifierType keyval,
						      AgsSelectBufferPopover *select_buffer_popover);

void ags_select_buffer_popover_activate_button_callback(GtkButton *activate_button,
							AgsSelectBufferPopover *select_buffer_popover);

/**
 * SECTION:ags_select_buffer_popover
 * @short_description: select tool
 * @title: AgsSelectBufferPopover
 * @section_id:
 * @include: ags/app/editor/ags_select_buffer_popover.h
 *
 * The #AgsSelectBufferPopover lets you select buffers.
 */

static gpointer ags_select_buffer_popover_parent_class = NULL;

GType
ags_select_buffer_popover_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_select_buffer_popover = 0;

    static const GTypeInfo ags_select_buffer_popover_info = {
      sizeof (AgsSelectBufferPopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_select_buffer_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSelectBufferPopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_select_buffer_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_select_buffer_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_select_buffer_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_select_buffer_popover = g_type_register_static(GTK_TYPE_POPOVER,
							    "AgsSelectBufferPopover", &ags_select_buffer_popover_info,
							    0);
    
    g_type_add_interface_static(ags_type_select_buffer_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_select_buffer_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_select_buffer_popover);
  }

  return g_define_type_id__static;
}

void
ags_select_buffer_popover_class_init(AgsSelectBufferPopoverClass *select_buffer_popover)
{
  GObjectClass *gobject;

  ags_select_buffer_popover_parent_class = g_type_class_peek_parent(select_buffer_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) select_buffer_popover;

  gobject->finalize = ags_select_buffer_popover_finalize;
}

void
ags_select_buffer_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_select_buffer_popover_is_connected;  
  connectable->connect = ags_select_buffer_popover_connect;
  connectable->disconnect = ags_select_buffer_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_select_buffer_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_select_buffer_popover_set_update;
  applicable->apply = ags_select_buffer_popover_apply;
  applicable->reset = ags_select_buffer_popover_reset;
}

void
ags_select_buffer_popover_init(AgsSelectBufferPopover *select_buffer_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  select_buffer_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) select_buffer_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_select_buffer_popover_key_pressed_callback), select_buffer_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_select_buffer_popover_key_released_callback), select_buffer_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_select_buffer_popover_modifiers_callback), select_buffer_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_popover_set_child((GtkPopover *) select_buffer_popover,
			(GtkWidget *) vbox);

  /* copy selection */
  select_buffer_popover->copy_selection = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("copy selection"));
  gtk_check_button_set_active(select_buffer_popover->copy_selection,
			      TRUE);
  gtk_box_append(vbox,
		 GTK_WIDGET(select_buffer_popover->copy_selection));  
  
  /* select x0 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) vbox,
		 (GtkWidget *) hbox);

  /* select x0 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x0"));
  gtk_box_append(hbox,
		 GTK_WIDGET(label));

  /* select x0 - spin button */
  select_buffer_popover->select_x0 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										      AGS_SELECT_BUFFER_MAX_BEATS,
										      0.25);
  gtk_spin_button_set_digits(select_buffer_popover->select_x0,
			     2);
  gtk_spin_button_set_value(select_buffer_popover->select_x0,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) select_buffer_popover->select_x0);
  
  /* select x1 - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* select x1 - label */
  label = (GtkLabel *) gtk_label_new(i18n("select x1"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* select x1 - spin button */
  select_buffer_popover->select_x1 = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										      AGS_SELECT_BUFFER_MAX_BEATS,
										      0.25);
  gtk_spin_button_set_digits(select_buffer_popover->select_x1,
			     2);
  gtk_spin_button_set_value(select_buffer_popover->select_x1,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) select_buffer_popover->select_x1);
  
  /* buttons */
  select_buffer_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) select_buffer_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) select_buffer_popover->action_area);

  select_buffer_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(select_buffer_popover->action_area,
		 (GtkWidget *) select_buffer_popover->activate_button);

  g_signal_connect(select_buffer_popover->activate_button, "clicked",
		   G_CALLBACK(ags_select_buffer_popover_activate_button_callback), select_buffer_popover);

  gtk_popover_set_default_widget((GtkPopover *) select_buffer_popover,
				 (GtkWidget *) select_buffer_popover->activate_button);
}

gboolean
ags_select_buffer_popover_is_connected(AgsConnectable *connectable)
{
  AgsSelectBufferPopover *select_buffer_popover;
  
  gboolean is_connected;
  
  select_buffer_popover = AGS_SELECT_BUFFER_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (select_buffer_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_select_buffer_popover_connect(AgsConnectable *connectable)
{
  AgsSelectBufferPopover *select_buffer_popover;

  select_buffer_popover = AGS_SELECT_BUFFER_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  select_buffer_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_select_buffer_popover_disconnect(AgsConnectable *connectable)
{
  AgsSelectBufferPopover *select_buffer_popover;

  select_buffer_popover = AGS_SELECT_BUFFER_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  select_buffer_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_select_buffer_popover_finalize(GObject *gobject)
{
  AgsSelectBufferPopover *select_buffer_popover;

  select_buffer_popover = (AgsSelectBufferPopover *) gobject;
  
  G_OBJECT_CLASS(ags_select_buffer_popover_parent_class)->finalize(gobject);
}

void
ags_select_buffer_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_select_buffer_popover_apply(AgsApplicable *applicable)
{
  AgsSelectBufferPopover *select_buffer_popover;
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  AgsNotebook *notebook;
  AgsWaveEdit *focused_wave_edit;

  AgsAudio *audio;

  AgsTimestamp *timestamp;

  AgsApplicationContext *application_context;

  GObject *output_soundcard;
  
  xmlDoc *clipboard;
  xmlNode *audio_node, *wave_node;

  GList *start_list_wave, *list_wave;

  xmlChar *buffer;

  guint samplerate;
  guint buffer_size;
  gdouble delay;
  guint64 relative_offset;
  int size;
  guint x0, y0;
  guint x1, y1;
  gint i;
  
  gboolean copy_selection;
    
  select_buffer_popover = AGS_SELECT_BUFFER_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;

  focused_wave_edit = (AgsWaveEdit *) composite_editor->wave_edit->focused_edit;
    
  notebook = composite_editor->wave_edit->channel_selector;
  
  audio = machine->audio;

  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       "wave", &start_list_wave,
	       NULL);

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));
  
  /* get some values */
  copy_selection = gtk_check_button_get_active(select_buffer_popover->copy_selection);

  x0 = gtk_spin_button_get_value_as_int(select_buffer_popover->select_x0);
  x0 = delay * buffer_size * x0;
  
  x1 = gtk_spin_button_get_value_as_int(select_buffer_popover->select_x1);
  x1 = delay * buffer_size * x1;
  
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  relative_offset = samplerate * AGS_WAVE_DEFAULT_BUFFER_LENGTH;
  
  /* select buffer */
  if(copy_selection){
    /* create document */
    clipboard = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
  
    /* create root node */
    audio_node = xmlNewNode(NULL, BAD_CAST "audio");
    xmlDocSetRootElement(clipboard, audio_node);
  }
  
  i = 0;
  
  while((i = ags_notebook_next_active_tab(notebook,
					  i)) != -1){
    list_wave = start_list_wave;
    timestamp->timer.ags_offset.offset = 0;

    while((list_wave = ags_wave_find_near_timestamp(list_wave, i,
						    timestamp)) != NULL){
      ags_wave_add_region_to_selection(AGS_WAVE(list_wave->data),
				       x0, x1,
				       TRUE);
      
      if(copy_selection){
	wave_node = ags_wave_copy_selection(AGS_WAVE(list_wave->data));
	xmlAddChild(audio_node, wave_node);      
      }

      /* iterate */
      timestamp->timer.ags_offset.offset += relative_offset;

      list_wave = list_wave->next;
    }

    i++;
  }    

  g_object_unref(timestamp);
  
  g_list_free_full(start_list_wave,
		   (GDestroyNotify) g_object_unref);

  g_object_unref(output_soundcard);
  
  /* write to clipboard */
  if(copy_selection){
    xmlDocDumpFormatMemoryEnc(clipboard, &buffer, &size, "UTF-8", TRUE);

    gdk_clipboard_set_text(gdk_display_get_clipboard(gdk_display_get_default()),
			   buffer);
    
    xmlFreeDoc(clipboard);
  }
}

void
ags_select_buffer_popover_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

gboolean
ags_select_buffer_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
					       guint keyval,
					       guint keycode,
					       GdkModifierType state,
					       AgsSelectBufferPopover *select_buffer_popover)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_select_buffer_popover_key_released_callback(GtkEventControllerKey *event_controller,
						guint keyval,
						guint keycode,
						GdkModifierType state,
						AgsSelectBufferPopover *select_buffer_popover)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	gtk_popover_popdown((GtkPopover *) select_buffer_popover);	
      }
      break;
    }
  }
}

gboolean
ags_select_buffer_popover_modifiers_callback(GtkEventControllerKey *event_controller,
					     GdkModifierType keyval,
					     AgsSelectBufferPopover *select_buffer_popover)
{
  return(FALSE);
}

void
ags_select_buffer_popover_activate_button_callback(GtkButton *activate_button,
						   AgsSelectBufferPopover *select_buffer_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(select_buffer_popover));
}

/**
 * ags_select_buffer_popover_new:
 *
 * Create a new #AgsSelectBufferPopover.
 *
 * Returns: a new #AgsSelectBufferPopover
 *
 * Since: 6.11.0
 */
AgsSelectBufferPopover*
ags_select_buffer_popover_new()
{
  AgsSelectBufferPopover *select_buffer_popover;

  select_buffer_popover = (AgsSelectBufferPopover *) g_object_new(AGS_TYPE_SELECT_BUFFER_POPOVER,
								  NULL);

  return(select_buffer_popover);
}
