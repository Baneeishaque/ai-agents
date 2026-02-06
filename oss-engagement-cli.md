# OSS Engagement CLI: Complete Operational Protocol

**Version:** 2.0.0  
**Classification:** Internal Tooling  
**Maintainer:** Senior Technical Lead  
**Source File:** `engagement_cli.py`

---

## 📋 Executive Summary

The **OSS Engagement CLI** (`engagement_cli.py`) is a Python-based automation utility designed to operationalize the *Strategic Engagement Plan*. It bridges the gap between high-level technical strategy (AI-generated insights) and platform execution (YouTube API).

This tool allows for "Human-in-the-Loop" automation, ensuring that all public-facing advocacy retains a senior level of quality while eliminating the friction of manual navigation and posting.

**Key Features:**
- OAuth 2.0 authentication with YouTube
- Automated comment posting to YouTube videos
- Multiple URL format support (standard and short URLs)
- User confirmation before posting
- Direct comment permalink generation

---

## 🏗 Architecture

### System Overview

The system operates on a simple **Input-Process-Output** model:

1. **Input:** Video URL + AI-Drafted "Sandwich" Comment (via CLI arguments)
2. **Process:** OAuth 2.0 Authentication → YouTube Data API v3 → `commentThreads.insert`
3. **Output:** Live comment permalink & verified publication status

### Code Structure

```
engagement_cli.py
├── SCOPES                      # API permission scope
├── get_authenticated_service() # YouTube API authentication
├── post_comment()              # Comment posting logic
├── extract_video_id()          # URL parsing utility
└── __main__                    # CLI argument handling & execution
```

### Execution Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                        User Input                                │
│              (--url "VIDEO_URL" --comment "TEXT")               │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                   Argument Parsing (argparse)                    │
│               Extract video URL and comment text                 │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│               get_authenticated_service()                        │
│   • Reads client_secret.json                                    │
│   • Initiates OAuth 2.0 flow                                    │
│   • Opens browser for authentication                            │
│   • Returns authenticated YouTube API client                    │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                   extract_video_id()                             │
│   • Parses video ID from URL                                    │
│   • Supports: youtube.com/watch?v=ID, youtu.be/ID               │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                   User Confirmation                              │
│              "Confirm posting? (y/n)"                           │
└─────────────────────────────────────────────────────────────────┘
                              │
                     ┌────────┴────────┐
                     │                 │
                     ▼                 ▼
              ┌───────────┐     ┌───────────────┐
              │   "y"     │     │   "n"/other   │
              └───────────┘     └───────────────┘
                     │                 │
                     ▼                 ▼
┌─────────────────────────┐   ┌───────────────────┐
│     post_comment()      │   │ "Operation        │
│  • commentThreads.insert│   │  cancelled."      │
│  • Returns permalink    │   └───────────────────┘
└─────────────────────────┘
```

---

## ⚙️ Prerequisites

Before deploying the protocol, ensure your environment meets the following standards:

| Requirement | Specification |
|:---|:---|
| **Runtime** | Python 3.8+ |
| **Platform** | Google Cloud Platform (GCP) Project |
| **Dependencies** | `google-auth`, `google-auth-oauthlib`, `google-auth-httplib2`, `google-api-python-client` |
| **Credentials** | `client_secret.json` (OAuth 2.0 Desktop App credentials from GCP) |

---

## 🚀 Installation & Setup

### Phase 1: Google Cloud Configuration

*This is a one-time infrastructure setup.*

1. **Create Project:**  
   Navigate to the [Google Cloud Console](https://console.cloud.google.com/) and create a new project (e.g., `oss-advocacy-engine`).

2. **Enable API:**  
   Go to **APIs & Services > Library**. Search for and enable **YouTube Data API v3**.

3. **Configure Consent Screen:**
   - Go to **APIs & Services > OAuth consent screen**
   - Select **External** (since you are using a personal @gmail account)
   - Fill in required fields:
     - App Name: `EngagementCLI`
     - User support email: Your email
   - **⚠️ Crucial Step:** Add your own email address to the **Test Users** list. This allows the app to run in "Testing" mode without verification by Google.

4. **Generate Credentials:**
   - Go to **APIs & Services > Credentials**
   - Click **Create Credentials > OAuth client ID**
   - Application Type: **Desktop App**
   - Download the JSON file, rename it to `client_secret.json`, and place it in the root directory of this repo.

### Phase 2: Local Environment Setup

```bash
# 1. Clone or navigate to the repository directory containing engagement_cli.py
# (Replace <repository-directory> with your actual repository path)
cd <repository-directory>

