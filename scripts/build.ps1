# Build script for Windows (PowerShell)

Write-Host "[*] Building project..."
cd ..
make clean
make all

if ($LASTEXITCODE -eq 0) {
    Write-Host "[OK] Build successful."
} else {
    Write-Host "[FAIL] Build failed."
}
