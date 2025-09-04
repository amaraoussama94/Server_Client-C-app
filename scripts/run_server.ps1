#!/bin/bash
# Run client with optional config

CONFIG=${1:-assets/client.cfg}
./client "$CONFIG"
