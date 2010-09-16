#include "ags_menu_bar_callbacks.h"

#include "../file/ags_file.h"

#include "../audio/ags_input.h"
#include "../audio/ags_output.h"

#include "ags_window.h"

#include "machine/ags_panel.h"
#include "machine/ags_mixer.h"
#include "machine/ags_drum.h"
#include "machine/ags_matrix.h"
#include "machine/ags_synth.h"
#include "machine/ags_ffplayer.h"

void ags_menu_bar_open_ok_callback(GtkWidget *widget, AgsMenuBar *menu_bar);
void ags_menu_bar_open_cancel_callback(GtkWidget *widget, AgsMenuBar *menu_bar);

void ags_menu_bar_save_as_ok_callback(GtkWidget *widget, AgsMenuBar *menu_bar);
void ags_menu_bar_save_as_cancel_callback(GtkWidget *widget, AgsMenuBar *menu_bar);

gboolean
ags_menu_bar_destroy_callback(GtkObject *object, AgsMenuBar *menu_bar)
{
  ags_menu_bar_destroy(object);

  return(TRUE);
}

void
ags_menu_bar_show_callback(GtkWidget *widget, AgsMenuBar *menu_bar)
{
  ags_menu_bar_show(widget);
}

void
ags_menu_bar_open_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  GtkFileSelection *file_selection;

  file_selection = (GtkFileSelection *) gtk_file_selection_new(g_strdup("open file\0"));
  gtk_file_selection_set_select_multiple(file_selection, FALSE);

  gtk_widget_show_all((GtkWidget *) file_selection);

  g_signal_connect((GObject *) file_selection->ok_button, "clicked\0",
		   G_CALLBACK(ags_menu_bar_open_ok_callback), menu_bar);
  g_signal_connect((GObject *) file_selection->cancel_button, "clicked\0",
		   G_CALLBACK(ags_menu_bar_open_cancel_callback), menu_bar);
}

void
ags_menu_bar_open_ok_callback(GtkWidget *widget, AgsMenuBar *menu_bar)
{
  GtkFileSelection *file_selection;
  AgsFile *file;

  file_selection = (GtkFileSelection *) gtk_widget_get_ancestor(widget, GTK_TYPE_DIALOG);

  file = ags_file_new();
  file->name = g_strdup(gtk_file_selection_get_filename(file_selection));
  ags_file_read(file);

  ags_file_destroy(file);
  gtk_widget_destroy((GtkWidget *) file_selection);
}

void
ags_menu_bar_open_cancel_callback(GtkWidget *widget, AgsMenuBar *menu_bar)
{
  GtkFileSelection *file_selection;

  file_selection = (GtkFileSelection *) gtk_widget_get_ancestor(widget, GTK_TYPE_DIALOG);
  gtk_widget_destroy((GtkWidget *) file_selection);
}

void
ags_menu_bar_save_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsFile *file;

  file = ags_file_new();
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);
  file->window = (GtkWidget *) window;
  file->name = g_strdup(window->name);
  ags_file_write(file);
  ags_file_destroy(file);
}

void
ags_menu_bar_save_as_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  GtkFileSelection *file_selection;

  file_selection = (GtkFileSelection *) gtk_file_selection_new(g_strdup("save file as\0"));
  gtk_file_selection_set_select_multiple(file_selection, FALSE);

  gtk_widget_show_all((GtkWidget *) file_selection);

  g_signal_connect((GObject *) file_selection->ok_button, "clicked\0",
		   G_CALLBACK(ags_menu_bar_save_as_ok_callback), (gpointer) menu_bar);
  g_signal_connect((GObject *) file_selection->cancel_button, "clicked\0",
		   G_CALLBACK(ags_menu_bar_save_as_cancel_callback), (gpointer) menu_bar);
}

void
ags_menu_bar_save_as_ok_callback(GtkWidget *widget, AgsMenuBar *menu_bar)
{
  GtkFileSelection *file_selection;
  AgsWindow *window;
  AgsFile *file;

  file_selection = (GtkFileSelection *) gtk_widget_get_ancestor(widget, GTK_TYPE_FILE_SELECTION);

  file = ags_file_new();
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) menu_bar);
  file->window = (GtkWidget *) window;
  file->name = g_strdup(gtk_file_selection_get_filename((GtkFileSelection *) file_selection));
  ags_file_write(file);

  ags_file_destroy(file);
  gtk_widget_destroy((GtkWidget *) file_selection);
}

void
ags_menu_bar_save_as_cancel_callback(GtkWidget *widget, AgsMenuBar *menu_bar)
{
  GtkFileSelection *file_selection;

  file_selection = (GtkFileSelection *) gtk_widget_get_ancestor(widget, GTK_TYPE_FILE_SELECTION);
  gtk_widget_destroy((GtkWidget *) file_selection);
}

