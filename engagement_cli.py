import os
import sys
import argparse
import google_auth_oauthlib.flow
import googleapiclient.discovery
import googleapiclient.errors

# Scopes required for posting comments
SCOPES = ["https://www.googleapis.com/auth/youtube.force-ssl"]

def get_authenticated_service():
    """Authenticates the user with YouTube."""
    # Disable OAuthlib's HTTPS verification when running locally.
    # *DO NOT* leave this in production if deploying to a server.
    os.environ["OAUTHLIB_INSECURE_TRANSPORT"] = "1"

    api_service_name = "youtube"
    api_version = "v3"
    client_secrets_file = "client_secret.json" # YOU NEED THIS FILE FROM GOOGLE

    # Get credentials and create an API client
    flow = google_auth_oauthlib.flow.InstalledAppFlow.from_client_secrets_file(
        client_secrets_file, SCOPES)
    credentials = flow.run_local_server(port=0)
    
    youtube = googleapiclient.discovery.build(
        api_service_name, api_version, credentials=credentials)

    return youtube

def post_comment(youtube, video_id, comment_text):
    """Posts a comment to a specific video."""
    try:
        request = youtube.commentThreads().insert(
            part="snippet",
            body={
                "snippet": {
                    "videoId": video_id,
                    "topLevelComment": {
                        "snippet": {
                            "textOriginal": comment_text
                        }
                    }
                }
            }
        )
        response = request.execute()
        print(f"✅ Success! Comment posted.")
        print(f"🔗 Link: https://www.youtube.com/watch?v={video_id}&lc={response['id']}")
        return response
    except googleapiclient.errors.HttpError as e:
        print(f"❌ An error occurred: {e}")
        return None

def extract_video_id(url):
    """Extracts video ID from a standard YouTube URL."""
    # Basic extraction logic (can be improved with regex)
    if "v=" in url:
        return url.split("v=")[1].split("&")[0]
    elif "youtu.be" in url:
        return url.split("/")[-1]
    else:
        return url

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Engagement Automation Tool')
    parser.add_argument('--url', required=True, help='YouTube Video URL')
    parser.add_argument('--comment', required=True, help='The text of the comment to post')
    
    args = parser.parse_args()

    print("🚀 Initializing Engagement Protocol...")
    youtube = get_authenticated_service()
    
    vid_id = extract_video_id(args.url)
    
    print(f"📝 Posting to Video ID: {vid_id}")
    print(f"💬 Content: \n---\n{args.comment}\n---")
    
    confirm = input("Confirm posting? (y/n): ")
    if confirm.lower() == 'y':
        post_comment(youtube, vid_id, args.comment)
    else:
        print("🛑 Operation cancelled.")