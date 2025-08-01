# Sentry Investigation Summary

## Overview
Investigated the latest issues across two Sentry organizations: **ceedar** and **dealstudio**.

## Critical Findings

### Ceedar Organization - High-Priority Issue
**Issue**: BACKEND-RUST-NA (54,330+ events)
- **Error**: `missing field 'prompt_version' at line 1 column 323`
- **Type**: JSON deserialization error in Rust backend
- **Location**: `/src/economic_events/detect_counterparty.rs:300`
- **Impact**: High volume (2,000+ events/hour), but no users directly affected
- **Root Cause**: Schema mismatch - code expects `prompt_version` field that's missing in incoming JSON

**Recommended Fix**: Make `prompt_version` field optional in the Rust struct or provide a default value.

### DealStudio Organization - Active Issues
Most recent issues (last 24 hours):
1. **DEALSTUDIO-1S**: Connection closed error (3 events)
2. **DEALSTUDIO-1M**: GHL API 400 Bad Request (15 events, assigned to Graeme)
3. **DEALSTUDIO-1P**: Unable to load buyers (8 events)
4. **DEALSTUDIO-1N**: GHL API pagination error (8 events)

**Pattern**: Multiple issues related to `/listing/[projectId]/pages/overview` route and GHL API integration.

## Security Observations
Ceedar organization shows numerous automated security scans attempting to access:
- PHPUnit vulnerability paths
- Configuration files (.env, config.js)
- AWS credentials

**Recommendation**: Implement rate limiting or IP blocking for these automated scans.

## Next Steps
1. **Immediate**: Fix the `prompt_version` deserialization error in ceedar backend
2. **Short-term**: Investigate GHL API integration issues in dealstudio
3. **Long-term**: Enable Seer AI analysis for deeper insights

## Links
- Ceedar issues: https://ceedar.sentry.io/issues/?query=is%3Aunresolved
- DealStudio issues: https://dealstudio.sentry.io/issues/?query=is%3Aunresolved