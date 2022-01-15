#include "keyframe-timeline-menu.h"

#include "../model/keyframe-layers.h"

typedef struct
{
    int _padding;
} KeyframeTimelineMenuPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeTimelineMenu, keyframe_timeline_menu, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_timeline_menu_new:
 *
 * Create a new #KeyframeTimelineMenu.
 *
 * Returns: (transfer full): a newly created #KeyframeTimelineMenu
 */
GtkWidget *
keyframe_timeline_menu_new (void)
{
    return GTK_WIDGET (g_object_new (KEYFRAME_TYPE_TIMELINE_MENU, NULL));
}

static void
keyframe_timeline_menu_finalize (GObject *object)
{
    KeyframeTimelineMenu *self = (KeyframeTimelineMenu *)object;
    KeyframeTimelineMenuPrivate *priv = keyframe_timeline_menu_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_timeline_menu_parent_class)->finalize (object);
}

static void
keyframe_timeline_menu_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
    KeyframeTimelineMenu *self = KEYFRAME_TIMELINE_MENU (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_menu_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
    KeyframeTimelineMenu *self = KEYFRAME_TIMELINE_MENU (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
cb_new_layer (GtkButton *btn, GType typeid)
{
    GVariant *type = g_variant_new_uint64 (typeid);
    gtk_widget_activate_action_variant (GTK_WIDGET (btn), "layer.new", type);
}

static void
cb_new_layer_popover (GtkMenuButton *new_layer_btn, KeyframeTimelineMenu *self)
{
    GtkWidget *popover_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *layer_btn = gtk_button_new_with_label ("Text");
    g_signal_connect (layer_btn, "clicked", cb_new_layer, KEYFRAME_TYPE_LAYER_TEXT);
    gtk_box_append (GTK_BOX (popover_box), layer_btn);

    layer_btn = gtk_button_new_with_label ("Geometry");
    g_signal_connect (layer_btn, "clicked", cb_new_layer, KEYFRAME_TYPE_LAYER_GEOMETRY);
    gtk_box_append (GTK_BOX (popover_box), layer_btn);

    layer_btn = gtk_button_new_with_label ("Cool");
    g_signal_connect (layer_btn, "clicked", cb_new_layer, KEYFRAME_TYPE_LAYER_COOL);
    gtk_box_append (GTK_BOX (popover_box), layer_btn);

    GtkWidget *popover = gtk_popover_new ();
    gtk_popover_set_child (popover, popover_box);

    gtk_menu_button_set_popover (new_layer_btn, popover);
}

static void
cb_delete_layer (GtkButton *btn, KeyframeTimelineMenu *self)
{
    gtk_widget_activate_action (self, "layer.delete", NULL);
}

static void
cb_edit_layer (GtkButton *btn, KeyframeTimelineMenu *self)
{
    gtk_widget_activate_action (GTK_WIDGET (self), "layer.edit", NULL);
}

static void
keyframe_timeline_menu_class_init (KeyframeTimelineMenuClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_menu_finalize;
    object_class->get_property = keyframe_timeline_menu_get_property;
    object_class->set_property = keyframe_timeline_menu_set_property;
}

static void
keyframe_timeline_menu_init (KeyframeTimelineMenu *self)
{
    GtkWidget *toolbar = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class (toolbar, "toolbar");

    GtkLayoutManager *layout = gtk_bin_layout_new ();
    gtk_widget_set_layout_manager (GTK_WIDGET (self), layout);
    gtk_widget_set_parent (toolbar, GTK_WIDGET (self));

    GtkWidget *add_layer_btn = gtk_menu_button_new ();
    gtk_menu_button_set_icon_name (GTK_MENU_BUTTON (add_layer_btn), "document-new-symbolic");
    gtk_menu_button_set_direction (GTK_MENU_BUTTON (add_layer_btn), GTK_ARROW_UP);
    gtk_menu_button_set_create_popup_func (GTK_MENU_BUTTON (add_layer_btn),
                                           (GtkMenuButtonCreatePopupFunc)cb_new_layer_popover,
                                           g_object_ref (self),
                                           g_object_unref);
    gtk_box_append (GTK_BOX (toolbar), add_layer_btn);

    GtkWidget *delete_layer_btn = gtk_button_new ();
    gtk_button_set_child (GTK_BUTTON (delete_layer_btn), gtk_image_new_from_icon_name ("user-trash-symbolic"));
    g_signal_connect (delete_layer_btn, "clicked", G_CALLBACK (cb_delete_layer), self);
    gtk_widget_add_css_class (delete_layer_btn, "flat");
    gtk_box_append (GTK_BOX (toolbar), delete_layer_btn);

    GtkWidget *edit_layer_btn = gtk_button_new ();
    gtk_button_set_child (GTK_BUTTON (edit_layer_btn), gtk_image_new_from_icon_name ("document-edit-symbolic"));
    g_signal_connect (edit_layer_btn, "clicked", G_CALLBACK (cb_edit_layer), self);
    gtk_widget_add_css_class (edit_layer_btn, "flat");
    gtk_box_append (GTK_BOX (toolbar), edit_layer_btn);
}
