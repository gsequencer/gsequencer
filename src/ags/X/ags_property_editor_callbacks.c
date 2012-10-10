#include <ags/X/ags_property_editor_callbacks.h>

void
ags_property_editor_enable_callback(GtkToggleButton *toggle,
				    AgsPropertyEditor *property_editor)
{
  if(gtk_toggle_button_get_active(toggle)){
    property_editor->flags |= AGS_PROPERTY_EDITOR_ENABLED;
  }else{
    property_editor->flags &= (~AGS_PROPERTY_EDITOR_ENABLED);
  }
}
