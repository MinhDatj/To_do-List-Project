#!/bin/bash

file="TodoList.exe"

if [ -f "$file" ]; then
    rm "$file"
else 
    echo "Program doesn't exist to delete!"
fi