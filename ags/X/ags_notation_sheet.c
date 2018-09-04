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

#include <ags/X/ags_notation_sheet.h>
#include <ags/X/ags_notation_sheet_callbacks.h>

#include <ags/i18n.h>

void ags_notation_sheet_class_init(AgsNotationSheetClass *notation_sheet);
void ags_notation_sheet_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_sheet_init(AgsNotationSheet *notation_sheet);
void ags_notation_sheet_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_notation_sheet_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_notation_sheet_connect(AgsConnectable *connectable);
void ags_notation_sheet_disconnect(AgsConnectable *connectable);
void ags_notation_sheet_finalize(GObject *gobject);

void ags_notation_sheet_real_machine_changed(AgsNotationSheet *notation_sheet,
					     AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_notation_sheet_parent_class = NULL;
static guint notation_sheet_signals[LAST_SIGNAL];

/**
 * SECTION:ags_notation_sheet
 * @short_description: A composite widget to edit notation
 * @title: AgsNotationSheet
 * @section_id:
 * @include: ags/X/ags_notation_sheet.h
 *
 * #AgsNotationSheet is a composite widget to edit notation. You may select machines
 * or change sheet tool to do notation.
 */

GType
ags_notation_sheet_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_notation_sheet = 0;

    static const GTypeInfo ags_notation_sheet_info = {
      sizeof (AgsNotationSheetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_notation_sheet_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNotationSheet),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_notation_sheet_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_notation_sheet_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_notation_sheet = g_type_register_static(GTK_TYPE_VBOX,
						     "AgsNotationSheet", &ags_notation_sheet_info,
						     0);
    
    g_type_add_interface_static(ags_type_notation_sheet,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notation_sheet);
  }

  return g_define_type_id__volatile;
}

void
ags_notation_sheet_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_notation_sheet_connect;
  connectable->disconnect = ags_notation_sheet_disconnect;
}

void
ags_notation_sheet_class_init(AgsNotationSheetClass *notation_sheet)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_notation_sheet_parent_class = g_type_class_peek_parent(notation_sheet);

  /* GObjectClass */
  gobject = (GObjectClass *) notation_sheet;

  gobject->set_property = ags_notation_sheet_set_property;
  gobject->get_property = ags_notation_sheet_get_property;

  gobject->finalize = ags_notation_sheet_finalize;
  
  /* properties */
  /**
   * AgsNotationSheet:soundcard:
   *
   * The assigned #AgsSoundcard acting as default sink.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("assigned soundcard"),
				   i18n_pspec("The soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* AgsSheetClass */
  notation_sheet->machine_changed = ags_notation_sheet_real_machine_changed;

  /* signals */
  /**
   * AgsSheet::machine-changed:
   * @sheet: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   * 
   * Since: 2.0.0
   */
  notation_sheet_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(notation_sheet),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsNotationSheetClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_notation_sheet_init(AgsNotationSheet *notation_sheet)
{
  //TODO:JK: implement me
}

void
ags_notation_sheet_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsNotationSheet *notation_sheet;

  notation_sheet = AGS_NOTATION_SHEET(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = g_value_get_object(value);

      if(notation_sheet->soundcard == soundcard){
	return;
      }

      if(notation_sheet->soundcard != NULL){
	g_object_unref(notation_sheet->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(soundcard);
      }
      
      notation_sheet->soundcard = soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_sheet_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsNotationSheet *notation_sheet;

  notation_sheet = AGS_NOTATION_SHEET(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, notation_sheet->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_notation_sheet_connect(AgsConnectable *connectable)
{
  AgsNotationSheet *notation_sheet;

  notation_sheet = AGS_NOTATION_SHEET(connectable);

  if((AGS_NOTATION_SHEET_CONNECTED & (notation_sheet->flags)) != 0){
    return;
  }

  notation_sheet->flags |= AGS_NOTATION_SHEET_CONNECTED;  
  
  g_signal_connect((GObject *) notation_sheet->machine_selector, "changed",
		   G_CALLBACK(ags_notation_sheet_machine_changed_callback), (gpointer) notation_sheet);

  /* toolbar */
  ags_connectable_connect(AGS_CONNECTABLE(notation_sheet->notation_toolbar));

  /* machine selector */
  ags_connectable_connect(AGS_CONNECTABLE(notation_sheet->machine_selector));

  /* notation page */
  ags_connectable_connect(AGS_CONNECTABLE(notation_sheet->notation_page));
}

void
ags_notation_sheet_disconnect(AgsConnectable *connectable)
{
  AgsNotationSheet *notation_sheet;

  notation_sheet = AGS_NOTATION_SHEET(connectable);

  /* notation toolbar */
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_sheet->notation_toolbar)); 

  /* machine selector */
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_sheet->machine_selector));

  /* notation page */
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_sheet->notation_page));
}

void
ags_notation_sheet_finalize(GObject *gobject)
{
  AgsNotationSheet *notation_sheet;

  notation_sheet = AGS_NOTATION_SHEET(gobject);

  if(notation_sheet->soundcard != NULL){
    g_object_unref(notation_sheet->soundcard);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_notation_sheet_parent_class)->finalize(gobject);
}

void
ags_notation_sheet_real_machine_changed(AgsNotationSheet *notation_sheet,
					AgsMachine *machine)
{
  //TODO:JK: implement me
}

/**
 * ags_notation_sheet_machine_changed:
 * @notation_sheet: an #AgsNotationSheet
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of notation_sheet.
 *
 * Since: 2.0.0
 */
void
ags_notation_sheet_machine_changed(AgsNotationSheet *notation_sheet,
				   AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_NOTATION_SHEET(notation_sheet));

  g_object_ref((GObject *) notation_sheet);
  g_signal_emit((GObject *) notation_sheet,
		notation_sheet_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) notation_sheet);
}

/**
 * ags_notation_sheet_new:
 *
 * Creates an #AgsNotationSheet
 *
 * Returns: a new #AgsNotationSheet
 *
 * Since: 2.0.0
 */
AgsNotationSheet*
ags_notation_sheet_new()
{
  AgsNotationSheet *notation_sheet;

  notation_sheet = (AgsNotationSheet *) g_object_new(AGS_TYPE_NOTATION_SHEET,
						     NULL);

  return(notation_sheet);
}