void
ags_menu_bar_quit_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  gtk_main_quit();
}


void
ags_menu_bar_add_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
}


void
ags_menu_bar_add_panel_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  GtkWidget *widget;
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  widget = (GtkWidget*) ags_panel_new();
  gtk_box_pack_start((GtkBox *) window->machines,
		     widget,
		     FALSE, FALSE, 0);

  ags_machine_connect((AgsMachine *) widget);
  gtk_widget_show_all(widget);
}

void
ags_menu_bar_add_mixer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  GtkWidget *widget;
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  widget = (GtkWidget*) ags_mixer_new();
  gtk_box_pack_start((GtkBox *) window->machines,
		     widget,
		     FALSE, FALSE, 0);

  ags_mixer_connect((AgsMixer *) widget);
  ags_machine_connect((AgsMachine *) widget);
  gtk_widget_show_all(widget);
}

void
ags_menu_bar_add_drum_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsDrum *drum;
  GtkWidget *widget;
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  drum = ags_drum_new();
  widget = (GtkWidget*) drum;
  gtk_box_pack_start((GtkBox *) window->machines,
		     widget,
		     FALSE, FALSE, 0);

  drum->machine.audio->audio_channels = 2;
  ags_audio_set_pads(drum->machine.audio, AGS_TYPE_INPUT, 8);
  ags_audio_set_pads(drum->machine.audio, AGS_TYPE_OUTPUT, 1);

  ags_drum_connect(drum);
  ags_machine_connect((AgsMachine *) widget);

  gtk_widget_show_all(widget);
}

void
ags_menu_bar_add_matrix_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsMatrix *matrix;
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  matrix = ags_matrix_new();
  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) matrix,
		     FALSE, FALSE, 0);

  matrix->machine.audio->audio_channels = 1;
  ags_audio_set_pads(matrix->machine.audio, AGS_TYPE_INPUT, 78);
  ags_audio_set_pads(matrix->machine.audio, AGS_TYPE_OUTPUT, 1);

  ags_matrix_connect(matrix);
  ags_machine_connect((AgsMachine *) matrix);

  gtk_widget_show_all((GtkWidget *) matrix);
}

void
ags_menu_bar_add_synth_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsSynth *synth;
  AgsWindow *window;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  synth = ags_synth_new();
  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) synth,
		     FALSE, FALSE, 0);

  synth->machine.audio->audio_channels = 1;
  ags_audio_set_pads((AgsAudio*) synth->machine.audio, AGS_TYPE_OUTPUT, 78);
  ags_audio_set_pads((AgsAudio*) synth->machine.audio, AGS_TYPE_INPUT, 2);

  ags_synth_connect(synth);
  ags_machine_connect((AgsMachine *) synth);

  gtk_widget_show_all((GtkWidget *) synth);
}

void
ags_menu_bar_add_ffplayer_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  AgsWindow *window;
  AgsFFPlayer *ffplayer;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, AGS_TYPE_WINDOW);

  ffplayer =  ags_ffplayer_new();
  gtk_box_pack_start((GtkBox *) window->machines,
		     (GtkWidget *) ffplayer,
		     FALSE, FALSE, 0);

  //  ffplayer->machine.audio->frequence = ;
  ags_audio_set_audio_channels(ffplayer->machine.audio, 2);

  ags_audio_set_pads(ffplayer->machine.audio, AGS_TYPE_INPUT, 76);
  ags_audio_set_pads(ffplayer->machine.audio, AGS_TYPE_OUTPUT, 1);

  ags_machine_connect((AgsMachine *) ffplayer);
  ags_ffplayer_connect(ffplayer);

  gtk_widget_show_all((GtkWidget *) ffplayer);
}


void
ags_menu_bar_remove_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
}

void
ags_menu_bar_about_callback(GtkWidget *menu_item, AgsMenuBar *menu_bar)
{
  static FILE *file = NULL;
  struct stat sb;
  static gchar *license;

  gchar *authors[] = { "joel kraehemann\0", NULL }; 

  if(file == NULL){
    file = fopen("./license/gpl-license\0", "r\0");
    stat("./license/gpl-license", &sb);
    license = (gchar *) malloc((sb.st_size + 1) * sizeof(gchar));
    fread(license, sizeof(char), sb.st_size, file);
    license[sb.st_size] = '\0';
    fclose(file);
  }

  gtk_show_about_dialog((GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) menu_bar, GTK_TYPE_WINDOW),
			"program-name\0", "ags\0",
			"authors\0", authors,
			"license\0", license,
			"title\0", "ags\0",
			NULL);
}
