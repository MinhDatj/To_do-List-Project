#!/bin/bash

gcc To_do_list.c -o TodoList.exe

if [ $? -eq 0 ]; then
    echo "Compilation completed!"
else
    echo "Compilation failed!"
fi