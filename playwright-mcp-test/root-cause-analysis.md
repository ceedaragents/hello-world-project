# Playwright MCP Browser Initialization - Root Cause Analysis

## Problem Summary
The Playwright MCP server fails to initialize any browser (Chromium, Firefox, WebKit) on macOS, while HTTP functionality works correctly.

## Technical Analysis

### 1. Browser Binary Status
- **Location**: `/Users/cyrusops/Library/Caches/ms-playwright/`
- **Browsers Present**: chromium-1179, firefox-1488, webkit-2182
- **Code Signing**: Chromium has adhoc signature (not properly signed)

### 2. Error Patterns

#### Chromium
- **Error**: "Target page, context or browser has been closed"
- **Cause**: Browser process starts but immediately terminates
- **Code Signing**: `adhoc,linker-signed` - not properly signed for macOS

#### Firefox
- **Error**: Process exits with `signal=SIGTRAP`
- **Cause**: macOS kernel protection terminating unsigned/untrusted process
- **Log**: Shows headless mode starts then immediately trapped

#### WebKit
- **Error**: "Trace/BPT trap: 5"
- **Cause**: Breakpoint trap - typical of macOS security violations
- **Script**: Uses pw_run.sh wrapper which fails at DYLD library loading

### 3. Root Cause: macOS Security Restrictions

The primary issue is macOS security mechanisms preventing unsigned Playwright browser binaries from executing:

1. **Gatekeeper**: Blocks unsigned applications
2. **System Integrity Protection (SIP)**: Prevents unauthorized code execution
3. **Code Signing Requirements**: macOS requires proper code signatures
4. **DYLD Protection**: Dynamic library loading restrictions

### 4. Why HTTP Works but Browsers Don't

- HTTP requests use Node.js native fetch/HTTP modules
- Browser automation requires launching external browser processes
- macOS allows Node.js network operations but blocks unsigned browser executables

### 5. Evidence from Logs

```bash
# Chromium - adhoc signature (not trusted)
Signature=adhoc
TeamIdentifier=not set

# Firefox - SIGTRAP signal
[pid=44470] <process did exit: exitCode=null, signal=SIGTRAP>

# WebKit - Trace/BPT trap during DYLD operations
Trace/BPT trap: 5       DYLD_FRAMEWORK_PATH="$DYLIB_PATH"
```

## Potential Solutions

1. **Remove Quarantine Attributes**
   ```bash
   xattr -cr /Users/cyrusops/Library/Caches/ms-playwright/
   ```

2. **Allow Unsigned Apps in Security Settings**
   - System Settings > Privacy & Security > Allow apps downloaded from: App Store and identified developers
   - May need to manually approve each browser

3. **Use Properly Signed Browsers**
   - Install official Chrome/Firefox/Safari
   - Configure Playwright to use system browsers instead of cached ones

4. **Disable Gatekeeper Temporarily** (Not Recommended)
   ```bash
   sudo spctl --master-disable
   ```

5. **Code Sign the Browsers**
   ```bash
   codesign --force --deep --sign - /path/to/browser.app
   ```

## Conclusion

The Playwright MCP browser initialization failures are caused by macOS security policies blocking the execution of unsigned browser binaries downloaded by Playwright. This is a known issue with Playwright on macOS when browsers are not properly signed or have quarantine attributes.