# 2. Create a virtual environment (Recommended practice)
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# 3. Install dependencies
pip install google-auth google-auth-oauthlib google-auth-httplib2 google-api-python-client
```

### Phase 3: Credential Placement

Ensure `client_secret.json` is placed in the same directory as `engagement_cli.py`:

```
<repository-directory>/
├── engagement_cli.py
├── client_secret.json    ← Place here (DO NOT commit!)
└── ...
```

---

## 🤖 Integration Workflow: The AI-Human Loop

This is how you combine your AI Agent (Copilot) with this CLI tool for maximum velocity.

### Step 1: The Trigger
You find a high-potential video (e.g., a "VS Code vs Fleet" comparison).

### Step 2: The AI Analysis
Paste the link to your AI Agent with the prompt:
> *"Run the Engagement Protocol on this video."*

The AI will:
1. Analyze the transcript/content
2. Apply the "Deep Scan" logic
3. Generate a drafted "Sandwich Method" comment

### Step 3: The Command Generation
Ask the AI to **"Format for CLI"**. The AI will output the ready-to-paste code block:

```bash
python engagement_cli.py --url "https://youtube.com/..." --comment "Your drafted text here..."
```

### Step 4: Execution
1. Copy the code block from the chat
2. Paste it into your terminal
3. Review the output one last time
4. Press `y` to confirm

**Result:** You have engaged with high-level technical authority in <30 seconds.

---

## 💻 Usage Protocol

### Command Syntax

```bash
python engagement_cli.py --url "[VIDEO_URL]" --comment "[YOUR_COMMENT_TEXT]"
```

### Command-Line Arguments

| Argument | Required | Description |
|:---|:---:|:---|
| `--url` | ✅ | YouTube Video URL (supports standard and short URLs) |
| `--comment` | ✅ | The text of the comment to post |

### Supported URL Formats

The CLI supports multiple YouTube URL formats:

| Format | Example |
|:---|:---|
| Standard | `https://www.youtube.com/watch?v=dQw4w9WgXcQ` |
| With parameters | `https://www.youtube.com/watch?v=dQw4w9WgXcQ&t=120` |
| Short URL | `https://youtu.be/dQw4w9WgXcQ` |
| Direct ID | `dQw4w9WgXcQ` |

### Example Operations

#### Basic Usage
```bash
python engagement_cli.py \
  --url "https://www.youtube.com/watch?v=example" \
  --comment "Great video!"
```

#### Full "Sandwich Method" Comment
```bash
python engagement_cli.py \
  --url "https://www.youtube.com/watch?v=example" \
  --comment "Excellent breakdown of the N+1 query problem. In my 12 years of backend work, I've found that tools like OpenTelemetry provide much better visibility into these bottlenecks than standard logging. Would love to see a deep dive on OTel next!"
```

### Expected Output

```
🚀 Initializing Engagement Protocol...
📝 Posting to Video ID: example123
💬 Content: 
---
Your comment text here...
---
Confirm posting? (y/n): y
✅ Success! Comment posted.
🔗 Link: https://www.youtube.com/watch?v=example123&lc=commentId123
```

---

## 📚 API Reference

### Functions

#### `get_authenticated_service()`

Authenticates the user with YouTube and returns an authenticated API client.

**Returns:** `googleapiclient.discovery.Resource` - Authenticated YouTube API client

**Behavior:**
- Reads OAuth credentials from `client_secret.json`
- Opens a local browser window for OAuth consent
- Creates and returns a YouTube Data API v3 client

**Note:** Sets `OAUTHLIB_INSECURE_TRANSPORT=1` for local development. Remove this in production.

---

#### `post_comment(youtube, video_id, comment_text)`

Posts a comment to a specific YouTube video.

**Parameters:**
| Name | Type | Description |
|:---|:---|:---|
| `youtube` | `Resource` | Authenticated YouTube API client |
| `video_id` | `str` | The ID of the target video |
| `comment_text` | `str` | The content of the comment to post |

