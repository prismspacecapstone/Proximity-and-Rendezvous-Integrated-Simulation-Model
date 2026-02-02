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
        // Get list of files in storage folder
        const response = await fetch('/storage/');
        
        if (!response.ok) {
            throw new Error('Storage folder not accessible');
        }
        
        const html = await response.text();
        
        // Parse directory listing (this works with basic HTTP servers)
        const parser = new DOMParser();
        const doc = parser.parseFromString(html, 'text/html');
        const links = doc.querySelectorAll('a');
        
        const files = [];
        links.forEach(link => {
            const href = link.getAttribute('href');
            if (href && href !== '../' && !href.startsWith('/') && !href.startsWith('?')) {
                files.push({
                    name: decodeURIComponent(href),
                    path: '/storage/' + href
                });
            }
        });
        
        if (files.length === 0) {
            filesGrid.innerHTML = `
                <div class="empty-state" style="grid-column: 1 / -1;">
                    <div class="empty-icon">📭</div>
                    <p>No files found in storage folder</p>
                </div>
            `;
            fileCount.textContent = '0 files';
            return;
        }
        
        fileCount.textContent = `${files.length} file${files.length !== 1 ? 's' : ''}`;
        
        // Create file cards
        filesGrid.innerHTML = files.map(file => `
            <div class="file-card">
                <div class="file-icon">${getFileIcon(file.name)}</div>
                <div class="file-name">${file.name}</div>
                <div class="file-info">Click to view or download</div>
                <div class="file-actions">
                    <a href="${file.path}" target="_blank" class="action-btn">VIEW</a>
                    <a href="${file.path}" download class="action-btn download">DOWNLOAD</a>
                </div>
            </div>
        `).join('');
        
    } catch (error) {
        console.error('Error loading files:', error);
        
        // Fallback: Show manual file list if directory listing fails
        // You can manually add files here
        const manualFiles = [
            // Add your files here, for example:
            // { name: 'example.pdf', path: '/storage/example.pdf' },
            // { name: 'document.docx', path: '/storage/document.docx' }
        ];
        
        if (manualFiles.length > 0) {
            fileCount.textContent = `${manualFiles.length} file${manualFiles.length !== 1 ? 's' : ''}`;
            filesGrid.innerHTML = manualFiles.map(file => `
                <div class="file-card">
                    <div class="file-icon">${getFileIcon(file.name)}</div>
                    <div class="file-name">${file.name}</div>
                    <div class="file-info">Click to view or download</div>
                    <div class="file-actions">
                        <a href="${file.path}" target="_blank" class="action-btn">VIEW</a>
                        <a href="${file.path}" download class="action-btn download">DOWNLOAD</a>
                    </div>
                </div>
            `).join('');
        } else {
            filesGrid.innerHTML = `
                <div class="empty-state" style="grid-column: 1 / -1;">
                    <div class="empty-icon">📁</div>
                    <p>Storage folder is empty or inaccessible</p>
                    <p style="margin-top: 1rem; font-size: 0.9rem;">
                        Add files to the /storage folder or configure file list in storage.js
                    </p>
                </div>
            `;
            fileCount.textContent = 'No files available';
        }
    }
}

// Initialize
createStars();
loadFiles();
