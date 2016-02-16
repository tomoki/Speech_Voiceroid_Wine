// I don't know why, but C++ does not work for Wine.
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char* SAVE_WIN_NAME = "音声ファイルの保存";
#define SPEAKERS_SIZE 2

char SPEAKERS[SPEAKERS_SIZE][300] = {
    "yukari",
    "maki"
};

char WINDOW_NAME[SPEAKERS_SIZE][300] = {
    "VOICEROID＋ 結月ゆかり",
    "VOICEROID＋ 民安ともえ",
};

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

/* #define IsWindowOwner(h)    (GetWindow(h,GW_OWNER) == NULL) */
HWND traverse_windows(HWND root, UINT* path, size_t path_length){
    HWND cur = root;
    for(size_t i=0;i<path_length; i++){
        cur = GetWindow(cur, path[i]);

        /* For debugging */
        /* TCHAR   szTitle[ 1024 ]; */
        /* TCHAR   szClass[ 1024 ]; */
        /* GetWindowText( cur, szTitle, sizeof(szTitle) );    // キャプションの取得 */
        /* GetClassName(  cur, szClass, sizeof(szClass) );    // クラス文字列の取得 */
        /* // 列挙ウインドウの表示 */
        /* printf( TEXT("%4d: %c %c %c %c %c %c %c [%-50s] [%s]\n"), */
        /*         i,                                       // 列挙番号 */
        /*         IsWindowOwner(cur)   ? 'O' : '_',              // Ownerウインドウ */
        /*         IsWindowUnicode(cur) ? 'U' : '_',              // Unicodeタイプ */
        /*         IsWindowVisible(cur) ? 'V' : '_',              // 可視状態 */
        /*         IsWindowEnabled(cur) ? 'E' : '_',              // 有効状態 */
        /*         IsIconic(cur)        ? 'I' : '_',              // 最小化状態 */
        /*         IsZoomed(cur)        ? 'Z' : '_',              // 最大化状態 */
        /*         IsWindow(cur)        ? 'W' : '_',              // ウインドウ有無 */
        /*         szClass,                                        // クラス文字列 */
        /*         szTitle );                                      // キャプション */
        /* printf("%x\n", cur); */

        if(cur == NULL){
            printf("Can't find handle %d\n", i);
            exit(1);
        }
    }
    return cur;
}

// may take a while.
// path is windows-style like c:\home\users\wine\a.wav
boolean save_to_file_wine(HWND win, char* path){
    UINT path_for_save_button[] = {
        GW_CHILD,
        GW_HWNDNEXT, GW_HWNDNEXT, GW_HWNDNEXT, GW_HWNDNEXT, GW_HWNDNEXT,
        GW_HWNDNEXT, GW_HWNDNEXT, GW_HWNDNEXT, GW_HWNDNEXT, GW_HWNDNEXT,
    };
    UINT path_for_edit[] = {
        GW_CHILD,
        GW_HWNDNEXT, GW_HWNDNEXT, GW_HWNDNEXT, GW_HWNDNEXT,
        GW_HWNDNEXT, GW_HWNDNEXT,
        GW_CHILD, GW_CHILD,
    };

    const HWND save = traverse_windows(win, path_for_save_button,
                                       sizeof(path_for_save_button)/sizeof(UINT));
    const HWND edit = traverse_windows(win, path_for_edit,
                                       sizeof(path_for_edit)/sizeof(UINT));

    SendMessage(edit, WM_SETTEXT, 0, path);
    SendMessage(save, BM_CLICK,   0, 0);
    Sleep(300);

    // TODO: works for alert...
    /* UINT path_for_overwrite_alert[] = { */
    /*     GW_CHILD, GW_HWDLAST */
    /* }; */
    /* const HWND over_alert = traverse_windows(win, path_for_overwrite_alert, */
    /*                                     sizeof(path_for_overwrite_alert)/sizeof(UINT)); */
    /* if(over_alert != NULL){ */
    /*     UINT path_for_overwrite_yes[] = { */
    /*         GW_CHILD,  GW_HWNDNEXT, GW_HWNDNEXT */
    /*     }; */
    /*     const HWND yes_button = traverse_windows(over_alert, path_for_overwrite_yes, */
    /*                                              sizeof(path_for_overwrite_yes)/sizeof(UINT)); */
    /*     SendMessage(save, BM_CLICK, 0, 0); */
    /* } */
}



// Yukari ----------------------------------------------------------------------
void paste_text_yukari(char* message){
    copy_to_clipboard(message);
    send_key_with_ctrl(VK_OEM_2); // Ctrl+'/' to select all
    send_key(VK_DELETE);          // Delete
    send_key_with_ctrl('V');      // Paste
}

void speak_voiceroid_yukari(HWND win, char* message){
    UINT search_path_for_play_button[] = {
        GW_CHILD,    GW_CHILD,    GW_HWNDNEXT,
        GW_CHILD,    GW_HWNDNEXT, GW_HWNDNEXT,
        GW_HWNDNEXT, GW_HWNDNEXT
    };

    const HWND play_button = traverse_windows(win, search_path_for_play_button,
                                              sizeof(search_path_for_play_button)/sizeof(UINT));

    paste_text_yukari(message);
    Sleep(10);
    SendMessage(play_button, 0, 0, 0); // I don't know why 0...
}

void save_voiceroid_yukari(HWND win, char* message, char* path){
    UINT search_path_for_save_button[] = {
        GW_CHILD,    GW_CHILD,    GW_HWNDNEXT,
        GW_CHILD,    GW_HWNDNEXT, GW_HWNDNEXT
    };
    const HWND save_button = traverse_windows(win, search_path_for_save_button,
                                              sizeof(search_path_for_save_button)/sizeof(UINT));
    paste_text_yukari(message);
    Sleep(10);
    PostMessage(save_button, 0, 0, 0);
    Sleep(100);
    const HWND save_window = FindWindow(NULL, SAVE_WIN_NAME);
    save_to_file_wine(save_window, path);
}
// -----------------------------------------------------------------------------

int main(int argc, char**argv){
    if(argc < 3){
        printf("Usage: speech_voiceroid.exe yukari \"Hello World\" [path] \n");
        for(int i=0;i<SPEAKERS_SIZE;i++){
            printf(" %s\t=> %s \n", SPEAKERS[i], WINDOW_NAME[i]);
        }
        return 1;
    }
    const char* speaker = argv[1];
    const char* message = argv[2];
    char* appname = "";
    for(int i=0;i<SPEAKERS_SIZE;i++){
        if(strcmp(SPEAKERS[i], speaker) == 0){
            appname = WINDOW_NAME[i];
        }
    }
    if(strlen(appname) == 0){
        printf("Can't find %s\n", speaker);
        printf("Usable speakers are followings:\n");
        for(int i=0;i<SPEAKERS_SIZE;i++){
            printf(" %s\t=> %s \n", SPEAKERS[i], WINDOW_NAME[i]);
        }
        return 1;
    }

    const HWND win = FindWindow(NULL, appname);
    if(win == NULL){
        printf("Error: could not find %s\n", appname);
        return 1;
    }

    /* // take window foreground. */
    ShowWindow(win, SW_SHOWNORMAL);
    SetForegroundWindow(win);
    Sleep(10);

    if(argc >= 4){
        const char* path = argv[3];
        save_voiceroid_yukari(win, message, path);
    }else{
        speak_voiceroid_yukari(win, message);
    }

    return 0;
}
