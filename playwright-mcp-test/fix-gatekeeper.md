# Fixing Playwright MCP Gatekeeper Issue

## Problem Confirmation
The Playwright browsers are being rejected by macOS Gatekeeper:
```
spctl -a -v /Users/cyrusops/Library/Caches/ms-playwright/chromium-1179/chrome-mac/Chromium.app
# Output: rejected
```

## Manual Fix Required
Since the automated fixes (xattr, codesign) didn't resolve the issue, and we cannot use sudo in the automation context, the user needs to manually approve the Playwright browsers.

### Option 1: Open System Settings and Allow
1. Try to run Playwright MCP navigation
2. When it fails, go to System Settings > Privacy & Security
3. Look for a message about "Chromium.app was blocked"
4. Click "Open Anyway"

### Option 2: Command Line Fix (requires sudo)
Run these commands in a terminal with admin privileges:
```bash
# For Chromium
sudo spctl --add --label "Playwright" /Users/cyrusops/Library/Caches/ms-playwright/chromium-1179/chrome-mac/Chromium.app
sudo spctl --enable --label "Playwright"

# For Firefox  
sudo spctl --add --label "Playwright" /Users/cyrusops/Library/Caches/ms-playwright/firefox-1488/firefox/Nightly.app
sudo spctl --enable --label "Playwright"

# For WebKit
sudo spctl --add --label "Playwright" /Users/cyrusops/Library/Caches/ms-playwright/webkit-2182/Playwright.app
sudo spctl --enable --label "Playwright"
```

### Option 3: Disable Gatekeeper Temporarily (Not Recommended)
```bash
sudo spctl --master-disable
# Run your Playwright tests
sudo spctl --master-enable
```

## Alternative Solution: Use System Browsers
Instead of using Playwright's bundled browsers, configure Playwright to use system-installed browsers if available.