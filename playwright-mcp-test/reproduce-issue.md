# Playwright MCP Browser Initialization Issue - Reproduction Steps

## Issue Description
The Playwright MCP browser automation features are failing with the error "Target page, context or browser has been closed" across all browser types (Chromium, Firefox, WebKit) and modes (headless/headful).

## Environment
- Platform: macOS Darwin 24.3.0
- Working Directory: /Users/cyrusops/code/hello-world-project-workspaces/TEST-29
- MCP Tools: All Playwright MCP tools are available

## Reproduction Steps

### Step 1: Attempt Basic Navigation
```javascript
// Using Playwright MCP navigation tool
await mcp__playwright__playwright_navigate({
    url: 'https://example.com',
    headless: false
});
```

**Expected Result:** Browser opens and navigates to example.com  
**Actual Result:** Error: "Failed to initialize browser: browser.newContext: Target page, context or browser has been closed"

### Step 2: Try Different Browser Types
```javascript
// Test with Firefox
await mcp__playwright__playwright_navigate({
    url: 'https://example.com',
    browserType: 'firefox',
    headless: true
});
```

**Expected Result:** Firefox browser launches in headless mode  
**Actual Result:** Process exits with signal=SIGTRAP

```javascript
// Test with WebKit
await mcp__playwright__playwright_navigate({
    url: 'https://example.com',
    browserType: 'webkit',
    headless: true
});
```

**Expected Result:** WebKit browser launches  
**Actual Result:** Trace/BPT trap: 5 error

### Step 3: Verify HTTP Functionality Works
```javascript
// HTTP GET request
await mcp__playwright__playwright_get({
    url: 'https://api.github.com'
});
```

**Result:** ✅ Success - Status 200 OK (HTTP functionality works correctly)

## Error Details

### Chromium Error
```
Failed to initialize browser: browser.newContext: Target page, context or browser has been closed
```

### Firefox Error
```
Browser logs:
<launching> /Users/cyrusops/Library/Caches/ms-playwright/firefox-1488/firefox/Nightly.app/Contents/MacOS/firefox -no-remote -headless -profile /var/folders/bw/83ysb4pn61j28nj7hzzxs42h0000gq/T/playwright_firefoxdev_profile-EEO3mG -juggler-pipe -silent
<launched> pid=44470
[pid=44470][err] *** You are running in headless mode.
[pid=44470] <process did exit: exitCode=null, signal=SIGTRAP>
```

### WebKit Error
```
Browser logs:
<launching> /Users/cyrusops/Library/Caches/ms-playwright/webkit-2182/pw_run.sh --inspector-pipe --headless --no-startup-window
<launched> pid=44565
[pid=44565][err] /Users/cyrusops/Library/Caches/ms-playwright/webkit-2182/pw_run.sh: line 7: 44571 Trace/BPT trap: 5
```

## Root Cause Analysis

The browser initialization failures appear to be related to:

1. **macOS Security Restrictions**: The SIGTRAP and "Trace/BPT trap: 5" errors suggest macOS Gatekeeper or code signing issues preventing the browser executables from running.

2. **Browser Binary Issues**: The browsers are being launched from the Playwright cache directory (`/Users/cyrusops/Library/Caches/ms-playwright/`), but the processes are immediately terminated.

3. **Permission Issues**: The browser processes start but are immediately killed, suggesting potential permission or security policy violations.

## Working vs Non-Working Features

### ✅ Working:
- HTTP GET requests
- HTTP POST requests
- HTTP PUT requests
- HTTP PATCH requests
- HTTP DELETE requests (with expected server errors)

### ❌ Not Working:
- Browser navigation
- Screenshots
- Form interactions
- JavaScript execution in browser context
- PDF generation
- Tab management

## Conclusion

The Playwright MCP server's HTTP functionality is working correctly, but browser automation features are completely non-functional due to browser initialization failures on macOS. This appears to be an environment-specific issue related to macOS security policies preventing the Playwright browser binaries from executing properly.