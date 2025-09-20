# Windows PowerShell
# Run client with optional config

$CONFIG = if ($args.Count -ge 1) { $args[0] } else { "../assets/client_chat.cfg" }

Write-Host "Using config file: $CONFIG"
& "../build/bin/client.exe" $CONFIG
