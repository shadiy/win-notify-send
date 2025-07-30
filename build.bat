@echo off
clang src/main.c libs/argparse/argparse.c -o notify-send.exe  -luser32 -lshell32 -Wl,/ENTRY:mainCRTStartup

