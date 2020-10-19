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

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
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
