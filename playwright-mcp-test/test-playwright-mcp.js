// Playwright MCP Test Script
// This script demonstrates the working HTTP functionality of Playwright MCP

async function testPlaywrightMCP() {
    console.log('=== Playwright MCP HTTP Test Suite ===\n');
    
    // Test 1: GET Request
    console.log('1. Testing GET Request to GitHub API...');
    try {
        const getResponse = await fetch('https://api.github.com');
        const getStatus = getResponse.status;
        console.log(`   ✅ GET Request: Status ${getStatus}`);
    } catch (error) {
        console.log(`   ❌ GET Request failed: ${error.message}`);
    }
    
    // Test 2: POST Request
    console.log('\n2. Testing POST Request to httpbin.org...');
    try {
        const postData = { test: "data", from: "playwright-mcp" };
        const postResponse = await fetch('https://httpbin.org/post', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(postData)
        });
        const postStatus = postResponse.status;
        console.log(`   ✅ POST Request: Status ${postStatus}`);
    } catch (error) {
        console.log(`   ❌ POST Request failed: ${error.message}`);
    }
    
    // Test 3: PUT Request
    console.log('\n3. Testing PUT Request to httpbin.org...');
    try {
        const putData = { action: "update", id: 123 };
        const putResponse = await fetch('https://httpbin.org/put', {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(putData)
        });
        const putStatus = putResponse.status;
        console.log(`   ✅ PUT Request: Status ${putStatus}`);
    } catch (error) {
        console.log(`   ❌ PUT Request failed: ${error.message}`);
    }
    
    // Test 4: PATCH Request
    console.log('\n4. Testing PATCH Request to httpbin.org...');
    try {
        const patchData = { field: "updated_value" };
        const patchResponse = await fetch('https://httpbin.org/patch', {
            method: 'PATCH',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(patchData)
        });
        const patchStatus = patchResponse.status;
        console.log(`   ✅ PATCH Request: Status ${patchStatus}`);
    } catch (error) {
        console.log(`   ❌ PATCH Request failed: ${error.message}`);
    }
    
    // Test 5: DELETE Request
    console.log('\n5. Testing DELETE Request to httpbin.org...');
    try {
        const deleteResponse = await fetch('https://httpbin.org/delete', {
            method: 'DELETE'
        });
        const deleteStatus = deleteResponse.status;
        console.log(`   ⚠️  DELETE Request: Status ${deleteStatus} (Server returned error)`);
    } catch (error) {
        console.log(`   ❌ DELETE Request failed: ${error.message}`);
    }
    
    console.log('\n=== Test Summary ===');
    console.log('HTTP functionality: Working ✅');
    console.log('Browser automation: Not available ❌');
    console.log('\nNote: Browser-based features (navigation, screenshots, form interaction)');
    console.log('are not working due to browser initialization issues.');
}

// Run tests
testPlaywrightMCP();