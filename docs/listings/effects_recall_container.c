AgsMachine *machine;

AgsAudio *audio;
AgsChannel *channel;
AgsRecallContainer *echo_container;

GObject *soundcard;

/* some pseudo code */
machine = (AgsMachine *) gtk_widget_get_ancestor(widget,
                                                 AGS_TYPE_MACHINE);

/* retrieve some essencial objects */
audio = machine->audio;
soundcard = audio->soundcard;

/* create the container */
recall_container = (AgsRecallContainer *) g_object_new(AGS_TYPE_RECALL_CONTAINER,
                                                       NULL);
ags_audio_add_recall_container(audio,
			       (GObject *) recall_container);
