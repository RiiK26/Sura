#!/bin/bash
set -e

REPO="RiiK26/Sura"
echo "Checking for updates from https://github.com/$REPO..."

# Fetch latest release info using GitHub API
LATEST_RELEASE_URL=$(curl -s "https://api.github.com/repos/$REPO/releases/latest" | grep "browser_download_url.*Sura-.*\.AppImage" | cut -d : -f 2,3 | tr -d \" | xargs)

if [ -z "$LATEST_RELEASE_URL" ]; then
    echo "Error: Could not find the latest release."
    echo "Please ensure you have an active internet connection and that an AppImage release is published on GitHub."
    exit 1
fi

echo "Found latest release! Updating..."

# Download to temporary files to prevent corrupted binaries on network failure
curl -L --fail -o ~/.local/bin/sura.tmp "$LATEST_RELEASE_URL"
chmod +x ~/.local/bin/sura.tmp
mv ~/.local/bin/sura.tmp ~/.local/bin/sura

# Make sure icon metadata is preserved
gio set ~/.local/bin/sura metadata::custom-icon "file://$HOME/.local/share/icons/hicolor/256x256/apps/sura.png" 2>/dev/null || true

# Update the updater scripts safely
curl -sL --fail "https://raw.githubusercontent.com/$REPO/main/scripts/update.sh" -o ~/.local/bin/sura-update.tmp
chmod +x ~/.local/bin/sura-update.tmp
mv ~/.local/bin/sura-update.tmp ~/.local/bin/sura-update

curl -sL --fail "https://raw.githubusercontent.com/$REPO/main/scripts/uninstall.sh" -o ~/.local/bin/sura-uninstall.tmp
chmod +x ~/.local/bin/sura-uninstall.tmp
mv ~/.local/bin/sura-uninstall.tmp ~/.local/bin/sura-uninstall

echo "Sura has been successfully updated to the latest version!"
