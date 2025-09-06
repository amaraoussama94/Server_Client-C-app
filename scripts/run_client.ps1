#win powershell
# Run client with optional config

CONFIG=${1:-../assets/client.cfg}
../build/bin/client.exe "$CONFIG"
