#!/bin/bash
set -e
echo -e "\033[33mInstalling Sura...\033[0m"

REPO="RiiK26/Sura"
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" 2>/dev/null && pwd || echo "" )"

# Create standard user directories if they don't exist
mkdir -p ~/.local/bin ~/.local/share/applications ~/.local/share/icons/hicolor/256x256/apps

if [ -n "$SCRIPT_DIR" ] && [ -f "$SCRIPT_DIR/../build/Sura" ]; then
    echo "Installing from local source build..."
    cp "$SCRIPT_DIR/../build/Sura" ~/.local/bin/sura
    cp "$SCRIPT_DIR/update.sh" ~/.local/bin/sura-update
    cp "$SCRIPT_DIR/uninstall.sh" ~/.local/bin/sura-uninstall
    cp "$SCRIPT_DIR/../resources/images/icon.png" ~/.local/share/icons/hicolor/256x256/apps/sura.png
else
    echo "Fetching latest AppImage from GitHub..."
    LATEST_RELEASE_URL=$(curl -s "https://api.github.com/repos/$REPO/releases/latest" | grep "browser_download_url.*Sura-.*\.AppImage" | cut -d : -f 2,3 | tr -d \" | xargs)
    if [ -z "$LATEST_RELEASE_URL" ]; then
        echo "Error: Could not find the latest AppImage release."
        exit 1
    fi
    echo "Downloading components..."
    curl -L --fail -o ~/.local/bin/sura.tmp "$LATEST_RELEASE_URL"
    curl -sL --fail "https://raw.githubusercontent.com/$REPO/main/scripts/update.sh" -o ~/.local/bin/sura-update.tmp
    curl -sL --fail "https://raw.githubusercontent.com/$REPO/main/scripts/uninstall.sh" -o ~/.local/bin/sura-uninstall.tmp
    curl -sL --fail "https://raw.githubusercontent.com/$REPO/main/resources/images/icon.png" -o ~/.local/share/icons/hicolor/256x256/apps/sura.png.tmp
    
    # Atomic moves ensure we don't leave corrupted files if download fails
    mv ~/.local/bin/sura.tmp ~/.local/bin/sura
    mv ~/.local/bin/sura-update.tmp ~/.local/bin/sura-update
    mv ~/.local/bin/sura-uninstall.tmp ~/.local/bin/sura-uninstall
    mv ~/.local/share/icons/hicolor/256x256/apps/sura.png.tmp ~/.local/share/icons/hicolor/256x256/apps/sura.png
fi

chmod +x ~/.local/bin/sura
chmod +x ~/.local/bin/sura-update
chmod +x ~/.local/bin/sura-uninstall

# Set custom file icon for the binary using gio
gio set ~/.local/bin/sura metadata::custom-icon "file://$HOME/.local/share/icons/hicolor/256x256/apps/sura.png" 2>/dev/null || true

# Generate the desktop file dynamically
cat <<EOF > ~/.local/share/applications/sura.desktop
[Desktop Entry]
Version=1.0
Type=Application
Name=Sura
Comment=A lightweight, Qt6 based image viewer
Exec=$HOME/.local/bin/sura %F
Icon=sura
Terminal=false
Categories=Image;Viewer;Graphics;
EOF

# Refresh the desktop database
update-desktop-database ~/.local/share/applications 2>/dev/null || true

echo ""
if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
    echo -e "\033[0;33mWARNING: \$HOME/.local/bin is not in your PATH.\033[0m"
    echo -e "To use the 'sura' command from anywhere, add this line to your ~/.bashrc or ~/.zshrc:"
    echo -e "  export PATH=\"\$HOME/.local/bin:\$PATH\""
    echo ""
fi

echo -e "\033[0;32mSura has been successfully installed!\033[0m"
echo "Sura is now available in your application launcher."
echo "The binaries are located at:"
echo "  - Main App    : $HOME/.local/bin/sura"
echo "  - Updater     : $HOME/.local/bin/sura-update"
echo "  - Uninstaller : $HOME/.local/bin/sura-uninstall"
echo ""
