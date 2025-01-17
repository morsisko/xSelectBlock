#include "SelectBlockDialog.h"
#include "resource.h"
#include "pluginmain.h"

HWND dialog = NULL;

enum NUM_BASE
{
    HEX = 0,
    DEC,
    OCT
};

NUM_BASE currentBase = NUM_BASE::HEX;

int getMaxTextLenByBase(NUM_BASE num)
{
    if (num == NUM_BASE::OCT)
        return 22;

    else if (num == NUM_BASE::DEC)
        return 20;

    return 16;
}

int getBase(NUM_BASE num)
{
    if (num == NUM_BASE::OCT)
        return 8;

    else if (num == NUM_BASE::DEC)
        return 10;

    return 16;
}

NUM_BASE getCurrentBase(HWND hwndDialog)
{
    if (IsDlgButtonChecked(hwndDialog, IDC_HEX_RADIO))
        return NUM_BASE::HEX;

    else if (IsDlgButtonChecked(hwndDialog, IDC_OCT_RADIO))
        return NUM_BASE::OCT;

    return NUM_BASE::DEC;
}

void limitInputControlsLength(HWND hwndDialog, int maxLen)
{
    SendMessageA(GetDlgItem(hwndDialog, IDC_LENGTH_EDIT), EM_LIMITTEXT, maxLen, NULL);
    SendMessageA(GetDlgItem(hwndDialog, IDC_START_EDIT), EM_LIMITTEXT, maxLen, NULL);
    SendMessageA(GetDlgItem(hwndDialog, IDC_END_EDIT), EM_LIMITTEXT, maxLen, NULL);
}

void convertBaseInputs(HWND hwndDialog, NUM_BASE to)
{
    char startBuffer[64] = { 0 };
    char endBuffer[64] = { 0 };
    char lenBuffer[64] = { 0 };

    GetWindowTextA(GetDlgItem(hwndDialog, IDC_START_EDIT), startBuffer, sizeof(startBuffer));
    GetWindowTextA(GetDlgItem(hwndDialog, IDC_END_EDIT), endBuffer, sizeof(endBuffer));
    GetWindowTextA(GetDlgItem(hwndDialog, IDC_LENGTH_EDIT), lenBuffer, sizeof(lenBuffer));

    int currentBaseVal = getBase(currentBase);

    uint64_t start = strtoull(startBuffer, NULL, currentBaseVal);
    uint64_t end = strtoull(endBuffer, NULL, currentBaseVal);
    uint64_t len = strtoull(lenBuffer, NULL, currentBaseVal);

    int newBaseVal = getBase(to);

    _ui64toa_s(start, startBuffer, sizeof(startBuffer), newBaseVal);
    _ui64toa_s(end, endBuffer, sizeof(endBuffer), newBaseVal);
    _ui64toa_s(len, lenBuffer, sizeof(lenBuffer), newBaseVal);

    limitInputControlsLength(hwndDialog, getMaxTextLenByBase(to));

    currentBase = to;

    SetWindowTextA(GetDlgItem(hwndDialog, IDC_START_EDIT), startBuffer);
    SetWindowTextA(GetDlgItem(hwndDialog, IDC_END_EDIT), endBuffer);
    SetWindowTextA(GetDlgItem(hwndDialog, IDC_LENGTH_EDIT), lenBuffer);
}

bool isodigit(char val)
{
    return val >= '0' && val < '8';
}

bool checkText(char* input, int len)
{
    char temp[256] = { 0 };
    int p = 0;
    bool wasChanged = false;

    for (int i = 0; i < len; i++)
    {
        char current = input[i];
        if ((currentBase == NUM_BASE::HEX && isxdigit(current)) ||
            (currentBase == NUM_BASE::DEC && isdigit(current)) ||
            (currentBase == NUM_BASE::OCT && isodigit(current)))
            temp[p++] = current;

        else
            wasChanged = true;
    }

    if (wasChanged)
        strcpy_s(input, 256, temp);

    return wasChanged;
}

void setupDialogValues(HWND hwndDialog, uint64_t start, uint64_t end)
{
    uint64_t len = end - start + 1;
    int base = getBase(getCurrentBase(hwndDialog));

    char startBuffer[64] = { 0 };
    char endBuffer[64] = { 0 };
    char lenBuffer[64] = { 0 };

    _ui64toa_s(start, startBuffer, sizeof(startBuffer), base);
    _ui64toa_s(end, endBuffer, sizeof(endBuffer), base);
    _ui64toa_s(len, lenBuffer, sizeof(lenBuffer), base);

    SetWindowTextA(GetDlgItem(hwndDialog, IDC_START_EDIT), startBuffer);
    SetWindowTextA(GetDlgItem(hwndDialog, IDC_END_EDIT), endBuffer);
    SetWindowTextA(GetDlgItem(hwndDialog, IDC_LENGTH_EDIT), lenBuffer);
    SendMessageA(GetDlgItem(hwndDialog, IDC_LENGTH_EDIT), EM_SETSEL, 0, -1);
}

