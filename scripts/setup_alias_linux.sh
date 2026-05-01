#!/usr/bin/env bash

# setup_alias.sh
# This script sets up the 'rm' alias to use 'rmSafe' instead.
# It detects the current shell and appends the alias to the appropriate config file.

echo "Setting up rmSafe alias..."

# Function to add alias if it doesn't exist
add_alias() {
    local rc_file="$1"
    local alias_cmd="alias rm='rmSafe'"
    
    if [ ! -f "$rc_file" ]; then
        echo "Creating $rc_file..."
        touch "$rc_file"
    fi

    # Check if the alias already exists
    if grep -q "alias rm=['\"]rmSafe['\"]" "$rc_file" || grep -q "alias rm=rmSafe" "$rc_file"; then
        echo "Alias already exists in $rc_file"
    else
        echo "" >> "$rc_file"
        echo "# Use rmSafe instead of standard rm for safety" >> "$rc_file"
        echo "$alias_cmd" >> "$rc_file"
        echo "Added alias to $rc_file"
    fi
}

# Determine the shell and update appropriate files
SHELL_BASENAME=$(basename "$SHELL")

case "$SHELL_BASENAME" in
    bash)
        if [ -f "$HOME/.bashrc" ]; then
            add_alias "$HOME/.bashrc"
        elif [ -f "$HOME/.bash_profile" ]; then
            add_alias "$HOME/.bash_profile"
        else
            add_alias "$HOME/.bashrc"
        fi
        ;;
    zsh)
        add_alias "$HOME/.zshrc"
        ;;
    fish)
        # Fish uses a different syntax for aliases, usually stored in functions or config.fish
        FISH_CONFIG="$HOME/.config/fish/config.fish"
        if [ ! -d "$(dirname "$FISH_CONFIG")" ]; then
            mkdir -p "$(dirname "$FISH_CONFIG")"
        fi
        if ! grep -q "alias rm rmSafe" "$FISH_CONFIG" 2>/dev/null; then
            echo "" >> "$FISH_CONFIG"
            echo "# Use rmSafe instead of standard rm for safety" >> "$FISH_CONFIG"
            echo "alias rm rmSafe" >> "$FISH_CONFIG"
            echo "Added alias to $FISH_CONFIG"
        else
             echo "Alias already exists in $FISH_CONFIG"
        fi
        ;;
    *)
        # Fallback to common files
        echo "Unsupported or undetected shell: $SHELL_BASENAME. Attempting fallback..."
        add_alias "$HOME/.bashrc"
        add_alias "$HOME/.zshrc"
        ;;
esac

echo "=================================================================="
echo "Alias setup complete!"
echo "Please restart your terminal or run the following command"
echo "to apply the changes in your current session:"
echo ""
if [ "$SHELL_BASENAME" = "zsh" ]; then
    echo "  source ~/.zshrc"
elif [ "$SHELL_BASENAME" = "bash" ]; then
    echo "  source ~/.bashrc"
elif [ "$SHELL_BASENAME" = "fish" ]; then
    echo "  source ~/.config/fish/config.fish"
else
    echo "  source ~/.bashrc  (or your respective shell config file)"
fi
echo "=================================================================="
