/* keyframe-window.c
 *
 * Copyright 2021 Matthew Jakeman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keyframe-config.h"
#include "keyframe-window.h"
#include "keyframe-canvas.h"
#include "keyframe-timeline.h"
#include "keyframe-composition-manager.h"
#include "model/keyframe-layers.h"
#include "keyframe-export-dialog.h"

#include <libpanel.h>

struct _KeyframeWindow
{
    AdwApplicationWindow  parent_instance;
    KeyframeCompositionManager *manager;

    /* Template widgets */
    GtkHeaderBar        *header_bar;
    PanelGrid           *grid;
    PanelDock           *dock;
    KeyframeTimeline    *timeline;
    GtkButton           *render_btn;
};

G_DEFINE_TYPE (KeyframeWindow, keyframe_window, ADW_TYPE_APPLICATION_WINDOW)

static GtkWidget *
get_default_focus_cb (GtkWidget *widget,
                      GtkWidget *canvas)
{
  return canvas;
}

static gboolean
on_save_cb (PanelSaveDelegate *delegate,
            GTask             *task,
            PanelWidget       *widget)
{
  g_assert (PANEL_IS_SAVE_DELEGATE (delegate));
  g_assert (G_IS_TASK (task));
  g_assert (PANEL_IS_WIDGET (widget));

  // actually do the save here, ideally asynchronously

  g_print ("Actually save the file\n");

  panel_widget_set_modified (widget, FALSE);
  panel_save_delegate_set_progress (delegate, 1.0);
  g_task_return_boolean (task, TRUE);

  return TRUE;
}

static void
keyframe_window_add_document (KeyframeWindow *self)
{
    PanelWidget *widget;
    PanelSaveDelegate *save_delegate;

    g_return_if_fail (KEYFRAME_IS_WINDOW (self));

    KeyframeComposition *composition = keyframe_composition_manager_new_composition (self->manager);

    char *title = NULL;
    g_object_get (composition, "title", &title, NULL);

    GtkWidget *canvas = keyframe_canvas_new (composition);

    save_delegate = panel_save_delegate_new ();
    panel_save_delegate_set_title (save_delegate, title);
    panel_save_delegate_set_subtitle (save_delegate, "~/Documents");

    widget = g_object_new (PANEL_TYPE_WIDGET,
                         "title", title,
                         "kind", PANEL_WIDGET_KIND_DOCUMENT,
                         "icon-name", "text-x-generic-symbolic",
                         // "menu-model", self->page_menu,
                         "can-maximize", TRUE,
                         "save-delegate", save_delegate,
                         "modified", TRUE,
                         "child", g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                                                "child", canvas,
                                                NULL),
                         NULL);

    g_signal_connect (widget,
                    "get-default-focus",
                    G_CALLBACK (get_default_focus_cb),
                    canvas);
    g_signal_connect (save_delegate,
                    "save",
                    G_CALLBACK (on_save_cb),
                    widget);

    panel_grid_add (self->grid, widget);
    panel_widget_raise (widget);
    panel_widget_focus_default (widget);

    g_object_unref (save_delegate);
}

static void
add_document_action (GtkWidget  *widget,
                     const char *action_name,
                     GVariant   *param)
{
    keyframe_window_add_document (KEYFRAME_WINDOW (widget));
}

