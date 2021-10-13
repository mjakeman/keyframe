#include "keyframe-track-frame-private.h"

struct _KeyframeTrackFrameLayout
{
    GtkLayoutManager parent_instance;
};

G_DEFINE_FINAL_TYPE (KeyframeTrackFrameLayout, keyframe_track_frame_layout, GTK_TYPE_LAYOUT_MANAGER)

static void
keyframe_track_frame_layout_measure (GtkLayoutManager *layout_manager,
                                     GtkWidget        *widget,
                                     GtkOrientation    orientation,
                                     int               for_size,
                                     int              *minimum,
                                     int              *natural,
                                     int              *minimum_baseline,
                                     int              *natural_baseline)
{
    *minimum = 0;
    *natural = 0;
}

static void
keyframe_track_frame_layout_allocate (GtkLayoutManager *layout_manager,
                                      GtkWidget        *widget,
                                      int               width,
                                      int               height,
                                      int               baseline)
{
    KeyframeTrackFrame *frame_track = KEYFRAME_TRACK_FRAME (widget);

    for (GtkWidget *child = gtk_widget_get_first_child (widget);
         child != NULL;
         child = gtk_widget_get_next_sibling (child))
    {
        if (!gtk_widget_should_layout (child))
            continue;

        KeyframeTrackFramePoint *point = KEYFRAME_TRACK_FRAME_POINT (child);

        float timestamp;
        if (frame_track->drag_active && (child == frame_track->drag_widget))
            timestamp = frame_track->drag_current_x;
        else
            g_object_get (point, "timestamp", &timestamp, NULL);

        float x = timestamp - frame_track->start_position - (height/2);

        gtk_widget_size_allocate (child, &(const GtkAllocation){x, 0, height, height}, -1);
    }
}

static GtkSizeRequestMode
keyframe_track_frame_layout_get_request_mode (GtkLayoutManager *layout_manager,
                                              GtkWidget        *widget)
{
    return GTK_SIZE_REQUEST_CONSTANT_SIZE;
}

static void
keyframe_track_frame_layout_class_init (KeyframeTrackFrameLayoutClass *klass)
{
    GtkLayoutManagerClass *layout_manager_class = GTK_LAYOUT_MANAGER_CLASS (klass);

    layout_manager_class->measure = keyframe_track_frame_layout_measure;
    layout_manager_class->allocate = keyframe_track_frame_layout_allocate;
    layout_manager_class->get_request_mode = keyframe_track_frame_layout_get_request_mode;
}

static void
keyframe_track_frame_layout_init (KeyframeTrackFrameLayout *self)
{

}
