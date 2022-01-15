#include "keyframe-track-clip-private.h"

struct _KeyframeTrackClipLayout
{
    GtkLayoutManager parent_instance;
};

G_DEFINE_FINAL_TYPE (KeyframeTrackClipLayout, keyframe_track_clip_layout, GTK_TYPE_LAYOUT_MANAGER)

static void
keyframe_track_clip_layout_measure (GtkLayoutManager *layout_manager,
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
keyframe_track_clip_layout_allocate (GtkLayoutManager *layout_manager,
                                     GtkWidget        *widget,
                                     int               width,
                                     int               height,
                                     int               baseline)
{
    KeyframeTrackClip *clip_track = KEYFRAME_TRACK_CLIP (widget);

    if (!gtk_widget_should_layout (clip_track->clip))
        return;

    float clip_width = clip_track->clip_timestamp_end - clip_track->clip_timestamp_start;

    if (clip_track->drag_active)
    {
        gtk_widget_size_allocate (clip_track->clip,
                              &(const GtkAllocation){ clip_track->drag_current_x - clip_track->start_position, 0, clip_width, height },
                              -1);
    }
    else
    {
        gtk_widget_size_allocate (clip_track->clip,
                              &(const GtkAllocation){ clip_track->clip_timestamp_start - clip_track->start_position, 0, clip_width, height },
                              -1);
    }
}

static GtkSizeRequestMode
keyframe_track_clip_layout_get_request_mode (GtkLayoutManager *layout_manager,
                                                      GtkWidget        *widget)
{
    return GTK_SIZE_REQUEST_CONSTANT_SIZE;
}

static void
keyframe_track_clip_layout_class_init (KeyframeTrackClipLayoutClass *klass)
{
    GtkLayoutManagerClass *layout_manager_class = GTK_LAYOUT_MANAGER_CLASS (klass);

    layout_manager_class->measure = keyframe_track_clip_layout_measure;
    layout_manager_class->allocate = keyframe_track_clip_layout_allocate;
    layout_manager_class->get_request_mode = keyframe_track_clip_layout_get_request_mode;
}

static void
keyframe_track_clip_layout_init (KeyframeTrackClipLayout *self)
{

}
