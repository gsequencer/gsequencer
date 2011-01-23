#include <ags/lib/ags_list.h>

void
ags_list_free_and_unref_link(GList *list)
{
  GList *list_next;

  while(list != NULL){
    list_next = list->next;
   
    g_object_unref((GObject *) list->data);
    g_list_free1(list);

    list = list_next;
  }
}