void checkAndFixInputText(HWND inputControl)
{
    char content[256] = { 0 };
    DWORD firstChar, lastChar;
    SendMessage(inputControl, EM_GETSEL, (WPARAM)&firstChar, (LPARAM)&lastChar);

    GetWindowTextA(inputControl, content, 256);
    int currentTextLength = strlen(content);

    if (checkText(content, strlen(content)))
    {
        SetWindowTextA(inputControl, content);

        if (currentTextLength != firstChar) {
            firstChar--;
            lastChar--;
        }

        SendMessage(inputControl, EM_SETSEL, (WPARAM)firstChar, (LPARAM)lastChar);
    }
}

void centerWindow(HWND window)
{
    RECT parentRect;
    RECT windowRect;

    GetWindowRect(GuiGetWindowHandle(), &parentRect);
    GetWindowRect(window, &windowRect);

    int x = parentRect.left + (parentRect.right - parentRect.left) / 2 - (windowRect.right - windowRect.left) / 2;
    int y = parentRect.top + (parentRect.bottom - parentRect.top) / 2 - (windowRect.bottom - windowRect.top) / 2;

    SetWindowPos(window, NULL, x, y, NULL, NULL, SWP_NOSIZE);
}

uint64_t getStartAddress(HWND hwndDialog)
{
    char startBuffer[64] = { 0 };
    int base = getBase(getCurrentBase(hwndDialog));

    GetWindowTextA(GetDlgItem(dialog, IDC_START_EDIT), startBuffer, sizeof(startBuffer));

    return strtoull(startBuffer, NULL, base);
}

uint64_t getStopAddress(HWND hwndDialog)
{
    char buffer[64] = { 0 };
    int base = getBase(getCurrentBase(hwndDialog));

    if (IsDlgButtonChecked(dialog, IDC_END_RADIO))
    {
        GetWindowTextA(GetDlgItem(dialog, IDC_END_EDIT), buffer, sizeof(buffer));
        return strtoull(buffer, NULL, base);
    }

    GetWindowTextA(GetDlgItem(dialog, IDC_LENGTH_EDIT), buffer, sizeof(buffer));
    return getStartAddress(hwndDialog) + strtoull(buffer, NULL, base) - 1;
}

void HideSelectBlockDialog()
{
    if (dialog)
        SendMessage(dialog, WM_CLOSE, 0, 0);

    dialog = NULL;
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        dialog = hwndDlg;
        SendMessageA(GetDlgItem(hwndDlg, IDC_LEN_RADIO), BM_SETCHECK, BST_CHECKED, 0);
        SendMessageA(GetDlgItem(hwndDlg, IDC_HEX_RADIO), BM_SETCHECK, BST_CHECKED, 0);
        SendMessageA(GetDlgItem(hwndDlg, IDC_HEX_RADIO), BM_CLICK, 0, 0);
        SetFocus(GetDlgItem(hwndDlg, IDC_LENGTH_EDIT));
        SendMessageA(GetDlgItem(hwndDlg, IDC_LENGTH_EDIT), EM_SETSEL, 0, -1);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwndDlg);
        dialog = NULL;
        return true;
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_START_EDIT:
        {
            if (HIWORD(wParam) == EN_UPDATE)
            {
                HWND hwnd = (HWND)lParam;
                checkAndFixInputText(hwnd);
            }
            break;
        }
        case IDC_END_EDIT:
        {
            if (HIWORD(wParam) == EN_SETFOCUS)
            {
                SendMessageA(GetDlgItem(hwndDlg, IDC_LENGTH_EDIT), EM_SETREADONLY, TRUE, NULL);
                SendMessageA(GetDlgItem(hwndDlg, IDC_END_EDIT), EM_SETREADONLY, FALSE, NULL);
                SendMessageA(GetDlgItem(hwndDlg, IDC_LEN_RADIO), BM_SETCHECK, BST_UNCHECKED, NULL);
                SendMessageA(GetDlgItem(hwndDlg, IDC_END_RADIO), BM_SETCHECK, BST_CHECKED, NULL);

            }

            else if (HIWORD(wParam) == EN_UPDATE)
            {
                HWND hwnd = (HWND)lParam;
                checkAndFixInputText(hwnd);
            }

            break;
        }
        case IDC_LENGTH_EDIT:
        {
            if (HIWORD(wParam) == EN_SETFOCUS)
            {
                SendMessageA(GetDlgItem(hwndDlg, IDC_LENGTH_EDIT), EM_SETREADONLY, FALSE, NULL);
                SendMessageA(GetDlgItem(hwndDlg, IDC_END_EDIT), EM_SETREADONLY, TRUE, NULL);
                SendMessageA(GetDlgItem(hwndDlg, IDC_END_RADIO), BM_SETCHECK, BST_UNCHECKED, NULL);
                SendMessageA(GetDlgItem(hwndDlg, IDC_LEN_RADIO), BM_SETCHECK, BST_CHECKED, NULL);
            }

            else if (HIWORD(wParam) == EN_UPDATE)
            {
                HWND hwnd = (HWND)lParam;
                checkAndFixInputText(hwnd);
            }

            break;
        }

        case IDC_LEN_RADIO:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                SendMessageA(GetDlgItem(hwndDlg, IDC_LENGTH_EDIT), EM_SETREADONLY, FALSE, NULL);
                SendMessageA(GetDlgItem(hwndDlg, IDC_END_EDIT), EM_SETREADONLY, TRUE, NULL);
            }
            break;
        }

        case IDC_END_RADIO:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                SendMessageA(GetDlgItem(hwndDlg, IDC_LENGTH_EDIT), EM_SETREADONLY, TRUE, NULL);
                SendMessageA(GetDlgItem(hwndDlg, IDC_END_EDIT), EM_SETREADONLY, FALSE, NULL);
            }
            break;
        }

        case IDC_DEC_RADIO:
        {
            if (HIWORD(wParam) == BN_CLICKED)
                convertBaseInputs(hwndDlg, NUM_BASE::DEC);
            break;
        }

        case IDC_HEX_RADIO:
        {
            if (HIWORD(wParam) == BN_CLICKED)
                convertBaseInputs(hwndDlg, NUM_BASE::HEX);
            break;
        }

        case IDC_OCT_RADIO:
        {
            if (HIWORD(wParam) == BN_CLICKED)
                convertBaseInputs(hwndDlg, NUM_BASE::OCT);
            break;
        }

        case IDC_BUTTON_OK:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                auto start = getStartAddress(hwndDlg);
                auto end = getStopAddress(hwndDlg);

                if (start > end)
                    MessageBoxA(dialog, "Start address can't be higher than stop address", "Wrong data", MB_ICONWARNING);

                else if (!Script::Gui::Dump::SelectionSet(start, end))
                    MessageBoxA(dialog, "Couldn't set your selection. Maybe those values are out of bounds for this memory page?", "Couldn't select", MB_ICONWARNING);

                else
                    HideSelectBlockDialog();
            }
            break;
        }

        case IDC_BUTTON_CANCEL:
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                HideSelectBlockDialog();
            }
            break;
        }

        default:
            break;
        }
    }
    break;
    default:
        break;
    }

    return false;
}


