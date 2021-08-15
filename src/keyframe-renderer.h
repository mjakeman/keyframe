#pragma once

#include <glib-2.0/glib-object.h>
#include <cairo/cairo.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_RENDERER (keyframe_renderer_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeRenderer, keyframe_renderer, KEYFRAME, RENDERER, GObject)

struct _KeyframeRendererClass
{
  GObjectClass parent_class;
};

KeyframeRenderer *keyframe_renderer_new (void);
cairo_t *keyframe_renderer_get_cairo (KeyframeRenderer *self);

void
keyframe_renderer_begin_frame (KeyframeRenderer *self, int width, int height);

cairo_surface_t *
keyframe_renderer_end_frame (KeyframeRenderer *self);

// TODO: Temporary
int keyframe_renderer_get_height (KeyframeRenderer *self);
int keyframe_renderer_get_width (KeyframeRenderer *self);

G_END_DECLS
