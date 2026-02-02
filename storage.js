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

// Load files from storage folder
async function loadFiles() {
    const filesGrid = document.getElementById('filesGrid');
    const fileCount = document.getElementById('fileCount');
    
    try {
        // For static sites: load files list from files.json
        const response = await fetch('storage/files.json');
        
        if (!response.ok) {
            throw new Error('files.json not found');
        }
        
        const data = await response.json();
        const files = data.files || [];
        
        if (files.length === 0) {
            filesGrid.innerHTML = `
                <div class="empty-state" style="grid-column: 1 / -1;">
                    <div class="empty-icon">📭</div>
                    <p>No files found in storage folder</p>
                    <p style="margin-top: 1rem; font-size: 0.9rem;">
                        Add files to storage/files.json
                    </p>
                </div>
            `;
            fileCount.textContent = '0 files';
            return;
        }
        
        fileCount.textContent = `${files.length} file${files.length !== 1 ? 's' : ''}`;
        
        // Create file cards
        filesGrid.innerHTML = files.map(file => {
            const filePath = `storage/${file.filename}`;
            const displayName = file.name || file.filename;
            const description = file.description || 'Click to view or download';
            
            return `
                <div class="file-card">
                    <div class="file-icon">${getFileIcon(file.filename)}</div>
                    <div class="file-name">${displayName}</div>
                    <div class="file-info">${description}</div>
                    <div class="file-actions">
                        <a href="${filePath}" target="_blank" class="action-btn">VIEW</a>
                        <a href="${filePath}" download class="action-btn download">DOWNLOAD</a>
                    </div>
                </div>
            `;
        }).join('');
        
    } catch (error) {
        console.error('Error loading files:', error);
        
        filesGrid.innerHTML = `
            <div class="empty-state" style="grid-column: 1 / -1;">
                <div class="empty-icon">⚠️</div>
                <p>Could not load file list</p>
                <p style="margin-top: 1rem; font-size: 0.9rem;">
                    Make sure storage/files.json exists and is properly formatted
                </p>
            </div>
        `;
        fileCount.textContent = 'Error loading files';
    }
}

// Initialize
createStars();
loadFiles();