WNDPROC ptr_l;
LRESULT CALLBACK SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYUP:
    {
        if (wParam == VK_TAB)
        {
            bool isShiftPressed = GetAsyncKeyState(VK_LSHIFT) & 0x8000;

            static const int tabOrder[] = { IDC_START_EDIT, IDC_END_EDIT, IDC_LENGTH_EDIT };

            int currentIndex = 0;
            for (int i = 0; i < sizeof(tabOrder) / sizeof(int); i++)
            {
                if (GetDlgItem(dialog, tabOrder[i]) == hwnd)
                {
                    currentIndex = i;
                    break;
                }
            }

            if (isShiftPressed)
                currentIndex--;
            else
                currentIndex++;

            if (currentIndex < 0)
                currentIndex = sizeof(tabOrder) / sizeof(int) - 1;

            else if (currentIndex >= sizeof(tabOrder) / sizeof(int))
                currentIndex = 0;

            SetFocus(GetDlgItem(dialog, tabOrder[currentIndex]));
            SendMessage(GetDlgItem(dialog, tabOrder[currentIndex]), EM_SETSEL, 0, -1);
        }

        else if (wParam == VK_ESCAPE)
        {
            SendMessageA(GetDlgItem(dialog, IDC_BUTTON_CANCEL), BM_CLICK, 0, 0);
        }

        else if (wParam == VK_RETURN)
        {
            SendMessageA(GetDlgItem(dialog, IDC_BUTTON_OK), BM_CLICK, 0, 0);
        }

        break;
    }

    }

    WNDPROC oldProc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
}

void ShowSelectBlockDialog(HINSTANCE instance, uint64_t start, uint64_t end)
{
    if (!dialog)
        dialog = CreateDialog(instance, MAKEINTRESOURCE(IDD_SelectBlock), GuiGetWindowHandle(), DialogProc);

    ptr_l = (WNDPROC)SetWindowLongPtr(GetDlgItem(dialog, IDC_START_EDIT), GWL_WNDPROC, (DWORD)SubClassProc);
    SetWindowLongPtr(GetDlgItem(dialog, IDC_START_EDIT), GWLP_USERDATA, (LONG)ptr_l);

    ptr_l = (WNDPROC)SetWindowLongPtr(GetDlgItem(dialog, IDC_END_EDIT), GWL_WNDPROC, (DWORD)SubClassProc);
    SetWindowLongPtr(GetDlgItem(dialog, IDC_END_EDIT), GWLP_USERDATA, (LONG)ptr_l);

    ptr_l = (WNDPROC)SetWindowLongPtr(GetDlgItem(dialog, IDC_LENGTH_EDIT), GWL_WNDPROC, (DWORD)SubClassProc);
    SetWindowLongPtr(GetDlgItem(dialog, IDC_LENGTH_EDIT), GWLP_USERDATA, (LONG)ptr_l);

    setupDialogValues(dialog, start, end);
    ShowWindow(dialog, SW_SHOW);
    centerWindow(dialog);
}
