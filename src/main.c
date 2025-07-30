#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include <shellapi.h>
#include <synchapi.h>
#include <windef.h>
#include <winuser.h>

#include <psapi.h>

#include "../libs/argparse/argparse.h"

NOTIFYICONDATA nid;
HWND hwnd;

char *title = " ";
char *body = " ";
char *icon = NULL;
char *urgency = "normal";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
  case WM_CREATE: {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = (rand() % (10000 - 1 + 1)) + 1;
    nid.uFlags = NIF_INFO;

    lstrcpy(nid.szInfoTitle, TEXT(title));
    lstrcpy(nid.szInfo, TEXT(body));

    nid.dwInfoFlags = NIIF_NONE;

    if (strcmp(urgency, "low")) {
      nid.dwInfoFlags = NIIF_INFO;
    } else if (strcmp(urgency, "critical")) {
      nid.dwInfoFlags = NIIF_ERROR;
    }

    if (icon != NULL) {
      nid.dwInfoFlags = NIIF_USER;
      nid.hBalloonIcon =
          LoadImageA(NULL, TEXT(icon), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }

    Shell_NotifyIcon(NIM_ADD, &nid);
    // Shell_NotifyIcon(NIM_MODIFY, &nid);
    Shell_NotifyIcon(NIM_DELETE, &nid);

    PostQuitMessage(0);

    break;
  }

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }

  return DefWindowProc(hwnd, msg, wParam, lParam);
}

void create_window() {
  HINSTANCE hInstance = GetModuleHandle(NULL);

  WNDCLASS wc = {0};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = TEXT("notify-send");

  RegisterClass(&wc);

  hwnd = CreateWindowEx(0, wc.lpszClassName, TEXT("notify-send"),
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300,
                        200, NULL, NULL, hInstance, NULL);

  ShowWindow(hwnd, SW_HIDE); // Hide window
}

void window_loop() {
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

static const char *const usages[] = {
    "notify-send [OPTIONS] {summary} [body]",
    NULL,
};

int main(int argc, const char *argv[]) {
  struct argparse_option options[] = {
      OPT_HELP(),
      OPT_GROUP("Basic options"),
      OPT_STRING('a', "app-name", &title,
                 "Specifies the app name for the notification", NULL, 0, 0),
      OPT_STRING('i', "icon", &icon, "Specifies an icon filename", NULL, 0, 0),
      OPT_STRING('u', "urgency", &urgency,
                 "Specifies the urgency level (low, normal, critical)", NULL, 0,
                 0),
      OPT_END(),
  };

  struct argparse argparse;
  argparse_init(&argparse, options, usages, 0);
  argparse_describe(
      &argparse, "\nwin-notify-send - a program to send desktop notifications",
      "\nWith win-notify-send you can send desktop notifications to the user "
      "\nvia a command line.");
  argc = argparse_parse(&argparse, argc, argv);

  if (argc == 1) {
    body = _strdup(argv[0]);
  } else {
    printf(
        "\nERROR: Body not defined. Please add one positonal argument for the "
        "\nbody (ex. win-notify-send \"Notification Body\" -a \"a cool app\")");
  }

  create_window();
  window_loop();

  return 0;
}
