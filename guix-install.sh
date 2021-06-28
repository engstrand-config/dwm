#!/bin/sh
guix environment engstrand-dwm --ad-hoc pkg-config -- make clean install PREFIX=~/.local CC=gcc FREETYPEINC="/home/$(whoami)/.guix-profile/include/freetype2/"
