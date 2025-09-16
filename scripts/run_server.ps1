# PowerShell - run_server.ps1
# Exécute le serveur avec un fichier de config optionnel

$CONFIG = if ($args.Count -ge 1) { $args[0] } else { "../assets/server.cfg" }

Write-Host "Using config file: $CONFIG"
& "../build/bin/server.exe" $CONFIG
