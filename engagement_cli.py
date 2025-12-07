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

def get_comments(youtube, video_id, fetch_all=False):
    """Fetches comments from a video. Defaults to top 20, or all if requested."""
    print(f"🔍 Scanning {'ALL' if fetch_all else 'top'} comments for context...")
    comments = []
    
    try:
        request = youtube.commentThreads().list(
            part="snippet",
            videoId=video_id,
            maxResults=100 if fetch_all else 20,
            order="relevance"
        )
        
        while request:
            response = request.execute()
            
            for item in response.get('items', []):
                snippet = item['snippet']['topLevelComment']['snippet']
                comments.append({
                    'author': snippet['authorDisplayName'],
                    'text': snippet['textOriginal'],
                    'likes': snippet['likeCount']
                })
            
            # If we only want a quick scan, stop after first page
            if not fetch_all:
                break
                
            # Check for next page
            if 'nextPageToken' in response:
                request = youtube.commentThreads().list_next(request, response)
            else:
                break
                
            print(f"   ...fetched {len(comments)} comments so far...")

        print(f"✅ Total comments fetched: {len(comments)}")
        
        for i, c in enumerate(comments, 1):
            clean_text = c['text'].replace('\n', ' ')
            print(f"[{i}] {c['author']} (👍 {c['likes']}): {clean_text}")
            
        return comments
        
    except googleapiclient.errors.HttpError as e:
    parser.add_argument('--all', action='store_true', help='Fetch ALL comments (use with caution)')
    
    args = parser.parse_args()

    print("🚀 Initializing Engagement Protocol...")
    youtube = get_authenticated_service()
    
    vid_id = extract_video_id(args.url)
    
    if args.scan:
        get_comments(youtube, vid_id, fetch_all=args.all
    else:
        return url

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Engagement Automation Tool')
    parser.add_argument('--url', required=True, help='YouTube Video URL')
    parser.add_argument('--comment', help='The text of the comment to post')
    parser.add_argument('--scan', action='store_true', help='Scan existing comments for context')
    
    args = parser.parse_args()

    print("🚀 Initializing Engagement Protocol...")
    youtube = get_authenticated_service()
    
    vid_id = extract_video_id(args.url)
    
    if args.scan:
        get_comments(youtube, vid_id)
        
    if args.comment:
        print(f"📝 Posting to Video ID: {vid_id}")
        print(f"💬 Content: \n---\n{args.comment}\n---")
        
        confirm = input("Confirm posting? (y/n): ")
        if confirm.lower() == 'y':
            post_comment(youtube, vid_id, args.comment)
        else:
            print("🛑 Operation cancelled.")
    elif not args.scan:
        print("⚠️ No action specified. Use --scan to read comments or --comment to post.")