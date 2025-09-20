#!/bin/bash
# Run client with optional config

CONFIG=${1:-../assets/client_chat.cfg}
../build/bin/client "$CONFIG"
