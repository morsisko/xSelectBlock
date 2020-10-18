#include "plugin.h"
#include "resource.h"

enum
{
    MENU,
    DUMP,
};

HWND dialog = NULL;
HINSTANCE g_LocalDllHandle;

INT_PTR CALLBACK DialogProc(
    HWND Arg1,
    UINT Arg2,
    WPARAM Arg3,
    LPARAM Arg4
)
{
    if (Arg2 == WM_CLOSE)
    {
        DestroyWindow(dialog);
        dialog = NULL;
        return true;
    }

    return false;
}

void ShowDialog()
{
    if (!dialog)
        dialog = CreateDialog(g_LocalDllHandle, MAKEINTRESOURCE(IDD_SelectBlock), GuiGetWindowHandle(), DialogProc);

    ShowWindow(dialog, SW_SHOW);
}

static void Adler32Menu(int hWindow)
{
    if(!DbgIsDebugging())
    {
        dputs("You need to be debugging to use this command");
        return;
    }
    SELECTIONDATA sel;
    GuiSelectionGet(hWindow, &sel);
    duint len = sel.end - sel.start + 1;
    unsigned char* data = new unsigned char[len];
    if(DbgMemRead(sel.start, data, len))
    {
        DWORD a = 1, b = 0;
        for(duint index = 0; index < len; ++index)
        {
            a = (a + data[index]) % 65521;
            b = (b + a) % 65521;
        }
        delete[] data;
        DWORD checksum = (b << 16) | a;
        dprintf("Adler32 of %p[%X] is: %08X\n", sel.start, len, checksum);
    }
    else
        dputs("DbgMemRead failed...");
}

static bool cbTestCommand(int argc, char* argv[])
{
    dputs("Test command!");
    char line[GUI_MAX_LINE_SIZE] = "";
    if(!GuiGetLineWindow("test", line))
        dputs("Cancel pressed!");
    else
        dprintf("Line: \"%s\"\n", line);
    return true;
}

static duint exprZero(int argc, duint* argv, void* userdata)
{
    return 0;
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case MENU:
        ShowDialog();
        break;

    case DUMP:
        Adler32Menu(GUI_DUMP);
        break;

    default:
        break;
    }
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    _plugin_registercommand(pluginHandle, PLUGIN_NAME, cbTestCommand, false);
    _plugin_registerexprfunction(pluginHandle, PLUGIN_NAME ".zero", 0, exprZero, nullptr);
    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
    if (dialog)
    {
        SendMessage(dialog, WM_CLOSE, 0, 0);
    }
}

//Do GUI/Menu related things here.
void pluginSetup()
{
    _plugin_menuaddentry(hMenu, MENU, "xSelectBlock");
    _plugin_menuaddentry(hMenuDump, DUMP, "&Select block");
    _plugin_menuadd(hMenuDump, "&Test menu");
    _plugin_menuadd(hMenu, "&Test menu");
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
        g_LocalDllHandle = hinstDLL;

    return TRUE;
}
