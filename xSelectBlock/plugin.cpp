#include "plugin.h"
#include "resource.h"
#include "SelectBlockDialog.h"

enum
{
    MENU,
    DUMP,
};

HINSTANCE instance;

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case MENU:
        ShowSelectBlockDialog(instance, 0, 1);
        break;

    case DUMP:
    {
        uint64_t start = Script::Gui::Dump::SelectionGetStart();
        uint64_t end = Script::Gui::Dump::SelectionGetEnd();
        ShowSelectBlockDialog(instance, start, end);
    }
        break;

    default:
        break;
    }
}

static bool cbXrange(int argc, char* argv[])
{
    if (argc < 3)
        return false;

    auto start = DbgValFromString(argv[1]);
    auto stop = DbgValFromString(argv[2]);
    return Script::Gui::Dump::SelectionSet(start, stop);
}

static bool cbXset(int argc, char* argv[])
{
    if (argc < 2)
        return false;

    auto start = Script::Gui::Dump::SelectionGetStart();
    auto len = DbgValFromString(argv[1]);

    return Script::Gui::Dump::SelectionSet(start, start + len - 1);
}

static bool cbXlen(int argc, char* argv[])
{
    if (argc < 3)
        return false;

    auto start = DbgValFromString(argv[1]);
    auto len = DbgValFromString(argv[2]);

    return Script::Gui::Dump::SelectionSet(start, start + len - 1);
}

static bool cbXext(int argc, char* argv[])
{
    if (argc < 2)
        return false;

    auto start = Script::Gui::Dump::SelectionGetStart();
    auto currentEnd = Script::Gui::Dump::SelectionGetEnd();
    auto len = DbgValFromString(argv[1]);

    return Script::Gui::Dump::SelectionSet(start, currentEnd + len);
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    if (!_plugin_registercommand(pluginHandle, "xrange", cbXrange, false))
        _plugin_logputs("[" PLUGIN_NAME "] Error registering the 'xrange' command!");
    if (!_plugin_registercommand(pluginHandle, "xset", cbXset, false))
        _plugin_logputs("[" PLUGIN_NAME "] Error registering the 'xset' command!");
    if (!_plugin_registercommand(pluginHandle, "xlen", cbXlen, false))
        _plugin_logputs("[" PLUGIN_NAME "] Error registering the 'xlen' command!");
    if (!_plugin_registercommand(pluginHandle, "xext", cbXext, false))
        _plugin_logputs("[" PLUGIN_NAME "] Error registering the 'xext' command!");

    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
    HideSelectBlockDialog();
}

//Do GUI/Menu related things here.
void pluginSetup()
{
    _plugin_menuaddentry(hMenu, MENU, "xSelectBlock");
    _plugin_menuaddentry(hMenuDump, DUMP, "&Select block");
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
        instance = hinstDLL;

    return TRUE;
}
