AgsMachine *machine;
AgsDevout *devout;
AgsAudio *audio;
AgsChannel *channel;
AgsRecallContainer *echo_container;

/* some pseudo code */
machine = (AgsMachine *) gtk_widget_get_ancestor(widget,
						 AGS_TYPE_MACHINE);

/* retrieve some essencial objects */
audio = machine->audio;
devout = audio->devout;

/* create the container */
recall_container = (AgsRecallContainer *) g_object_new(AGS_TYPE_RECALL_CONTAINER,
						       NULL);
