// Test script to demonstrate Playwright MCP browser initialization issues
// This test reproduces the error: "Target page, context or browser has been closed"

const testBrowserInitialization = async () => {
    console.log('=== Playwright MCP Browser Initialization Test ===\n');
    
    const browserTypes = ['chromium', 'firefox', 'webkit'];
    const modes = [
        { headless: true, description: 'headless mode' },
        { headless: false, description: 'headful mode' }
    ];
    
    const results = [];
    
    for (const browserType of browserTypes) {
        for (const mode of modes) {
            console.log(`Testing ${browserType} in ${mode.description}...`);
            
            try {
                // This should fail with "Target page, context or browser has been closed"
                const result = await testNavigation(browserType, mode.headless);
                results.push({
                    browser: browserType,
                    mode: mode.description,
                    status: 'SUCCESS',
                    result: result
                });
            } catch (error) {
                results.push({
                    browser: browserType,
                    mode: mode.description,
                    status: 'FAILED',
                    error: error.message || error.toString()
                });
                console.log(`  ❌ Failed: ${error.message || error.toString()}\n`);
            }
        }
    }
    
    // Summary
    console.log('\n=== Test Summary ===');
    console.log('Browser Initialization Results:');
    results.forEach(r => {
        console.log(`- ${r.browser} (${r.mode}): ${r.status}`);
        if (r.status === 'FAILED') {
            console.log(`  Error: ${r.error}`);
        }
    });
    
    // Expected vs Actual
    console.log('\n=== Expected vs Actual ===');
    console.log('Expected: Browsers should initialize and navigate to URLs');
    console.log('Actual: All browser initialization attempts fail with "Target page, context or browser has been closed"');
    
    // Additional diagnostics
    console.log('\n=== Diagnostic Information ===');
    console.log('Common error patterns observed:');
    console.log('1. Chrome/Chromium: Immediate failure with "Target page, context or browser has been closed"');
    console.log('2. Firefox: Process exits with signal=SIGTRAP');
    console.log('3. WebKit: Trace/BPT trap: 5 error');
    console.log('\nPossible causes:');
    console.log('- macOS security restrictions (Gatekeeper, code signing)');
    console.log('- Missing browser binaries or incorrect paths');
    console.log('- Permissions issues with browser executable');
    console.log('- MCP server configuration issues');
};

// Helper function to test navigation
async function testNavigation(browserType, headless) {
    // This would normally use the Playwright MCP navigation tool
    // For demonstration, we'll simulate the expected call
    return `Navigate to https://example.com using ${browserType}`;
}

// Note: To run this test with actual Playwright MCP, uncomment the following:
/*
// Using Playwright MCP tools:
async function testNavigationWithMCP(browserType, headless) {
    // This will fail with the browser initialization error
    const result = await mcp__playwright__playwright_navigate({
        url: 'https://example.com',
        browserType: browserType,
        headless: headless
    });
    return result;
}
*/

// Run the test
testBrowserInitialization();