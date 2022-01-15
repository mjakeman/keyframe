#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE_COLUMN_VIEW (keyframe_timeline_column_view_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeTimelineColumnView, keyframe_timeline_column_view, KEYFRAME, TIMELINE_COLUMN_VIEW, GtkWidget)

struct _KeyframeTimelineColumnViewClass
{
  GtkWidgetClass parent_class;
};

GtkWidget *keyframe_timeline_column_view_new (void);

void
keyframe_timeline_column_view_set_model (KeyframeTimelineColumnView *self,
                                         GtkSelectionModel *model);

GtkSelectionModel *
keyframe_timeline_column_view_get_model (KeyframeTimelineColumnView *self);

G_END_DECLS
