#pragma once

#include <gtk/gtk.h>
#include "keyframe-tracks.h"

#include "../../model/keyframe-value-float.h"

struct _KeyframeTrackFrame
{
    KeyframeTrack parent_instance;

    KeyframeValueFloat *float_value;

    gboolean inhibit_drag;
    gboolean drag_active;
    float drag_start_x;
    float drag_current_x;
    GtkWidget *drag_widget;

    float start_position;

    // List of KeyframeTrackFramePoint widgets
    GSList *control_points;
};

// Layout
#define KEYFRAME_TYPE_TRACK_FRAME_LAYOUT (keyframe_track_frame_layout_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackFrameLayout, keyframe_track_frame_layout, KEYFRAME, TRACK_FRAME_LAYOUT, GtkLayoutManager)


// Control Point Widget
#define KEYFRAME_TYPE_TRACK_FRAME_POINT (keyframe_track_frame_point_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackFramePoint, keyframe_track_frame_point, KEYFRAME, TRACK_FRAME_POINT, GtkWidget)

GtkWidget *keyframe_track_frame_point_new (float timestamp, float value);
