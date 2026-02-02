// Create stars background
function createStars() {
    const starsContainer = document.querySelector('.stars');
    const starCount = 60;
    
    for (let i = 0; i < starCount; i++) {
        const star = document.createElement('div');
        star.className = 'star';
        
        const size = Math.random() > 0.9 ? Math.random() * 2 + 2 : Math.random() * 1.5 + 0.5;
        star.style.width = size + 'px';
        star.style.height = size + 'px';
        
        star.style.left = Math.random() * 100 + '%';
        star.style.top = Math.random() * 100 + '%';
        star.style.animationDelay = Math.random() * 4 + 's';
        star.style.animationDuration = (Math.random() * 3 + 3) + 's';
        
        starsContainer.appendChild(star);
    }
}

// Custom cursor
const cursor = document.querySelector('.custom-cursor');
document.addEventListener('mousemove', (e) => {
    cursor.style.left = e.clientX - 8 + 'px';
    cursor.style.top = e.clientY - 8 + 'px';
});

// Get file icon based on extension
function getFileIcon(filename) {
    const ext = filename.split('.').pop().toLowerCase();
    const icons = {
        'pdf': '📄',
        'doc': '📝',
        'docx': '📝',
        'txt': '📃',
        'jpg': '🖼️',
        'jpeg': '🖼️',
        'png': '🖼️',
        'gif': '🖼️',
        'mp4': '🎬',
        'mp3': '🎵',
        'zip': '📦',
        'rar': '📦',
        'exe': '⚙️',
        'js': '💻',
        'html': '🌐',
        'css': '🎨',
        'json': '📋',
        'xml': '📋',
        'csv': '📊',
        'xls': '📊',
        'xlsx': '📊',
        'ppt': '📊',
        'pptx': '📊'
    };
    return icons[ext] || '📄';
}

// Format file size
function formatFileSize(bytes) {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return Math.round(bytes / Math.pow(k, i) * 100) / 100 + ' ' + sizes[i];
}

// Current path tracking
let currentPath = '';

// Load files from storage folder
async function loadFiles(path = '') {
    const filesGrid = document.getElementById('filesGrid');
    const fileCount = document.getElementById('fileCount');
    const folderPath = document.querySelector('.folder-path');
    
    currentPath = path;
    
    try {
        // For static sites: load files list from files.json
        const jsonPath = path ? `storage/${path}/files.json` : 'storage/files.json';
        const response = await fetch(jsonPath);
        
        if (!response.ok) {
            throw new Error('files.json not found');
        }
        
        const data = await response.json();
        const files = data.files || [];
        const folders = data.folders || [];
        
        // Update folder path display
        if (path) {
            folderPath.innerHTML = `📁 /storage/${path}`;
        } else {
            folderPath.innerHTML = `📁 /storage`;
        }
        
        if (files.length === 0 && folders.length === 0) {
            filesGrid.innerHTML = `
                <div class="empty-state" style="grid-column: 1 / -1;">
                    <div class="empty-icon">📭</div>
                    <p>No files or folders found</p>
                    ${path ? `<button class="action-btn" onclick="loadFiles('')" style="margin-top: 1rem;">← Back to Main</button>` : ''}
                </div>
            `;
            fileCount.textContent = '0 items';
            return;
        }
        
        const totalItems = files.length + folders.length;
        fileCount.textContent = `${totalItems} item${totalItems !== 1 ? 's' : ''}`;
        
        // Create back button if in subfolder
        let backButton = '';
        if (path) {
            const parentPath = path.split('/').slice(0, -1).join('/');
            backButton = `
                <div class="file-card folder-card" onclick="loadFiles('${parentPath}')">
                    <div class="file-icon">📁</div>
                    <div class="file-name">.. (Go Back)</div>
                    <div class="file-info">Return to parent folder</div>
                </div>
            `;
        }
        
        // Create folder cards
        const folderCards = folders.map(folder => {
            const folderFullPath = path ? `${path}/${folder.foldername}` : folder.foldername;
            const displayName = folder.name || folder.foldername;
            const description = folder.description || 'Click to open folder';
            
            return `
                <div class="file-card folder-card" onclick="loadFiles('${folderFullPath}')">
                    <div class="file-icon">📁</div>
                    <div class="file-name">${displayName}</div>
                    <div class="file-info">${description}</div>
                </div>
            `;
        }).join('');
        
        // Create file cards
        const fileCards = files.map((file, index) => {
            const filePath = path ? `storage/${path}/${file.filename}` : `storage/${file.filename}`;
            const displayName = file.name || file.filename;
            const description = file.description || 'Click to view or download';
            
            return `
                <div class="file-card">
                    <div class="file-icon">${getFileIcon(file.filename)}</div>
                    <div class="file-name">${displayName}</div>
                    <div class="file-info">${description}</div>
                    <div class="file-actions">
                        <button class="action-btn" onclick="viewFile('${filePath}', '${displayName.replace(/'/g, "\\'")}')">VIEW</button>
                        <a href="${filePath}" download class="action-btn download">DOWNLOAD</a>
                    </div>
                </div>
            `;
        }).join('');
        
        filesGrid.innerHTML = backButton + folderCards + fileCards;
        
    } catch (error) {
        console.error('Error loading files:', error);
        
        filesGrid.innerHTML = `
            <div class="empty-state" style="grid-column: 1 / -1;">
                <div class="empty-icon">⚠️</div>
                <p>Could not load file list</p>
                <p style="margin-top: 1rem; font-size: 0.9rem;">
                    Make sure storage/files.json exists and is properly formatted
                </p>
                ${path ? `<button class="action-btn" onclick="loadFiles('')" style="margin-top: 1rem;">← Back to Main</button>` : ''}
            </div>
        `;
        fileCount.textContent = 'Error loading files';
    }
}

