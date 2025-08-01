# Sentry Issue Analysis: BACKEND-RUST-NA

## Issue Summary

**Issue ID**: BACKEND-RUST-NA  
**Organization**: ceedar  
**Project**: backend-rust  
**Status**: Unresolved  
**Total Occurrences**: 54,330  
**First Seen**: July 29, 2025  
**Last Seen**: August 1, 2025  

## Error Details

### Error Message
```
System Error: missing field `prompt_version` at line 1 column 323
```

### Location
- **File**: `src/economic_events/detect_counterparty.rs`
- **Line**: 300
- **GitHub URL**: https://github.com/ceedario/ceedar/blob/backend-v0.0.342/backend/src/economic_events/detect_counterparty.rs#L300

## Root Cause Analysis

This is a **deserialization error** occurring in the Rust backend. The error indicates that the code is attempting to deserialize JSON data that is missing a required field called `prompt_version`.

### Likely Causes

1. **Schema Mismatch**: The struct being used for deserialization expects a `prompt_version` field, but the incoming JSON data doesn't contain it.

2. **API Version Incompatibility**: The data source may have changed its schema, removing or renaming the `prompt_version` field.

3. **Missing Default Value**: The Rust struct likely doesn't have a default value or `Option<>` type for the `prompt_version` field.

## Recommended Fixes

### Fix 1: Make the Field Optional (Recommended)
```rust
// In the struct definition, change:
prompt_version: String,  // or whatever type it is

// To:
prompt_version: Option<String>,
```

### Fix 2: Add Default Value
```rust
use serde::{Deserialize, Serialize};

#[derive(Deserialize, Serialize)]
struct YourStruct {
    #[serde(default)]
    prompt_version: String,
    // other fields...
}
```

### Fix 3: Use Serde's Default Attribute
```rust
#[derive(Deserialize)]
struct YourStruct {
    #[serde(default = "default_prompt_version")]
    prompt_version: String,
    // other fields...
}

fn default_prompt_version() -> String {
    "1.0.0".to_string()  // or whatever default makes sense
}
```

### Fix 4: Handle Missing Fields Gracefully
```rust
use serde::{Deserialize, Deserializer};

#[derive(Deserialize)]
struct YourStruct {
    #[serde(deserialize_with = "deserialize_prompt_version")]
    prompt_version: String,
    // other fields...
}

fn deserialize_prompt_version<'de, D>(deserializer: D) -> Result<String, D::Error>
where
    D: Deserializer<'de>,
{
    let opt = Option::<String>::deserialize(deserializer)?;
    Ok(opt.unwrap_or_else(|| "default_version".to_string()))
}
```

## Implementation Steps

1. **Locate the Struct**: Find the struct definition in `detect_counterparty.rs` around line 300 that's being used for deserialization.

2. **Identify Usage**: Check where this struct is being deserialized (likely using `serde_json::from_str` or similar).

3. **Apply Fix**: Choose one of the recommended fixes based on your business logic:
   - If `prompt_version` is truly optional, use `Option<String>`
   - If it should have a default, use the `#[serde(default)]` attribute
   - If you need custom handling, implement a custom deserializer

4. **Test**: Ensure the fix handles both cases (with and without `prompt_version`).

## Additional Recommendations

1. **Add Logging**: Log the raw JSON being deserialized to help debug future issues.

2. **Version Your APIs**: If this is an external API, consider versioning to handle schema changes gracefully.

3. **Add Tests**: Create unit tests that verify deserialization works with and without the `prompt_version` field.

## Note on Seer AI Analysis

The Seer AI analysis tool is not enabled for the ceedar organization. To get AI-powered root cause analysis in the future, you'll need to:
1. Open an issue at sentry.io/issues
2. Request Seer to be enabled for your organization

## Resolution

Once you implement one of the fixes above and deploy it, you can mark this issue as resolved in Sentry by:
- Referencing `Fixes BACKEND-RUST-NA` in your commit message
- Or manually resolving it in the Sentry dashboard