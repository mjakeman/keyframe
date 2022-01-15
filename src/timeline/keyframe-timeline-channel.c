#include "keyframe-timeline-channel.h"

struct _KeyframeTimelineChannel
{
    GtkWidget parent_instance;

    GtkWidget *info_box;
    GtkWidget *track;
};

G_DEFINE_FINAL_TYPE (KeyframeTimelineChannel, keyframe_timeline_channel, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_TRACK,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

GtkWidget *
keyframe_timeline_channel_new (void)
{
    return GTK_WIDGET (g_object_new (KEYFRAME_TYPE_TIMELINE_CHANNEL, NULL));
}

static void
keyframe_timeline_channel_finalize (GObject *object)
{
    KeyframeTimelineChannel *self = (KeyframeTimelineChannel *)object;

    gtk_widget_unparent (self->info_box);

    if (self->track)
        gtk_widget_unparent (self->track);

    G_OBJECT_CLASS (keyframe_timeline_channel_parent_class)->finalize (object);
}

static void
keyframe_timeline_channel_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
    KeyframeTimelineChannel *self = KEYFRAME_TIMELINE_CHANNEL (object);

    switch (prop_id)
    {
    case PROP_TRACK:
        g_value_set_object (value, keyframe_timeline_channel_get_track (self));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_timeline_channel_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
    KeyframeTimelineChannel *self = KEYFRAME_TIMELINE_CHANNEL (object);

    switch (prop_id)
    {
    case PROP_TRACK:
        KeyframeTrack *track = g_value_get_object (value);
        keyframe_timeline_channel_set_track (self, track);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_timeline_channel_class_init (KeyframeTimelineChannelClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_channel_finalize;
    object_class->get_property = keyframe_timeline_channel_get_property;
    object_class->set_property = keyframe_timeline_channel_set_property;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_css_name (widget_class, "channel");

    properties [PROP_TRACK] = g_param_spec_object ("track", "Track", "Track",
                                                   KEYFRAME_TYPE_TRACK,
                                                   G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

/**
 * keyframe_timeline_channel_set_track: (attributes org.gtk.Method.set_property=track)
 * @self: The channel widget that will contain the track
 * @track: A `KeyframeTrack` for editing a layer or property
 *
 * Sets a track widget (the right hand side of the divider) on a channel (a row
 * in the timeline) which is responsible for editing the channel's contents.
 */
void
keyframe_timeline_channel_set_track (KeyframeTimelineChannel *self,
                                     KeyframeTrack   *track)
{
    if (self->track)
        gtk_widget_unparent (self->track);

    self->track = track;

    if (self->track)
    {
        gtk_widget_set_hexpand (track, TRUE);
        gtk_widget_set_parent (track, GTK_WIDGET (self));
    }

    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_TRACK]);
}

/**
 * keyframe_timeline_channel_get_track: (attributes org.gtk.Method.get_property=track)
 * @self: The channel widget which contains the track
 *
 * See [method@Keyframe.TimelineChannel.set_track].
 *
 * Returns: (nullable) (transfer none): The track widget of @self
 */
KeyframeTrack *
keyframe_timeline_channel_get_track (KeyframeTimelineChannel *self)
{
    return self->track;
}

GtkWidget *
keyframe_timeline_channel_get_info_box (KeyframeTimelineChannel *self)
{
    return self->info_box;
}

static void
keyframe_timeline_channel_init (KeyframeTimelineChannel *self)
{
    GtkLayoutManager *layout = gtk_box_layout_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_set_spacing (GTK_BOX_LAYOUT (layout), 5);
    gtk_widget_set_layout_manager (GTK_WIDGET (self), layout);

    GtkWidget *info_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_spacing (GTK_BOX (info_box), 5);
    gtk_widget_add_css_class (info_box, "infobox");
    gtk_widget_set_size_request (info_box, 400, -1);
    gtk_widget_set_parent (info_box, GTK_WIDGET (self));
    self->info_box = info_box;
}
