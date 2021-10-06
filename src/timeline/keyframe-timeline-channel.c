#include "keyframe-timeline-channel.h"

struct _KeyframeTimelineChannel
{
    GtkBox parent_instance;
};

G_DEFINE_FINAL_TYPE (KeyframeTimelineChannel, keyframe_timeline_channel, GTK_TYPE_BOX)

enum {
    PROP_0,
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
}

static void
keyframe_timeline_channel_init (KeyframeTimelineChannel *self)
{
}
