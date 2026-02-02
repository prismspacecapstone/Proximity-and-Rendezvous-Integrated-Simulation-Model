# PRISM File Storage Browser

This is a themed file browser page that allows users to view and download files from a `/storage` folder.

## Setup Instructions for Static Websites

### 1. Create Your Folder Structure
```
your-website/
├── index.html
├── storage.html
├── storage.js
└── storage/              ← Create this folder
    ├── files.json        ← List of your files (REQUIRED)
    ├── file1.pdf         ← Your actual files
    ├── file2.docx
    └── file3.jpg
```

### 2. Create the files.json File
Inside the `storage` folder, create a file called `files.json` that lists all your files:

```json
{
  "files": [
    {
      "filename": "Mission-Report-2025.pdf",
      "name": "Mission Report 2025",
      "description": "Annual mission report"
    },
    {
      "filename": "technical-specs.docx",
      "name": "Technical Specifications",
      "description": "System requirements and specs"
    },
    {
      "filename": "team-photo.jpg",
      "name": "Team Photo",
      "description": "Our amazing team"
    }
  ]
}
```

**Field explanation:**
- `filename` - The actual file name in the storage folder (REQUIRED)
- `name` - Display name shown on the page (optional, defaults to filename)
- `description` - Text shown under the file name (optional, defaults to "Click to view or download")

### 3. Add Your Files
Place all your actual files in the `storage` folder alongside `files.json`.

**Important:** The filenames in `files.json` must exactly match your actual file names!

### 4. Upload Everything
Upload the entire structure to your web host:
- Upload `storage.html` and `storage.js` to your root directory
- Upload the `storage` folder with `files.json` and all your files

### 5. Link from Homepage (Optional)
Add a link to your storage page in `index.html`:

```html
<div class="footer-nav">
    <a href="mission.html" class="footer-link">OUR MISSION</a>
    <a href="blog.html" class="footer-link">BLOG</a>
    <a href="storage.html" class="footer-link">FILES</a>
    <a href="contactus.html" class="footer-link">CONTACT US</a>
</div>
```

## Quick Example

If you have these files in your storage folder:
```
storage/
├── files.json
├── report.pdf
└── image.png
```

Your `files.json` should look like:
```json
{
  "files": [
    {
      "filename": "report.pdf",
      "name": "Annual Report",
      "description": "2025 financial report"
    },
    {
      "filename": "image.png",
      "name": "Company Logo"
    }
  ]
}
```

## Updating Files

To add or remove files:
1. Add/remove the actual files from the `storage` folder
2. Update `files.json` to match
3. Re-upload both the files and `files.json`

## Features

- **File Viewer** - View file contents directly in a modal popup without downloading
- **Supported preview types:**
  - **Text files** (TXT, MD, JSON, XML, CSV, HTML, CSS, JS) - Read contents directly
  - **Images** (JPG, PNG, GIF, WEBP, SVG) - View full-size images
  - **PDFs** - Read PDFs in browser
  - **Videos** (MP4, WEBM, OGG) - Play videos inline
  - **Audio** (MP3, WAV, OGG) - Play audio files
- **Simple configuration** - Just edit a JSON file
- **File type icons** - Automatic icons for different file types
- **View & Download** - Each file has buttons to view in modal or download directly
- **PRISM theme** - Matches your existing space-themed design
- **Responsive** - Works on desktop and mobile devices
- **Custom cursor** - Maintains the glowing cursor effect
- **Keyboard shortcuts** - Press ESC to close the viewer

## Supported File Types

The page recognizes and displays icons for:
- Documents: PDF, DOC, DOCX, TXT
- Images: JPG, JPEG, PNG, GIF
- Media: MP4, MP3
- Archives: ZIP, RAR
- Code: JS, HTML, CSS, JSON, XML
- Spreadsheets: CSV, XLS, XLSX
- Presentations: PPT, PPTX

## Troubleshooting

**Files not showing?**
- Check that `files.json` is in the `storage` folder
- Verify `files.json` is valid JSON (use a JSON validator)
- Ensure filenames in `files.json` exactly match actual file names
- Check browser console (F12) for error messages

**Can't view files?**
- Some file types (like .docx) may download instead of viewing - this is normal
- PDFs, images, and text files should view in browser

## How to Use

**Viewing Files:**
1. Click the **VIEW** button on any file
2. A popup modal will open showing the file contents
3. For text files (like .txt), you can read the contents directly
4. For images, you'll see the full image
5. For PDFs, you can scroll through pages
6. Press **ESC** or click outside the modal to close

**Downloading Files:**
1. Click the **DOWNLOAD** button to save the file to your computer
2. Or, while viewing a file in the modal, click **DOWNLOAD** in the footer

**What Gets Previewed:**
- ✅ Text files (.txt, .md, .json, .html, .css, .js, etc.) - Shows content
- ✅ Images (.jpg, .png, .gif, .svg) - Shows the image
- ✅ PDFs - Shows the document
- ✅ Videos (.mp4) - Plays the video
- ✅ Audio (.mp3, .wav) - Plays the audio
- ❌ Other files (.docx, .xlsx, .zip, etc.) - Shows download message

## Security Note

Only place files in `/storage` that you want to be publicly accessible!
