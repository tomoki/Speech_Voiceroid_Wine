// I don't know why, but C++ does not work for Wine.
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char* yukari = "VOICEROIDÅ{ åãåéÇ‰Ç©ÇË";

// http://onishi-lab.jp/programming/clip_win.html
// TODO: GlobalFree.
boolean copy_to_clipboard(const char* str){
    char* mem = GlobalAlloc(GMEM_MOVEABLE, strlen(str));
    if(mem == NULL) return false;

    char* shared = (char*) GlobalLock(mem);
    if(!shared) return false;
    strcpy(shared, str);
    GlobalUnlock(shared);

    if(OpenClipboard(NULL)){
        EmptyClipboard();
        SetClipboardData(CF_TEXT, mem);
        CloseClipboard();
        return true;
    }else{
        return false;
    }
}

INPUT make_key(short key, boolean is_down){
    INPUT k;
    k.type           = INPUT_KEYBOARD;
    k.ki.wVk         = key;
    k.ki.wScan       = (short)MapVirtualKey(key, 0);
    k.ki.dwFlags     = is_down ? 0 : KEYEVENTF_KEYUP;
    k.ki.dwExtraInfo = 0;
    k.ki.time        = 0;
    return k;
}

INPUT make_key_down(short key){
    return make_key(key, true);
}

INPUT make_key_up(short key){
    return make_key(key, false);
}

UINT send_key(short key){
    INPUT e[] = {
        make_key_down(key), make_key_up(key)
    };
    return SendInput(2, e, sizeof(INPUT));
}
UINT send_key_down(short key){
    INPUT e = make_key_down(key);
    return SendInput(1, &e, sizeof(INPUT));
}
UINT send_key_up(short key){
    INPUT e = make_key_up(key);
    return SendInput(1, &e, sizeof(INPUT));
}

UINT send_key_with_ctrl(short key){
    INPUT e[] = {
        make_key_down(VK_CONTROL),
        make_key_down(key),
        make_key_up(key),
        make_key_up(VK_CONTROL)
    };
    return SendInput(4, e, sizeof(INPUT));
}

int main(int argc, char**argv){
    if(argc != 3){
        printf("Usage: speech_voiceroid.exe yukari \"Hello World\"\n");
        return 1;
    }
    const char* speaker = argv[1];
    const char* message = argv[2];
    const char* appname = yukari; // TODO: change to variable

    const HWND win = FindWindow(NULL, appname);
    if(win == NULL){
        printf("Error: could not find %s\n", appname);
        return 1;
    }

    UINT search_path[] = {
        GW_CHILD,    GW_CHILD,    GW_HWNDNEXT,
        GW_CHILD,    GW_HWNDNEXT, GW_HWNDNEXT,
        GW_HWNDNEXT, GW_HWNDNEXT
    };
    HWND cur = win;
    for(size_t i=0;i<(sizeof(search_path)/sizeof(UINT)); i++){
        cur = GetWindow(cur, search_path[i]);
        if(cur == NULL){
            printf("Can't find handle %d\n", (i-1));
            return 1;
        }
    }
    const HWND play_button = cur;

    // take window foreground.
    ShowWindow(win, SW_SHOWNORMAL);
    SetForegroundWindow(win);

    Sleep(10);

    copy_to_clipboard(message);
    send_key_with_ctrl(VK_OEM_2); // Ctrl+'/' to select all
    send_key(VK_DELETE);          // Delete
    send_key_with_ctrl('V');      // Paste

    Sleep(10);
    SendMessage(play_button, 0, 0, 0); // press play.

    return 0;
}