static PanelFrame *
create_frame_cb (PanelGrid     *grid,
                 KeyframeWindow *self)
{
    PanelFrame *frame;
    PanelFrameHeader *header;
    AdwStatusPage *status;
    GtkGrid *shortcuts;

    g_assert (KEYFRAME_IS_WINDOW (self));

    frame = PANEL_FRAME (panel_frame_new ());

    status = ADW_STATUS_PAGE (adw_status_page_new ());
    adw_status_page_set_title (status, "Create a New Composition");
    adw_status_page_set_icon_name (status, "cameras-symbolic");
    adw_status_page_set_description (status, "Use the page switcher above or use one of the following:");
    shortcuts = GTK_GRID (gtk_grid_new ());
    gtk_grid_set_row_spacing (shortcuts, 6);
    gtk_grid_set_column_spacing (shortcuts, 32);
    gtk_widget_set_halign (GTK_WIDGET (shortcuts), GTK_ALIGN_CENTER);
    gtk_grid_attach (shortcuts, gtk_label_new ("New Document"), 0, 0, 1, 1);
    gtk_grid_attach (shortcuts, gtk_label_new ("Ctrl+N"), 1, 0, 1, 1);
    gtk_grid_attach (shortcuts, gtk_label_new ("Close Document"), 0, 1, 1, 1);
    gtk_grid_attach (shortcuts, gtk_label_new ("Ctrl+W"), 1, 1, 1, 1);
    for (GtkWidget *child = gtk_widget_get_first_child (GTK_WIDGET (shortcuts));
       child;
       child = gtk_widget_get_next_sibling (child))
    gtk_widget_set_halign (child, GTK_ALIGN_START);
    adw_status_page_set_child (status, GTK_WIDGET (shortcuts));
    panel_frame_set_placeholder (frame, GTK_WIDGET (status));

    header = PANEL_FRAME_HEADER (panel_frame_header_bar_new ());

    panel_frame_set_header (frame, header);
    panel_frame_header_pack_start (header,
                                 -100,
                                 g_object_new (GTK_TYPE_BUTTON,
                                               "width-request", 32,
                                               "focus-on-click", FALSE,
                                               "icon-name", "go-previous-symbolic",
                                               NULL));
    panel_frame_header_pack_start (header,
                                 -50,
                                 g_object_new (GTK_TYPE_BUTTON,
                                               "width-request", 32,
                                               "focus-on-click", FALSE,
                                               "icon-name", "go-next-symbolic",
                                               NULL));
    return frame;
}

static void
keyframe_window_constructed (GObject *object)
{
    KeyframeWindow *self = (KeyframeWindow *)object;

    G_OBJECT_CLASS (keyframe_window_parent_class)->constructed (object);

    (void)panel_grid_column_get_row (panel_grid_get_column (self->grid, 0), 0);
}

#include "export/export-gstreamer.h"

static void
cb_export_dialog_response (GtkDialog           *self,
                           gint                 response_id,
                           KeyframeComposition *composition)
{
    KeyframeExportDialog *dlg = KEYFRAME_EXPORT_DIALOG (self);

    if (response_id == GTK_RESPONSE_OK)
    {
        g_print ("Beginning Export Job\n");
        export_composition_gstreamer (composition);
        g_print ("Ending Export Job\n");
    }

    gtk_window_destroy (GTK_WINDOW (self));
}

static void
cb_render_btn_click (GtkButton      *btn,
                     KeyframeWindow *self)
{
    KeyframeComposition *composition = keyframe_composition_manager_get_current (self->manager);

    if (composition == NULL)
        return;

    KeyframeExportDialog *dlg = keyframe_export_dialog_new (composition);
    gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (self));
    gtk_window_set_modal (GTK_WINDOW (dlg), true);
    gtk_widget_show (dlg);

    g_signal_connect (dlg, "response", G_CALLBACK (cb_export_dialog_response), composition);
}

static void
render_document_action (GtkWidget  *widget,
                        const char *action_name,
                        GVariant   *param)
{
    cb_render_btn_click (NULL, KEYFRAME_WINDOW (widget));
}

static void
keyframe_window_class_init (KeyframeWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->constructed = keyframe_window_constructed;


    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/mattjakeman/Keyframe/keyframe-window.ui");
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, header_bar);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, grid);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, dock);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, timeline);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, render_btn);
    gtk_widget_class_bind_template_callback (widget_class, create_frame_cb);

    gtk_widget_class_install_action (widget_class, "document.new", NULL, add_document_action);
    gtk_widget_class_install_action (widget_class, "document.render", NULL, render_document_action);

    gtk_widget_class_add_binding_action (widget_class, GDK_KEY_n, GDK_CONTROL_MASK, "document.new", NULL);
    gtk_widget_class_add_binding_action (widget_class, GDK_KEY_r, GDK_CONTROL_MASK, "document.render", NULL);
}

static void
keyframe_window_init (KeyframeWindow *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));

    self->manager = keyframe_composition_manager_new ();
    g_object_set (self->timeline, "manager", self->manager, NULL);

    g_signal_connect (self->render_btn, "clicked", cb_render_btn_click, self);
}
