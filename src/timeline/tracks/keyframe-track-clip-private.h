#pragma once

#include <gtk/gtk.h>

#include "keyframe-tracks.h"

G_BEGIN_DECLS

// TODO: Make Private?
struct _KeyframeTrackClip
{
    KeyframeTrack parent_instance;

    GtkWidget *clip;
    gboolean drag_active;
    float drag_start_x;
    float drag_current_x;

    // TODO: Make properties?
    float clip_timestamp_start;
    float clip_timestamp_end;

    float start_position;
};


// Layout Manager

#define KEYFRAME_TYPE_TRACK_CLIP_LAYOUT (keyframe_track_clip_layout_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackClipLayout, keyframe_track_clip_layout, KEYFRAME, TRACK_CLIP_LAYOUT, GtkLayoutManager)


// Clip Widget

#define KEYFRAME_TYPE_TRACK_CLIP_WIDGET (keyframe_track_clip_widget_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackClipWidget, keyframe_track_clip_widget, KEYFRAME, TRACK_CLIP_WIDGET, GtkWidget)

GtkWidget *
keyframe_track_clip_widget_new ();

void
keyframe_track_clip_widget_set_active_cursor (KeyframeTrackClipWidget *self, gboolean pressed);

G_END_DECLS
