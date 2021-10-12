#include "keyframe-timeline-track.h"

typedef struct
{
    GtkAdjustment *adjustment;
    ulong adjustment_signal;
} KeyframeTimelineTrackPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeTimelineTrack, keyframe_timeline_track, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_ADJUSTMENT,
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
    KeyframeTimelineTrackPrivate *priv = keyframe_timeline_track_get_instance_private (self);

    if (priv->adjustment)
    {
        g_signal_handler_disconnect (priv->adjustment, priv->adjustment_signal);
        g_clear_pointer (&priv->adjustment, g_object_unref);
    }

    G_OBJECT_CLASS (keyframe_timeline_track_parent_class)->finalize (object);
}

static void
keyframe_timeline_track_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
    KeyframeTimelineTrack *self = KEYFRAME_TIMELINE_TRACK (object);
    KeyframeTimelineTrackPrivate *priv = keyframe_timeline_track_get_instance_private (self);

    switch (prop_id)
    {
        case PROP_ADJUSTMENT:
            g_value_set_object (value, priv->adjustment);
            break;
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
    KeyframeTimelineTrackPrivate *priv = keyframe_timeline_track_get_instance_private (self);

    switch (prop_id)
    {
        case PROP_ADJUSTMENT:
            priv->adjustment = g_object_ref (g_value_get_object (value));

            gpointer cb_value_changed = KEYFRAME_TIMELINE_TRACK_GET_CLASS (self)->adjustment_changed;
            priv->adjustment_signal = g_signal_connect_swapped (priv->adjustment, "value-changed", cb_value_changed, self);

            // Initial Update
            KEYFRAME_TIMELINE_TRACK_GET_CLASS (self)->adjustment_changed (self, priv->adjustment);

            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_timeline_track_adjustment_changed (KeyframeTimelineTrack *self,
                                            GtkAdjustment         *adj)
{
    g_critical ("%s does not implement virtual method 'adjustment_changed()'.",
                g_type_name_from_instance ((GTypeInstance *)self));
}

static void
keyframe_timeline_track_class_init (KeyframeTimelineTrackClass *klass)
{
    klass->adjustment_changed = keyframe_timeline_track_adjustment_changed;

    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_track_finalize;
    object_class->get_property = keyframe_timeline_track_get_property;
    object_class->set_property = keyframe_timeline_track_set_property;

    properties [PROP_ADJUSTMENT]
        = g_param_spec_object ("adjustment",
                              "adjustment",
                              "adjustment",
                              GTK_TYPE_ADJUSTMENT,
                              G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_css_name (widget_class, "track");
}

static void
keyframe_timeline_track_init (KeyframeTimelineTrack *self)
{
    gtk_widget_set_overflow (GTK_WIDGET (self), GTK_OVERFLOW_HIDDEN);
}
