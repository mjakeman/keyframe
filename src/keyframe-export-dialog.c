#include "keyframe-export-dialog.h"

#include <adwaita.h>

typedef struct
{
    KeyframeComposition *composition;
    KeyframeRenderer *renderer;
} KeyframeExportDialogPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeExportDialog, keyframe_export_dialog, GTK_TYPE_DIALOG)

enum {
    PROP_0,
    PROP_COMPOSITION,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_export_dialog_new:
 *
 * Create a new #KeyframeExportDialog.
 *
 * Returns: (transfer full): a newly created #KeyframeExportDialog
 */
GtkWidget *
keyframe_export_dialog_new (KeyframeComposition *composition)
{
    return g_object_new (KEYFRAME_TYPE_EXPORT_DIALOG,
                         "composition", composition,
                         "use-header-bar", 1,
                         NULL);
}

static void
keyframe_export_dialog_finalize (GObject *object)
{
    KeyframeExportDialog *self = (KeyframeExportDialog *)object;
    KeyframeExportDialogPrivate *priv = keyframe_export_dialog_get_instance_private (self);

    g_object_unref (priv->composition);

    G_OBJECT_CLASS (keyframe_export_dialog_parent_class)->finalize (object);
}

static void
keyframe_export_dialog_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
    KeyframeExportDialog *self = KEYFRAME_EXPORT_DIALOG (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_export_dialog_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
    KeyframeExportDialog *self = KEYFRAME_EXPORT_DIALOG (object);
    KeyframeExportDialogPrivate *priv = keyframe_export_dialog_get_instance_private (self);

    switch (prop_id)
    {
        case PROP_COMPOSITION:
            priv->composition = g_object_ref (g_value_get_object (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_export_dialog_class_init (KeyframeExportDialogClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_export_dialog_finalize;
    object_class->get_property = keyframe_export_dialog_get_property;
    object_class->set_property = keyframe_export_dialog_set_property;

    properties [PROP_COMPOSITION] =
        g_param_spec_object ("composition", "Composition", "Composition",
                             KEYFRAME_TYPE_COMPOSITION, G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_export_dialog_init (KeyframeExportDialog *self)
{
    gtk_dialog_add_buttons (GTK_DIALOG (self),
                            "Render", GTK_RESPONSE_OK,
                            "Close", GTK_RESPONSE_CANCEL,
                            NULL);

    gtk_dialog_set_default_response (GTK_DIALOG (self), GTK_RESPONSE_OK);
    gtk_window_set_title (GTK_WINDOW (self), "Export & Render");
    gtk_window_set_default_size (GTK_WINDOW (self), 450, 600);
    gtk_window_set_resizable (GTK_WINDOW (self), false);

    // Dialog Contents
    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (self));
    gtk_orientable_set_orientation (GTK_ORIENTABLE (content_area), GTK_ORIENTATION_VERTICAL);

    GtkWidget *scroll = gtk_scrolled_window_new ();
    gtk_widget_set_vexpand (scroll, true);
    gtk_scrolled_window_set_min_content_height (scroll, 400);
    gtk_box_append (GTK_BOX (content_area), scroll);

    AdwClamp *clamp = adw_clamp_new ();
    adw_clamp_set_maximum_size (clamp, 400);
    adw_clamp_set_tightening_threshold (clamp, 300);
    gtk_widget_set_margin_start (clamp, 12);
    gtk_widget_set_margin_end (clamp, 12);
    gtk_widget_set_margin_top (clamp, 32);
    gtk_widget_set_margin_bottom (clamp, 32);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scroll), clamp);

    GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    adw_clamp_set_child (clamp, box);

    GtkWidget *preview = gtk_image_new_from_resource ("/com/mattjakeman/Keyframe/icons/camera-web-symbolic.svg");
    // gtk_picture_set_paintable (preview, GdkPaintable *paintable);
    gtk_widget_set_size_request (preview, -1, 200);
    gtk_box_append (GTK_BOX (box), preview);

    GtkWidget *prefs = gtk_list_box_new ();
    gtk_list_box_set_selection_mode (GTK_LIST_BOX (prefs), GTK_SELECTION_NONE);
    gtk_widget_add_css_class (prefs, "content");
    gtk_box_append (GTK_BOX (box), prefs);

    AdwActionRow *action_row = adw_action_row_new ();
    adw_preferences_row_set_title (action_row, "Output Location");
    GtkWidget *action_row_widget = gtk_button_new_with_label ("File Location");
    gtk_widget_set_valign (action_row_widget, GTK_ALIGN_CENTER);
    adw_action_row_add_suffix (action_row, action_row_widget);
    gtk_list_box_append (prefs, action_row);

    AdwActionRow *action_row2 = adw_action_row_new ();
    adw_preferences_row_set_title (action_row2, "Format");
    GtkWidget *action_row_widget2 = gtk_drop_down_new_from_strings (NULL);
    gtk_widget_set_valign (action_row_widget2, GTK_ALIGN_CENTER);
    adw_action_row_add_suffix (action_row2, action_row_widget2);
    gtk_list_box_append (prefs, action_row2);

    /*GtkWidget *btn = gtk_button_new_with_label ("Render");
    g_signal_connect (btn, "clicked", cb_render, self);
    gtk_box_append (GTK_BOX (box), btn);*/
}
