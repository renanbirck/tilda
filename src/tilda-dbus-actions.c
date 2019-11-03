#include "tilda-dbus-actions.h"

#include "key_grabber.h"
#include "tilda-dbus.h"

#define TILDA_DBUS_NAME "com.github.lanoxx.tilda.Actions"
#define TILDA_DBUS_ACTIONS_PATH "/com/github/lanoxx/tilda/Actions"

static gboolean
on_handle_toggle (TildaDbusActions *skeleton,
                  GDBusMethodInvocation *invocation,
                  gpointer user_data)
{
    tilda_window *window;

    window = user_data;

    pull (window, PULL_TOGGLE, FALSE);

    tilda_dbus_actions_complete_toggle (skeleton, invocation);

    return GDK_EVENT_STOP;
}

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar *name,
                  gpointer window)
{
    TildaDbusActions *actions;
    GError *error;

    g_debug ("TildaDbusActions: Name acquired: %s", name);

    error = NULL;

    actions = tilda_dbus_actions_skeleton_new ();

    g_signal_connect (actions, "handle-toggle",G_CALLBACK (on_handle_toggle), window);

    g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (actions),
                                      connection,
                                      TILDA_DBUS_ACTIONS_PATH,
                                      &error);

    if (error)
    {
        g_error ("Error while exporting dbus interface path %s: %s",
                 TILDA_DBUS_ACTIONS_PATH, error->message);
    }
}

guint
tilda_dbus_actions_init (tilda_window *window)
{
    guint bus_identifier;

    bus_identifier = g_bus_own_name (G_BUS_TYPE_SESSION,
                                     TILDA_DBUS_NAME,
                                     G_BUS_NAME_OWNER_FLAGS_NONE,
                                     NULL,
                                     on_name_acquired,
                                     NULL, window, NULL);

    return bus_identifier;
}

void
tilda_dbus_actions_finish (guint bus_identifier)
{
    g_bus_unown_name (bus_identifier);
}