// Initialize
createStars();
loadFiles();

// File viewer functions
async function viewFile(filePath, fileName) {
    const modal = document.getElementById('fileModal');
    const modalTitle = document.getElementById('modalTitle');
    const modalBody = document.getElementById('modalBody');
    const modalDownload = document.getElementById('modalDownload');
    
    modalTitle.textContent = fileName;
    modalDownload.href = filePath;
    modalDownload.download = fileName;
    
    const extension = filePath.split('.').pop().toLowerCase();
    
    try {
        // Text files - read and display content
        if (['txt', 'md', 'json', 'xml', 'csv', 'html', 'css', 'js'].includes(extension)) {
            const response = await fetch(filePath);
            const text = await response.text();
            modalBody.innerHTML = `<pre>${escapeHtml(text)}</pre>`;
        }
        // Images - display directly
        else if (['jpg', 'jpeg', 'png', 'gif', 'webp', 'svg'].includes(extension)) {
            modalBody.innerHTML = `<img src="${filePath}" alt="${fileName}">`;
        }
        // PDFs - use iframe
        else if (extension === 'pdf') {
            modalBody.innerHTML = `<iframe src="${filePath}"></iframe>`;
        }
        // Videos
        else if (['mp4', 'webm', 'ogg'].includes(extension)) {
            modalBody.innerHTML = `
                <video controls style="width: 100%; border-radius: 8px;">
                    <source src="${filePath}" type="video/${extension}">
                    Your browser does not support the video tag.
                </video>
            `;
        }
        // Audio
        else if (['mp3', 'wav', 'ogg'].includes(extension)) {
            modalBody.innerHTML = `
                <audio controls style="width: 100%;">
                    <source src="${filePath}" type="audio/${extension}">
                    Your browser does not support the audio tag.
                </audio>
            `;
        }
        // Other files - show message
        else {
            modalBody.innerHTML = `
                <p style="text-align: center; padding: 3rem;">
                    <span style="font-size: 3rem; display: block; margin-bottom: 1rem;">📄</span>
                    This file type cannot be previewed.<br>
                    Please use the download button to save it.
                </p>
            `;
        }
        
        modal.classList.add('active');
        
    } catch (error) {
        console.error('Error loading file:', error);
        modalBody.innerHTML = `
            <p style="text-align: center; padding: 3rem; color: rgba(255, 100, 100, 0.8);">
                <span style="font-size: 3rem; display: block; margin-bottom: 1rem;">⚠️</span>
                Error loading file.<br>
                Please try downloading it instead.
            </p>
        `;
        modal.classList.add('active');
    }
}

function closeModal() {
    const modal = document.getElementById('fileModal');
    modal.classList.remove('active');
}

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Close modal when clicking outside
document.getElementById('fileModal').addEventListener('click', function(e) {
    if (e.target === this) {
        closeModal();
    }
});

// Close modal with Escape key
document.addEventListener('keydown', function(e) {
    if (e.key === 'Escape') {
        closeModal();
    }
});
