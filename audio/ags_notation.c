#include "ags_notation.h"

#include <stdlib.h>

void ags_notation_class_init(AgsNotationClass *notation);
void ags_notation_init(AgsNotation *notation);
void ags_notation_finalize(GObject *object);

static gpointer ags_notation_parent_class = NULL;

GType
ags_notation_get_type()
{
  static GType notation_type = 0;

  if(!notation_type){
    static const GTypeInfo notation_info = {
      sizeof(AgsNotationClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_notation_class_init,
      NULL,
      NULL,
      sizeof(AgsNotation),
      0,
      (GInstanceInitFunc) ags_notation_init,
    };
    notation_type = g_type_register_static(G_TYPE_OBJECT, "AgsNotation\0", &notation_info, 0);
  }
  return(notation_type);
}

void 
ags_notation_class_init(AgsNotationClass *notation)
{
  GObjectClass *gobject;

  ags_notation_parent_class = g_type_class_peek_parent(notation);

  gobject = (GObjectClass *) notation;
  gobject->finalize = ags_notation_finalize;
}

void
ags_notation_init(AgsNotation *notation)
{
  notation->flags = 0;

  notation->audio = NULL;

  notation->base_frequence = 0;

  notation->tact = "1/4\0";
  notation->bpm = 120;

  notation->note = NULL;
  notation->pads = 0;
}

void
ags_notation_finalize(GObject *gobject)
{
  AgsNotation *notation;
  GList *list, *list_next;

  notation = AGS_NOTATION(gobject);

  list = notation->note;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }

  G_OBJECT_CLASS(ags_notation_parent_class)->finalize(gobject);
}

void
ags_notation_connect(AgsNotation *notation)
{
  g_signal_connect(G_OBJECT(notation), "finalize\0",
		   G_CALLBACK(ags_notation_finalize), NULL);
}

void
ags_notation_add_note(AgsNotation *notation, AgsNote *note)
{
  GList *list, *list_new;
  void ags_notation_add_note_add1(){
    list_new = (GList *) malloc(sizeof(GList));
    list_new->data = (gpointer) note;

    if(list->prev == NULL){
      list_new->prev = NULL;
      list_new->next = list;
      list->prev = list_new;
      notation->note = list_new;
    }else{
      list_new->prev = list->prev;
      list_new->next = list;
      list->prev = list_new;
      list_new->prev->next = list_new;
    }
  }

  list = notation->note;

  if(list == NULL){
    list_new = g_list_alloc();
    list_new->data = (gpointer) note;
    notation->note = list_new;
    return;
  }

  while(list->next != NULL){
    if(((AgsNote *) (list->data))->x[0] >= note->x[0]){
      ags_notation_add_note_add1();
      return;
    }

    list = list->next;
  }

  if(((AgsNote *) (list->data))->x[0] >= note->x[0])
    ags_notation_add_note_add1();
  else{
    list_new = (GList *) malloc(sizeof(GList));
    list_new->data = (gpointer) note;
    list_new->next = NULL;
    list_new->prev = list;
    list->next = list_new;
  }
}

void
ags_note_convert(AgsNote *note)
{
}

AgsNote*
ags_note_alloc()
{
  AgsNote *note;

  note = (AgsNote *) malloc(sizeof(AgsNote));

  note->flags = 0;

  note->x[0] = 0;
  note->x[1] = 0;
  note->y = 0;

  return(note);
}

AgsNotation*
ags_notation_new()
{
  AgsNotation *notation;

  notation = (AgsNotation *) g_object_new(AGS_TYPE_NOTATION, NULL);

  return(notation);
}
