# setup_alias_windows.ps1
# This script sets up the 'rm' alias to use 'rmSafe' instead in PowerShell.

Write-Host "Setting up rmSafe alias for PowerShell..."

# Check if profile exists, if not, create it
if (!(Test-Path -Path $PROFILE)) {
    Write-Host "Creating PowerShell profile..."
    New-Item -ItemType File -Path $PROFILE -Force | Out-Null
}

$profilePath = $PROFILE
$aliasCommand = "Set-Alias -Name rm -Value rmSafe"

# Check if the alias already exists in the profile
$profileContent = Get-Content -Path $profilePath -ErrorAction SilentlyContinue

if ($profileContent -match "Set-Alias\s+-Name\s+rm\s+-Value\s+rmSafe") {
    Write-Host "Alias already exists in your PowerShell profile ($profilePath)."
} else {
    # Append the alias to the profile
    Add-Content -Path $profilePath -Value "`n# Use rmSafe instead of standard rm for safety"
    Add-Content -Path $profilePath -Value $aliasCommand
    Write-Host "Added alias to your PowerShell profile ($profilePath)."
}

Write-Host "=================================================================="
Write-Host "Alias setup complete!"
Write-Host "Please restart your PowerShell terminal or run the following command"
Write-Host "to apply the changes in your current session:"
Write-Host ""
Write-Host "  . `$PROFILE"
Write-Host "=================================================================="
