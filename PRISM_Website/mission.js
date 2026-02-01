// Create realistic star field
function createStars() {
    const starsContainer = document.querySelector('.stars');
    const starCount = 150;
    
    for (let i = 0; i < starCount; i++) {
        const star = document.createElement('div');
        star.className = 'star';
        
        const size = Math.random() > 0.85 ? Math.random() * 3 + 2 : Math.random() * 1.5 + 0.5;
        star.style.width = size + 'px';
        star.style.height = size + 'px';
        
        // Different star colors
        const starType = Math.random();
        if (starType < 0.6) {
            star.style.background = '#ffffff';
        } else if (starType < 0.8) {
            star.style.background = '#ffd54f';
        } else {
            star.style.background = '#ff8a65';
        }
        
        const x = Math.random() * 100;
        const y = Math.random() * 100;
        
        star.style.left = x + '%';
        star.style.top = y + '%';
        star.style.animationDelay = Math.random() * 4 + 's';
        star.style.animationDuration = (Math.random() * 4 + 3) + 's';
        
        starsContainer.appendChild(star);
    }
}

// Custom cursor functionality
const cursor = document.querySelector('.custom-cursor');

if (cursor) {
    document.addEventListener('mousemove', (e) => {
        cursor.style.left = e.clientX + 'px';
        cursor.style.top = e.clientY + 'px';
    });
    
    // Cursor hover effects for team members and nav
    document.addEventListener('mouseenter', (e) => {
        if (e.target.closest('.team-member') || e.target.closest('.prism-nav')) {
            cursor.style.transform = 'scale(1.5)';
            cursor.style.boxShadow = '0 0 25px rgba(138, 43, 226, 0.8), 0 0 50px rgba(138, 43, 226, 0.4)';
        }
    }, true);
    
    document.addEventListener('mouseleave', (e) => {
        if (e.target.closest('.team-member') || e.target.closest('.prism-nav')) {
            cursor.style.transform = 'scale(1)';
            cursor.style.boxShadow = '0 0 10px rgba(138, 43, 226, 0.6), 0 0 20px rgba(138, 43, 226, 0.3)';
        }
    }, true);
}

// Initialize when page loads
document.addEventListener('DOMContentLoaded', () => {
    createStars();
});

// Performance optimization - pause animations when tab is not visible
document.addEventListener('visibilitychange', () => {
    const spaceContainer = document.querySelector('.space-container');
    const demoSpace = document.querySelector('.demo-space');
    
    if (document.hidden) {
        spaceContainer.style.animationPlayState = 'paused';
        if (demoSpace.classList.contains('rpo-active')) {
            demoSpace.style.animationPlayState = 'paused';
        }
    } else {
        spaceContainer.style.animationPlayState = 'running';
        if (demoSpace.classList.contains('rpo-active')) {
            demoSpace.style.animationPlayState = 'running';
        }
    }
});
