@echo off

md build 2> nul
clang src/main.c libs/argparse/argparse.c -o build/win-notify-send.exe  -luser32 -lshell32 -Wl,/ENTRY:mainCRTStartup

