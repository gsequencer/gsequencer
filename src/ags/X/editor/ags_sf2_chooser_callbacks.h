#ifndef __AGS_SF2_CHOOSER_CALLBACKS_H__
#define __AGS_SF2_CHOOSER_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_sf2_chooser.h>

void ags_sf2_chooser_preset_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser);
void ags_sf2_chooser_instrument_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser);
void ags_sf2_chooser_sample_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser);

#endif /*__AGS_SF2_CHOOSER_CALLBACKS_H__*/
