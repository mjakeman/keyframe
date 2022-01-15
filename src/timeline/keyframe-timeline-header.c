#include "keyframe-timeline-header.h"

struct _KeyframeTimelineHeader
{
    GtkWidget parent_instance;
};

G_DEFINE_FINAL_TYPE (KeyframeTimelineHeader, keyframe_timeline_header, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

GtkWidget *
keyframe_timeline_header_new (void)
{
    return g_object_new (KEYFRAME_TYPE_TIMELINE_HEADER, NULL);
}

static void
keyframe_timeline_header_finalize (GObject *object)
{
    KeyframeTimelineHeader *self = (KeyframeTimelineHeader *)object;

    G_OBJECT_CLASS (keyframe_timeline_header_parent_class)->finalize (object);
}

static void
keyframe_timeline_header_get_property (GObject    *object,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
    KeyframeTimelineHeader *self = KEYFRAME_TIMELINE_HEADER (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_header_set_property (GObject      *object,
                                       guint         prop_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
    KeyframeTimelineHeader *self = KEYFRAME_TIMELINE_HEADER (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_header_class_init (KeyframeTimelineHeaderClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_header_finalize;
    object_class->get_property = keyframe_timeline_header_get_property;
    object_class->set_property = keyframe_timeline_header_set_property;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_css_name (widget_class, "header");
}

static void
keyframe_timeline_header_init (KeyframeTimelineHeader *self)
{
    GtkLayoutManager *layout = gtk_box_layout_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_layout_manager (GTK_WIDGET (self), layout);

    GtkWidget *label = gtk_label_new ("Layer Info");
    gtk_widget_set_size_request (label, 400, -1);
    gtk_widget_set_parent (label, GTK_WIDGET (self));

    GtkWidget *scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0, 100, 0.1);
    gtk_widget_set_hexpand (scale, TRUE);
    gtk_widget_set_parent (scale, GTK_WIDGET (self));
}
