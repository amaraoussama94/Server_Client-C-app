# PowerShell - run_server.ps1
# Ex�cute le serveur avec un fichier de config optionnel

CONFIG=${1:-../assets/server.cfg}
../build/bin/server.exe "$CONFIG"