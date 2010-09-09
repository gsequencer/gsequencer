#include "ags_pad.h"
#include "ags_pad_callbacks.h"

#include "../object/ags_marshal.h"
#include "ags_machine.h"

GType ags_pad_get_type(void);
void ags_pad_class_init(AgsPadClass *pad);
void ags_pad_init(AgsPad *pad);
void ags_pad_connect(AgsPad *pad);
void ags_pad_destroy(GtkObject *object);
void ags_pad_show(GtkWidget *widget);

void ags_pad_real_resize_lines(AgsPad *pad, GType line_type,
			       guint audio_channels, guint audio_channels_old);

enum{
  RESIZE_LINES,
  LAST_SIGNAL,
};

static guint pad_signals[LAST_SIGNAL];

GType
ags_pad_get_type(void)
{
  static GType pad_type = 0;

  if (!pad_type){
    static const GtkTypeInfo pad_info = {
      "AgsPad\0",
      sizeof(AgsPad), /* base_init */
      sizeof(AgsPadClass), /* base_finalize */
      (GtkClassInitFunc) ags_pad_class_init,
      (GtkObjectInitFunc) ags_pad_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };
    pad_type = gtk_type_unique (GTK_TYPE_VBOX, &pad_info);
  }

  return (pad_type);
}

void
ags_pad_class_init(AgsPadClass *pad)
{
  pad->resize_lines = ags_pad_real_resize_lines;

  pad_signals[RESIZE_LINES] =
    g_signal_new("resize_lines\0",
		 G_TYPE_FROM_CLASS (pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsPadClass, resize_lines),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG, G_TYPE_UINT, G_TYPE_UINT);
}

void
ags_pad_init(AgsPad *pad)
{
  GtkMenu *menu;
  GtkHBox *hbox;

  g_signal_connect((GObject *) pad, "parent_set\0",
		   G_CALLBACK(ags_pad_parent_set_callback), (gpointer) pad);

  pad->option = (GtkOptionMenu *) gtk_option_menu_new();
  menu = (GtkMenu *) gtk_menu_new();

  gtk_option_menu_set_menu(pad->option, (GtkWidget *) menu);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) pad->option, FALSE, FALSE, 0);

  hbox = (GtkHBox *) gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start((GtkBox *) pad, (GtkWidget *) hbox, FALSE, FALSE, 0);

  pad->group = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup("G\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->group, FALSE, FALSE, 0);

  pad->mute = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup("M\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->mute, FALSE, FALSE, 0);

  pad->solo = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup("S\0"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) pad->solo, FALSE, FALSE, 0);
}

void
ags_pad_connect(AgsPad *pad)
{
  g_signal_connect((GObject *) pad, "destroy\0",
		   G_CALLBACK(ags_pad_destroy_callback), (gpointer) pad);

  g_signal_connect((GObject *) pad, "show\0",
		   G_CALLBACK(ags_pad_show_callback), (gpointer) pad);

  g_signal_connect((GObject *) pad->option, "changed\0",
		   G_CALLBACK(ags_pad_option_changed_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->group, "clicked\0",
			 G_CALLBACK(ags_pad_group_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->mute, "clicked\0",
			 G_CALLBACK(ags_pad_mute_clicked_callback), (gpointer) pad);

  g_signal_connect_after((GObject *) pad->solo, "clicked\0",
			 G_CALLBACK(ags_pad_solo_clicked_callback), (gpointer) pad);
}

void
ags_pad_destroy(GtkObject *object)
{
}

void
ags_pad_show(GtkWidget *widget)
{
  AgsPad *pad;

  fprintf(stdout, "ags_pad_show\n\0");

  pad = AGS_PAD(widget);

  gtk_widget_show_all(gtk_option_menu_get_menu(pad->option));
}

void
ags_pad_real_resize_lines(AgsPad *pad, GType line_type,
			  guint audio_channels, guint audio_channels_old)
{
  //  AgsMachine *machine;
  AgsLine *line;
  AgsChannel *channel;
  guint i;

  if(audio_channels > audio_channels_old){
    //    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pad, AGS_TYPE_MACHINE);
    channel = ags_channel_nth(pad->channel, audio_channels_old);

    for(i = audio_channels_old; i < audio_channels; i++){
      line = ags_line_new();
      line->channel = channel;
      channel->line_widget = (GtkWidget *) line;
      gtk_menu_shell_insert((GtkMenuShell *) pad->option->menu, (GtkWidget *) line, i);

      channel = channel->next;
    }

    if(audio_channels_old == 0)
      pad->selected_line = (AgsLine *) gtk_container_get_children((GtkContainer *) pad->option->menu)->data;

    //    if(machine != NULL && GTK_WIDGET_VISIBLE((GtkWidget *) machine)){
      //     gtk_widget_show_all((GtkWidget *) line);
      //      ags_line_connect(line);
    //    }
  }else if(audio_channels < audio_channels_old){
  }
}

void ags_pad_resize_lines(AgsPad *pad, GType line_type,
			  guint audio_channels, guint audio_channels_old)
{
  g_return_if_fail(AGS_IS_PAD(pad));

  g_object_ref((GObject *) pad);
  g_signal_emit(G_OBJECT(pad),
		pad_signals[RESIZE_LINES], 0,
		line_type,
		audio_channels, audio_channels_old);
  g_object_unref((GObject *) pad);
}

AgsPad*
ags_pad_new()
{
  AgsPad *pad;

  pad = (AgsPad *) g_object_new(AGS_TYPE_PAD, NULL);

  return(pad);
}
