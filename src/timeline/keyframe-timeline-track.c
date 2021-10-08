#include "keyframe-timeline-track.h"

G_DEFINE_TYPE (KeyframeTimelineTrack, keyframe_timeline_track, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_timeline_track_new:
 *
 * Create a new #KeyframeTimelineTrack.
 *
 * Returns: (transfer full): a newly created #GtkWidget
 */
GtkWidget *
keyframe_timeline_track_new (void)
{
    return g_object_new (KEYFRAME_TYPE_TIMELINE_TRACK, NULL);
}

static void
keyframe_timeline_track_finalize (GObject *object)
{
    KeyframeTimelineTrack *self = (KeyframeTimelineTrack *)object;

    G_OBJECT_CLASS (keyframe_timeline_track_parent_class)->finalize (object);
}

static void
keyframe_timeline_track_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
    KeyframeTimelineTrack *self = KEYFRAME_TIMELINE_TRACK (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_track_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    KeyframeTimelineTrack *self = KEYFRAME_TIMELINE_TRACK (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_track_class_init (KeyframeTimelineTrackClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_track_finalize;
    object_class->get_property = keyframe_timeline_track_get_property;
    object_class->set_property = keyframe_timeline_track_set_property;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    gtk_widget_class_set_css_name (widget_class, "track");
}

static void
keyframe_timeline_track_init (KeyframeTimelineTrack *self)
{
}