**Returns:** 
- `dict` - API response containing the comment ID on success
- `None` - On failure

**Success Output:**
```
✅ Success! Comment posted.
🔗 Link: https://www.youtube.com/watch?v={video_id}&lc={comment_id}
```

---

#### `extract_video_id(url)`

Extracts the video ID from a YouTube URL.

**Parameters:**
| Name | Type | Description |
|:---|:---|:---|
| `url` | `str` | YouTube video URL or direct video ID |

**Returns:** `str` - The extracted video ID

**Supported Patterns:**
- `youtube.com/watch?v=VIDEO_ID` → extracts `VIDEO_ID`
- `youtu.be/VIDEO_ID` → extracts `VIDEO_ID`
- `VIDEO_ID` (direct) → returns as-is

---

## 🔐 Security & Compliance

### Credential Management

| Item | Recommendation |
|:---|:---|
| `client_secret.json` | **⚠️ NEVER commit to version control.** Add to `.gitignore` |
| OAuth tokens | Stored locally after first authentication |
| API Key | Not required; OAuth 2.0 handles authentication |

**Recommended `.gitignore` additions:**
```
client_secret.json
*.pickle
token.json
```

### Token Scope

This tool requests the `youtube.force-ssl` scope, which allows:
- Creating comments as the authenticated user
- Reading public video data

**⚠️ Important:** Review all comment content before confirming. The comment will be posted *as you*.

### Rate Limiting

The YouTube Data API has a daily quota:
- Default: 10,000 units/day
- Comment insert: ~50 units per request
- Practical limit: ~200 comments/day

**Best Practice:** Strategic engagement (quality over quantity) naturally stays well under quota limits.

### Development Mode Warning

The current implementation in `engagement_cli.py` (line 15) includes:
```python
os.environ["OAUTHLIB_INSECURE_TRANSPORT"] = "1"
```

This disables HTTPS verification for local development. **For production deployment:**
1. Remove or comment out this line from `get_authenticated_service()` function
2. Ensure your server has proper HTTPS configuration
3. Use environment-based configuration to control this setting:
   ```python
   if os.environ.get("ENVIRONMENT") != "production":
       os.environ["OAUTHLIB_INSECURE_TRANSPORT"] = "1"
   ```

---

## 🛠 Troubleshooting

### Common Errors

| Error | Root Cause | Resolution |
|:---|:---|:---|
| `403 Forbidden` | API not enabled | Enable YouTube Data API v3 in GCP Console |
| `400 Bad Request` | Invalid URL | Ensure the URL is a valid YouTube link |
| `access_denied` | Test User missing | Add your email to "Test Users" in OAuth Consent Screen |
| `FileNotFoundError` | Missing credentials | Ensure `client_secret.json` is in the same directory |
| `HttpError 403: commentsDisabled` | Comments disabled on video | Choose a different video |
| `HttpError 429: quotaExceeded` | Daily API quota reached | Wait until quota resets (Pacific midnight) |

### Debug Mode

For debugging, you can add verbose logging by modifying the script:

```python
import logging
logging.basicConfig(level=logging.DEBUG)
```

### Re-authentication

If you need to re-authenticate (e.g., changed Google accounts):
1. Delete any cached token files (`.pickle` or `token.json`)
2. Run the script again
3. Complete the OAuth flow in the browser

---

## 🔄 Version History

| Version | Changes |
|:---|:---|
| 2.0.0 | Unified documentation from v1 and v2; added API reference |
| 1.0.0 | Initial documentation with basic usage |

---

## 📖 Related Documents

The following documents are available in this repository:

| Document | Description |
|:---|:---|
| `engagement_plan_oss_v1.0.md` | Strategic Engagement Plan v1.0 |
| `engagement_plan_oss_v1.1.md` | Strategic Engagement Plan v1.1 |
| `engagement_plan_oss_v1.2.md` | Strategic Engagement Plan v1.2 |
| `engagement_plan_oss_v1.3_automated.md` | Automated Engagement Plan |
| `oss-engagement-cli_v1.md` | CLI Documentation v1 (superseded by this document) |
| `oss-engagement-cli_v2.md` | CLI Documentation v2 (superseded by this document) |

---

*Establish Authority. Build the Ecosystem. Automate the Rest.*
