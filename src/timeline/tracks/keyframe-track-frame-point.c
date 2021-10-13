#include "keyframe-track-frame-private.h"

struct _KeyframeTrackFramePoint
{
    GtkWidget parent_instance;

    float value;
    float timestamp;
};

G_DEFINE_FINAL_TYPE (KeyframeTrackFramePoint, keyframe_track_frame_point, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_TIMESTAMP,
    PROP_VALUE,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

GtkWidget *
keyframe_track_frame_point_new (float timestamp, float value)
{
    return g_object_new (KEYFRAME_TYPE_TRACK_FRAME_POINT,
                         "timestamp", timestamp,
                         "value", value,
                         NULL);
}

static void
keyframe_track_frame_point_finalize (GObject *object)
{
    KeyframeTrackFramePoint *self = (KeyframeTrackFramePoint *)object;

    G_OBJECT_CLASS (keyframe_track_frame_point_parent_class)->finalize (object);
}

static void
keyframe_track_frame_point_get_property (GObject    *object,
                                         guint       prop_id,
                                         GValue     *value,
                                         GParamSpec *pspec)
{
    KeyframeTrackFramePoint *self = KEYFRAME_TRACK_FRAME_POINT (object);

    switch (prop_id)
    {
    case PROP_TIMESTAMP:
        g_value_set_float (value, self->timestamp);
        break;
    case PROP_VALUE:
        g_value_set_float (value, self->value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_track_frame_point_set_property (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
    KeyframeTrackFramePoint *self = KEYFRAME_TRACK_FRAME_POINT (object);

    switch (prop_id)
    {
    case PROP_TIMESTAMP:
        self->timestamp = g_value_get_float (value);
        break;
    case PROP_VALUE:
        self->value = g_value_get_float (value);
        char fmt[100];
        g_snprintf(fmt, 100, "Value: %f", self->value);
        gtk_widget_set_tooltip_text (GTK_WIDGET (self), fmt);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_track_frame_point_class_init (KeyframeTrackFramePointClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_track_frame_point_finalize;
    object_class->get_property = keyframe_track_frame_point_get_property;
    object_class->set_property = keyframe_track_frame_point_set_property;

    properties [PROP_TIMESTAMP]
        = g_param_spec_float ("timestamp",
                              "Timestamp",
                              "Timestamp",
                              0, G_MAXFLOAT, 0,
                              G_PARAM_READWRITE);

    properties [PROP_VALUE]
        = g_param_spec_float ("value",
                              "Value",
                              "Value",
                              -(G_MAXFLOAT/2), (G_MAXFLOAT/2)-1, 0,
                              G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_css_name (widget_class, "control-point");
}

static void
keyframe_track_frame_point_init (KeyframeTrackFramePoint *self)
{
}
