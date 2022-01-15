#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE_MENU (keyframe_timeline_menu_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeTimelineMenu, keyframe_timeline_menu, KEYFRAME, TIMELINE_MENU, GtkWidget)

struct _KeyframeTimelineMenuClass
{
  GtkWidgetClass parent_class;
};

GtkWidget *keyframe_timeline_menu_new (void);

G_END_DECLS
