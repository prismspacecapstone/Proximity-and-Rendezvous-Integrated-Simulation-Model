# PRISM File Storage Browser

This is a themed file browser page that allows users to view and download files from a `/storage` folder with support for subfolders.

## Setup Instructions for Static Websites

### 1. Create Your Folder Structure
```
your-website/
├── index.html
├── storage.html
├── storage.js
└── storage/              ← Main storage folder
    ├── files.json        ← List of files/folders (REQUIRED)
    ├── file1.pdf         ← Root level files
    ├── file2.txt
    ├── documents/        ← Subfolder example
    │   ├── files.json    ← Each subfolder needs its own files.json
    │   ├── doc1.pdf
    │   └── doc2.docx
    └── images/           ← Another subfolder
        ├── files.json
        ├── photo1.jpg
        └── photo2.png
```

### 2. Create the Main files.json File
Inside the `storage` folder, create `files.json` that lists your files AND folders:

```json
{
  "files": [
    {
      "filename": "Mission-Report-2025.pdf",
      "name": "Mission Report 2025",
      "description": "Annual mission report"
    },
    {
      "filename": "readme.txt",
      "name": "README",
      "description": "Getting started guide"
    }
  ],
  "folders": [
    {
      "foldername": "documents",
      "name": "Documents",
      "description": "All project documents"
    },
    {
      "foldername": "images",
      "name": "Image Gallery",
      "description": "Photos and graphics"
    }
  ]
}
```

### 3. Create files.json for Each Subfolder
**For `storage/documents/files.json`:**
```json
{
  "files": [
    {
      "filename": "technical-spec.pdf",
      "name": "Technical Specifications"
    },
    {
      "filename": "requirements.docx",
      "name": "Requirements Doc"
    }
  ],
  "folders": []
}
```

**For `storage/images/files.json`:**
```json
{
  "files": [
    {
      "filename": "team-photo.jpg",
      "name": "Team Photo 2025"
    },
    {
      "filename": "logo.png",
      "name": "Company Logo"
    }
  ],
  "folders": []
}
```

### 4. Field Explanations

**For Files:**
- `filename` - The actual file name in the folder (REQUIRED)
- `name` - Display name shown on the page (optional)
- `description` - Text shown under the file name (optional)

**For Folders:**
- `foldername` - The actual folder name (REQUIRED, must match the folder on your server)
- `name` - Display name shown on the page (optional)
- `description` - Text shown under the folder name (optional)

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

**Example 1: Files only (no subfolders)**
```
storage/
├── files.json
├── report.pdf
└── image.png
```

Your `files.json`:
```json
{
  "files": [
    {
      "filename": "report.pdf",
      "name": "Annual Report"
    },
    {
      "filename": "image.png",
      "name": "Company Logo"
    }
  ],
  "folders": []
}
```

**Example 2: With subfolders**
```
storage/
├── files.json
├── readme.txt
├── documents/
│   ├── files.json
│   └── spec.pdf
└── photos/
    ├── files.json
    └── team.jpg
```

**Main `storage/files.json`:**
```json
{
  "files": [
    {
      "filename": "readme.txt",
      "name": "README"
    }
  ],
  "folders": [
    {
      "foldername": "documents",
      "name": "Documents"
    },
    {
      "foldername": "photos",
      "name": "Photos"
    }
  ]
}
```

**`storage/documents/files.json`:**
```json
{
  "files": [
    {
      "filename": "spec.pdf",
      "name": "Specifications"
    }
  ],
  "folders": []
}
```

**`storage/photos/files.json`:**
```json
{
  "files": [
    {
      "filename": "team.jpg",
      "name": "Team Photo"
    }
  ],
  "folders": []
}
```

## Updating Files

To add or remove files:
1. Add/remove the actual files from the `storage` folder
2. Update `files.json` to match
3. Re-upload both the files and `files.json`

## Features

- **Folder Navigation** - Browse through subfolders, click folders to open them
- **Breadcrumb Navigation** - See current path (e.g., 📁 /storage/documents)
- **Back Button** - Easy navigation back to parent folders
- **File Viewer** - View file contents directly in a modal popup without downloading
- **Supported preview types:**
  - **Text files** (TXT, MD, JSON, XML, CSV, HTML, CSS, JS) - Read contents directly
  - **Images** (JPG, PNG, GIF, WEBP, SVG) - View full-size images
  - **PDFs** - Read PDFs in browser
  - **Videos** (MP4, WEBM, OGG) - Play videos inline
  - **Audio** (MP3, WAV, OGG) - Play audio files
- **Simple configuration** - Just edit JSON files
- **File type icons** - Automatic icons for files and 📁 for folders
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

**Navigating Folders:**
1. Folders appear with a 📁 icon and yellow/orange glow when you hover
2. Click any folder to open it and see its contents
3. The path at the top shows where you are (e.g., 📁 /storage/documents)
4. Click the ".. (Go Back)" card or the back button to return to the parent folder

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
