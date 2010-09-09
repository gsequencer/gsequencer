#include "ags_navigation.h"
#include "ags_navigation_callbacks.h"

GType ags_navigation_get_type(void);
void ags_navigation_class_init(AgsNavigationClass *navigation);
void ags_navigation_init(AgsNavigation *navigation);
void ags_navigation_connect(AgsNavigation *navigation);
void ags_navigation_destroy(GtkObject *object);
void ags_navigation_show(GtkWidget *widget);

GType
ags_navigation_get_type(void)
{
  static GType navigation_type = 0;

  if (!navigation_type){
    static const GtkTypeInfo navigation_info = {
      "AgsNavigation\0",
      sizeof(AgsNavigation), /* base_init */
      sizeof(AgsNavigationClass), /* base_finalize */
      (GtkClassInitFunc) ags_navigation_class_init,
      (GtkObjectInitFunc) ags_navigation_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    navigation_type = gtk_type_unique (GTK_TYPE_VBOX, &navigation_info);
  }

  return (navigation_type);
}

void
ags_navigation_class_init(AgsNavigationClass *navigation)
{
}

void
ags_navigation_init(AgsNavigation *navigation)
{
  GtkHBox *hbox;
  GtkLabel *label;

  navigation->devout = ags_devout_new();

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) navigation, (GtkWidget *) hbox, FALSE, FALSE, 2);

  navigation->expander = (GtkToggleButton *) gtk_toggle_button_new();
  gtk_box_pack_start((GtkBox*) hbox, (GtkWidget *) navigation->expander, FALSE, FALSE, 2);
  gtk_container_add((GtkContainer *) navigation->expander,
		    (GtkWidget *) gtk_arrow_new(GTK_ARROW_RIGHT, GTK_SHADOW_NONE));

  label = (GtkLabel *) gtk_label_new("bpm\0");
  gtk_box_pack_start((GtkBox*) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->bpm = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 1000.0, 1.0);
  navigation->bpm->adjustment->value = 120.0;
  gtk_box_pack_start((GtkBox*) hbox, (GtkWidget *) navigation->bpm, FALSE, FALSE, 2);


  navigation->rewind = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
							"image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_REWIND, GTK_ICON_SIZE_LARGE_TOOLBAR),
							NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->rewind, FALSE, FALSE, 0);

  navigation->previous = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						    "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_LARGE_TOOLBAR),
						    NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->previous, FALSE, FALSE, 0);

  navigation->play = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						      "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_LARGE_TOOLBAR),
						      NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->play, FALSE, FALSE, 0);

  navigation->stop = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						"image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_LARGE_TOOLBAR),
						NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->stop, FALSE, FALSE, 0);

  navigation->next = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						"image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_NEXT, GTK_ICON_SIZE_LARGE_TOOLBAR),
						NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->next, FALSE, FALSE, 0);

  navigation->forward = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
							 "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_MEDIA_FORWARD, GTK_ICON_SIZE_LARGE_TOOLBAR),
							 NULL);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->forward, FALSE, FALSE, 0);


  navigation->loop = (GtkCheckButton *) gtk_check_button_new_with_label("loop\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->loop, FALSE, FALSE, 2);

  label = (GtkLabel *) gtk_label_new("position\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->position_min = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65000.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->position_min, FALSE, FALSE, 2);

  navigation->position_sec = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 60.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->position_sec, FALSE, FALSE, 2);


  label = (GtkLabel *) gtk_label_new("duration\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->duration_min = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65000.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->duration_min, FALSE, FALSE, 2);

  navigation->duration_sec = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 60.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->duration_sec, FALSE, FALSE, 2);


  /* expansion */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  //  GTK_WIDGET_SET_FLAGS((GtkWidget *) hbox, GTK_NO_SHOW_ALL);
  gtk_box_pack_start((GtkBox *) navigation, (GtkWidget *) hbox, FALSE, FALSE, 2);

  label = (GtkLabel *) gtk_label_new("loop L\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->loop_left_min = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65000.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->loop_left_min, FALSE, FALSE, 2);

  navigation->loop_left_sec = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 60.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->loop_left_sec, FALSE, FALSE, 2);

  label = (GtkLabel *) gtk_label_new("loop R\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) label, FALSE, FALSE, 2);

  navigation->loop_right_min = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 65000.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->loop_right_min, FALSE, FALSE, 2);

  navigation->loop_right_sec = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 60.0, 1.0);
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->loop_right_sec, FALSE, FALSE, 2);

  navigation->raster = (GtkCheckButton *) gtk_check_button_new_with_label("raster\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) navigation->raster, FALSE, FALSE, 2);
}

void
ags_navigation_connect(AgsNavigation *navigation)
{
  g_signal_connect((GObject *) navigation, "destroy\0",
		   G_CALLBACK(ags_navigation_destroy_callback), (gpointer) navigation);

  g_signal_connect_after((GObject *) navigation, "show\0",
			 G_CALLBACK(ags_navigation_show_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->expander, "clicked\0",
		   G_CALLBACK(ags_navigation_expander_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->bpm, "value-changed\0",
		   G_CALLBACK(ags_navigation_bpm_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->rewind, "clicked\0",
		   G_CALLBACK(ags_navigation_rewind_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->previous, "clicked\0",
		   G_CALLBACK(ags_navigation_prev_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->play, "clicked\0",
		   G_CALLBACK(ags_navigation_play_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->stop, "clicked\0",
		   G_CALLBACK(ags_navigation_stop_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->next, "clicked\0",
		   G_CALLBACK(ags_navigation_next_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->forward, "clicked\0",
		   G_CALLBACK(ags_navigation_forward_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->loop, "clicked\0",
		   G_CALLBACK(ags_navigation_loop_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->position_min, "value-changed\0",
		   G_CALLBACK(ags_navigation_position_min_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->position_sec, "value-changed\0",
		   G_CALLBACK(ags_navigation_position_sec_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->duration_min, "value-changed\0",
		   G_CALLBACK(ags_navigation_duration_min_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->duration_sec, "value-changed\0",
		   G_CALLBACK(ags_navigation_duration_sec_callback), (gpointer) navigation);

  /* expansion */
  g_signal_connect((GObject *) navigation->loop_left_min, "value-changed\0",
		   G_CALLBACK(ags_navigation_loop_left_min_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->loop_left_sec, "value-changed\0",
		   G_CALLBACK(ags_navigation_loop_left_sec_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->loop_right_min, "value-changed\0",
		   G_CALLBACK(ags_navigation_loop_right_min_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->loop_right_sec, "value-changed\0",
		   G_CALLBACK(ags_navigation_loop_right_sec_callback), (gpointer) navigation);

  g_signal_connect((GObject *) navigation->raster, "clicked\0",
		   G_CALLBACK(ags_navigation_raster_callback), (gpointer) navigation);
}

void
ags_navigation_destroy(GtkObject *object)
{
}

void
ags_navigation_show(GtkWidget *widget)
{
  GList *list;

  list = gtk_container_get_children((GtkContainer *) widget);
  //  GTK_WIDGET_UNSET_FLAGS((GtkWidget *) list->next->data, GTK_NO_SHOW_ALL);
}

AgsNavigation*
ags_navigation_new()
{
  AgsNavigation *navigation;

  navigation = (AgsNavigation *) g_object_new(AGS_TYPE_NAVIGATION, NULL);

  return(navigation);
}
