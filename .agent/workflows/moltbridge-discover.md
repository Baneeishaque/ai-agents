---
description: moltbridge-discover - Find a broker for social graph introduction
---

# moltbridge-discover Workflow

This workflow executes a formal broker discovery request against the Moltbridge social graph.

## Prerequisite

Ensure the `moltbridge` MCP server is available and configured with correct environment variables.

## Steps

1. **Format Discovery Payload**
Prepare the JSON-RPC tool call payload for `moltbridge_discover_broker`.

// turbo
2. **Execute Broker Discovery**
Pipe the discovery payload into the Moltbridge server using the established industrial setup.

```bash
echo '{"jsonrpc":"2.0","id":"1","method":"tools/call","params":{"name":"moltbridge_discover_broker",
"arguments":{"target":"<TARGET_NAME>","max_hops":4,"max_results":3}}}' | \
MOLTBRIDGE_AGENT_ID="mb-mlo60ny1-5grl" \
MOLTBRIDGE_SIGNING_KEY="8ec561b7aa273bf78bf72fcaf35ea28319734801afc83c9da0a12060ad49e683" \
MOLTBRIDGE_BASE_URL="https://api.moltbridge.ai" \
mise exec node -- npx -y moltbridge serve
```

1. **Verify Results**
Examine the `candidates` array for trust scores and path information. Refer to
[Moltbridge Discovery Rules](../ai-agent-rules/moltbridge-discovery-rules.md) for trust threshold mandates.
