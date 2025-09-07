#!/bin/bash
# Build script for Unix-like systems
cd ..
echo "[*] Building project..."
make clean && make all

if [ $? -eq 0 ]; then
    echo "[✓] Build successful."
else
    echo "[✗] Build failed."
fi
