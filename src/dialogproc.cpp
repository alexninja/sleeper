#include "common.h"

BOOL CALLBACK DialogProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(hdlg, IDC_EDIT_HOURS, 0, FALSE);
        SetDlgItemInt(hdlg, IDC_EDIT_MINUTES, 0, FALSE);
        //SetFocus(GetDlgItem(hdlg, IDC_EDIT_MINUTES));
        return TRUE;
        
    case WM_COMMAND:
        switch(LOWORD(wParam))
        { 
        case IDOK:
            {
                BOOL success;
                int hrs = GetDlgItemInt(hdlg, IDC_EDIT_HOURS, &success, FALSE);
                if (success == FALSE)
                    EndDialog(hdlg, 0);

                int mins = GetDlgItemInt(hdlg, IDC_EDIT_MINUTES, &success, FALSE);
                if (!success)
                    EndDialog(hdlg, 0);

                EndDialog(hdlg, hrs*60+mins);
                return TRUE;
            }
        case IDCANCEL:
            EndDialog(hdlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
