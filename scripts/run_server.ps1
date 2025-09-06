#win powershell
# Run serveur  with optional config

CONFIG=${1:-../assets/server.cfg}
../build/bin/server.exe "$CONFIG"
