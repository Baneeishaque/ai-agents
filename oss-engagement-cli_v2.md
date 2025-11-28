# OSS Engagement CLI: Operational Protocol

**Version:** 1.0.0
**Classification:** Internal Tooling
**Maintainer:** Senior Technical Lead

## 📋 Executive Summary

The **OSS Engagement CLI** is a Python-based automation utility designed to operationalize the *Strategic Engagement Plan*. It bridges the gap between high-level technical strategy (AI-generated insights) and platform execution (YouTube API).

This tool allows for "Human-in-the-Loop" automation, ensuring that all public-facing advocacy retains a senior level of quality while eliminating the friction of manual navigation and posting.

---

## 🏗 Architecture

The system operates on a simple **Input-Process-Output** model:
1.  **Input:** Video URL + AI-Drafted "Sandwich" Comment.
2.  **Process:** OAuth 2.0 Authentication -> YouTube Data API v3 -> `commentThreads.insert`.
3.  **Output:** Live comment permalink & verified publication status.

---

## ⚙️ Prerequisites

Before deploying the protocol, ensure your environment meets the following standards:

*   **Runtime:** Python 3.8+
*   **Platform:** Google Cloud Platform (GCP) Project
*   **Dependencies:** `google-auth`, `google-api-python-client`

---

## 🚀 Installation & Setup

### Phase 1: Google Cloud Configuration
*This is a one-time infrastructure setup.*

1.  **Create Project:** Navigate to the [Google Cloud Console](https://console.cloud.google.com/) and create a new project (e.g., `oss-advocacy-engine`).
2.  **Enable API:** Go to **APIs & Services > Library**. Search for and enable **YouTube Data API v3**.
3.  **Configure Consent Screen:**
    *   Go to **APIs & Services > OAuth consent screen**.
    *   Select **External** (since you are using a personal @gmail, usually).
    *   Fill in required fields (App Name: `EngagementCLI`, Email).
    *   **Crucial Step:** Add your own email address to the **Test Users** list. This allows the app to run in "Testing" mode without verification by Google.
4.  **Generate Credentials:**
    *   Go to **APIs & Services > Credentials**.
    *   Click **Create Credentials > OAuth client ID**.
    *   Application Type: **Desktop App**.
    *   Download the JSON file, rename it to `client_secret.json`, and place it in the root directory of this repo.

### Phase 2: Local Environment

```bash
# 1. Clone or create the directory
mkdir oss-engagement-cli
cd oss-engagement-cli

# 2. Create a virtual environment (Recommended practice)
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# 3. Install dependencies
pip install google-auth google-auth-oauthlib google-auth-httplib2 google-api-python-client
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
1.  Analyze the transcript/content.
2.  Apply the "Deep Scan" logic.
3.  Generate a drafted "Sandwich Method" comment.

### Step 3: The Command Generation
Ask the AI to **"Format for CLI"**. The AI will output the ready-to-paste code block:

```bash
python engagement_cli.py --url "https://youtube.com/..." --comment "Your drafted text here..."
```

### Step 4: Execution
1.  Copy the code block from the chat.
2.  Paste it into your terminal.
3.  Review the output one last time.
4.  Press `Y` to confirm.

**Result:** You have engaged with high-level technical authority in <30 seconds.

---

## 💻 Usage Protocol (Manual)

If you are running the tool manually without AI assistance:

### Command Syntax
```bash
python engagement_cli.py --url "[VIDEO_URL]" --comment "[YOUR_COMMENT_TEXT]"
```

### Example Operation
```bash
python engagement_cli.py \
  --url "https://www.youtube.com/watch?v=example" \
  --comment "Excellent breakdown of the N+1 query problem. In my 12 years of backend work, I've found that tools like OpenTelemetry provide much better visibility into these bottlenecks than standard logging. Would love to see a deep dive on OTel next!"
```

---

## 🔐 Security & Compliance

*   **Credential Management:** The `client_secret.json` file contains sensitive OAuth keys. **Do not commit this file to public repositories (GitHub, GitLab, etc.).** Add it to your `.gitignore`.
*   **Token Scope:** This tool requests the `youtube.force-ssl` scope. It creates comments *as you*. Review all payloads before hitting "Confirm."
*   **Rate Limiting:** The YouTube Data API has a daily quota. This tool uses very little, but be mindful if scaling to hundreds of comments per day (not recommended per strategy).

---

## 🛠 Troubleshooting

| Error Code | Root Cause | Resolution |
| :--- | :--- | :--- |
| `403 Forbidden` | API not enabled | Ensure YouTube Data API v3 is enabled in GCP. |
| `400 Bad Request` | Invalid URL | Ensure the URL is a standard YouTube link. |
| `access_denied` | Test User missing | Add your email to "Test Users" in OAuth Consent Screen. |

---

*Establish Authority. Build the Ecosystem. Automate the Rest.*