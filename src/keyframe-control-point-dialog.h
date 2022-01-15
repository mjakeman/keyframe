#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_CONTROL_POINT_DIALOG (keyframe_control_point_dialog_get_type())

G_DECLARE_FINAL_TYPE (KeyframeControlPointDialog, keyframe_control_point_dialog, KEYFRAME, CONTROL_POINT_DIALOG, GtkDialog)

GtkWidget *keyframe_control_point_dialog_new (float timestamp, float value);

G_END_DECLS
