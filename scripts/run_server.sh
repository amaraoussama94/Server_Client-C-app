#!/bin/bash
# Run server with optional config

CONFIG=${1:-assets/server.cfg}
../build/bin/server "$CONFIG"
