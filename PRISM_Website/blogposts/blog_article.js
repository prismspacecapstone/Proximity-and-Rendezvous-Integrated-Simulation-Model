        // Create enhanced star field
        function createStars() {
            const starsContainer = document.querySelector('.stars');
            const starCount = 120;
            
            for (let i = 0; i < starCount; i++) {
                const star = document.createElement('div');
                star.className = 'star';
                
                const size = Math.random() > 0.85 ? Math.random() * 3 + 2 : Math.random() * 1.5 + 0.5;
                star.style.width = size + 'px';
                star.style.height = size + 'px';
                
                if (Math.random() > 0.7) {
                    star.style.background = 'radial-gradient(circle, rgba(200, 230, 255, 0.9), rgba(150, 200, 255, 0.6))';
                    star.style.boxShadow = '0 0 8px rgba(200, 230, 255, 0.5)';
                }
                
                const x = Math.random() * 100;
                const y = Math.random() * 100;
                
                star.style.left = x + '%';
                star.style.top = y + '%';
                star.style.animationDelay = Math.random() * 5 + 's';
                star.style.animationDuration = (Math.random() * 4 + 4) + 's';
                
                starsContainer.appendChild(star);
            }
        }

        // Enhanced custom cursor with reading trail
        const cursor = document.querySelector('.custom-cursor');
        
        document.addEventListener('mousemove', (e) => {
            cursor.style.left = e.clientX - 10 + 'px';
            cursor.style.top = e.clientY - 10 + 'px';
            
            // Create subtle trail for reading
            if (Math.random() > 0.97) {
                createCursorTrail(e.clientX, e.clientY);
            }
        });

        function createCursorTrail(x, y) {
            const trail = document.createElement('div');
            trail.style.position = 'fixed';
            trail.style.left = x - 2 + 'px';
            trail.style.top = y - 2 + 'px';
            trail.style.width = '4px';
            trail.style.height = '4px';
            trail.style.background = 'rgba(200, 230, 255, 0.4)';
            trail.style.borderRadius = '50%';
            trail.style.pointerEvents = 'none';
            trail.style.zIndex = '999';
            trail.style.animation = 'trailFade 1.5s ease-out forwards';
            
            document.body.appendChild(trail);
            
            setTimeout(() => {
                if (trail.parentNode) {
                    trail.parentNode.removeChild(trail);
                }
            }, 1500);
        }

        // Reading progress bar
        function updateReadingProgress() {
            const article = document.querySelector('.article-content');
            const progressBar = document.querySelector('.reading-progress');
            
            if (!article || !progressBar) return;
            
            const articleTop = article.offsetTop;
            const articleHeight = article.offsetHeight;
            const windowTop = window.pageYOffset;
            const windowHeight = window.innerHeight;
            
            const articleBottom = articleTop + articleHeight;
            const windowBottom = windowTop + windowHeight;
            
            let progress = 0;
            
            if (windowTop >= articleTop) {
                if (windowBottom >= articleBottom) {
                    progress = 100;
                } else {
                    const articleVisible = windowBottom - articleTop;
                    progress = (articleVisible / articleHeight) * 100;
                }
            } else if (windowBottom > articleTop) {
                const articleVisible = windowBottom - articleTop;
                progress = (articleVisible / articleHeight) * 100;
            }
            
            progressBar.style.width = Math.min(Math.max(progress, 0), 100) + '%';
        }

        // Enhanced ice belt effects
        let iceBeltPhase = 0;
        setInterval(() => {
            iceBeltPhase += 0.008;
            const iceSystem = document.querySelector('.ice-belt-system');
            if (iceSystem) {
                const shimmer = Math.sin(iceBeltPhase) * 0.3 + 0.7;
                const hueShift = Math.sin(iceBeltPhase * 0.7) * 15;
                iceSystem.style.filter = `brightness(${shimmer}) contrast(1.1) hue-rotate(${hueShift}deg)`;
            }
        }, 100);

        // Smooth navigation prism rotation
        const navPrism = document.querySelector('.nav-prism');
        let navRotation = 0;
        
        function updateNavPrism() {
            navRotation += 0.2;
            if (navRotation >= 360) navRotation -= 360;
            
            if (navPrism) {
                navPrism.style.transform = `rotateZ(${navRotation}deg)`;
            }
            requestAnimationFrame(updateNavPrism);
        }

        // Floating ice crystal physics
        function updateCrystalPositions() {
            const crystals = document.querySelectorAll('.ice-crystal');
            crystals.forEach((crystal, index) => {
                const time = Date.now() * 0.0008 + index * 2.5;
                const x = Math.sin(time * 0.4) * 25;
                const y = Math.cos(time * 0.2) * 15;
                const rotation = time * 15;
                
                crystal.style.transform = `translate(${x}px, ${y}px) rotate(${rotation}deg)`;
            });
            
            requestAnimationFrame(updateCrystalPositions);
        }

        // Parallax scroll effects
        window.addEventListener('scroll', () => {
            const scrollY = window.pageYOffset;
            const iceSystem = document.querySelector('.ice-belt-system');
            const crystals = document.querySelectorAll('.ice-crystal');
            
            updateReadingProgress();
            
            if (iceSystem) {
                iceSystem.style.transform = `translateY(${scrollY * 0.2}px) rotate(${scrollY * 0.05}deg)`;
            }
            
            crystals.forEach((crystal, index) => {
                const speed = 0.1 + (index * 0.05);
                crystal.style.transform += ` translateY(${scrollY * speed}px)`;
            });
        });

        // Article interaction effects
        const articleContent = document.querySelector('.article-content');
        const headings = document.querySelectorAll('.article-content h2, .article-content h3');
        
        // Add glow effect to headings on scroll
        function checkHeadingVisibility() {
            headings.forEach(heading => {
                const rect = heading.getBoundingClientRect();
                const isVisible = rect.top < window.innerHeight * 0.8 && rect.bottom > window.innerHeight * 0.2;
                
                if (isVisible) {
                    heading.style.textShadow = '0 0 25px rgba(200, 230, 255, 0.6), 0 0 50px rgba(150, 200, 255, 0.4)';
                    heading.style.transition = 'text-shadow 0.5s ease';
                } else {
                    heading.style.textShadow = '0 0 20px rgba(200, 230, 255, 0.3)';
                }
            });
        }

        // Highlight box animations
        const highlightBoxes = document.querySelectorAll('.highlight-box');
        
        function animateHighlightBoxes() {
            highlightBoxes.forEach(box => {
                const rect = box.getBoundingClientRect();
                const isVisible = rect.top < window.innerHeight * 0.9 && rect.bottom > window.innerHeight * 0.1;
                
                if (isVisible) {
                    box.style.borderColor = 'rgba(200, 230, 255, 0.5)';
                    box.style.boxShadow = '0 0 30px rgba(200, 230, 255, 0.2), inset 0 0 20px rgba(200, 230, 255, 0.1)';
                    box.style.transform = 'translateY(-2px)';
                } else {
                    box.style.borderColor = 'rgba(200, 230, 255, 0.3)';
                    box.style.boxShadow = 'none';
                    box.style.transform = 'translateY(0px)';
                }
            });
        }

        // Related articles interactions
        const relatedCards = document.querySelectorAll('.related-card');
        relatedCards.forEach(card => {
            card.addEventListener('mouseenter', function() {
                this.style.background = 'linear-gradient(135deg, rgba(255, 255, 255, 0.08) 0%, rgba(200, 230, 255, 0.12) 50%, rgba(150, 200, 255, 0.08) 100%)';
                this.style.borderColor = 'rgba(200, 230, 255, 0.5)';
                
                // Add sparkle effect
                if (Math.random() > 0.6) {
                    createSparkle(this);
                }
            });
            
            card.addEventListener('mouseleave', function() {
                this.style.background = 'linear-gradient(135deg, rgba(255, 255, 255, 0.05) 0%, rgba(200, 230, 255, 0.08) 50%, rgba(150, 200, 255, 0.05) 100%)';
                this.style.borderColor = 'rgba(200, 230, 255, 0.2)';
            });
            
            card.addEventListener('click', function() {
                // Simulate article navigation
                this.style.transform = 'translateY(-5px) scale(0.98)';
                setTimeout(() => {
                    this.style.transform = 'translateY(-5px) scale(1)';
                }, 100);
            });
        });

        function createSparkle(element) {
            const sparkle = document.createElement('div');
            sparkle.style.position = 'absolute';
            sparkle.style.width = '3px';
            sparkle.style.height = '3px';
            sparkle.style.background = 'rgba(200, 230, 255, 0.8)';
            sparkle.style.borderRadius = '50%';
            sparkle.style.top = Math.random() * 100 + '%';
            sparkle.style.left = Math.random() * 100 + '%';
            sparkle.style.animation = 'sparkleEffect 1.2s ease-out forwards';
            sparkle.style.pointerEvents = 'none';
            sparkle.style.zIndex = '5';
            
            element.style.position = 'relative';
            element.appendChild(sparkle);
            
            setTimeout(() => {
                if (sparkle.parentNode) {
                    sparkle.parentNode.removeChild(sparkle);
                }
            }, 1200);
        }

        // Navigation button interactions
        const navButtons = document.querySelectorAll('.nav-button');
        navButtons.forEach(button => {
            button.addEventListener('mouseenter', function() {
                this.style.boxShadow = '0 15px 30px rgba(0, 0, 0, 0.4), 0 0 20px rgba(200, 230, 255, 0.3)';
            });
            
            button.addEventListener('mouseleave', function() {
                this.style.boxShadow = '0 10px 20px rgba(0, 0, 0, 0.3)';
            });
        });

        // Share functionality
        function shareArticle() {
            if (navigator.share) {
                navigator.share({
                    title: document.querySelector('.article-title').textContent,
                    text: document.querySelector('.article-subtitle').textContent,
                    url: window.location.href
                });
            } else {
                // Fallback - copy to clipboard
                navigator.clipboard.writeText(window.location.href).then(() => {
                    // Show temporary notification
                    showNotification('Article URL copied to clipboard!');
                });
            }
        }

        function showNotification(message) {
            const notification = document.createElement('div');
            notification.style.position = 'fixed';
            notification.style.top = '2rem';
            notification.style.right = '2rem';
            notification.style.background = 'linear-gradient(135deg, rgba(200, 230, 255, 0.9) 0%, rgba(150, 200, 255, 0.8) 100%)';
            notification.style.color = '#000';
            notification.style.padding = '1rem 2rem';
            notification.style.borderRadius = '20px';
            notification.style.fontSize = '0.9rem';
            notification.style.fontWeight = '600';
            notification.style.zIndex = '1002';
            notification.style.animation = 'slideInRight 0.3s ease-out';
            notification.textContent = message;
            
            document.body.appendChild(notification);
            
            setTimeout(() => {
                notification.style.animation = 'slideOutRight 0.3s ease-in forwards';
                setTimeout(() => {
                    if (notification.parentNode) {
                        notification.parentNode.removeChild(notification);
                    }
                }, 300);
            }, 3000);
        }

        // Scroll event listeners
        window.addEventListener('scroll', () => {
            checkHeadingVisibility();
            animateHighlightBoxes();
        }, { passive: true });

        // Initialize all effects
        createStars();
        updateNavPrism();
        updateCrystalPositions();
        
        // Initial visibility checks
        checkHeadingVisibility();
        animateHighlightBoxes();
        
        // Add missing animations
        const style = document.createElement('style');
        style.textContent = `
            @keyframes trailFade {
                0% { opacity: 1; transform: scale(1); }
                100% { opacity: 0; transform: scale(0.3); }
            }
            
            @keyframes sparkleEffect {
                0% {
                    opacity: 0;
                    transform: scale(0) rotate(0deg);
                    box-shadow: 0 0 0px rgba(200, 230, 255, 0);
                }
                50% {
                    opacity: 1;
                    transform: scale(1.3) rotate(180deg);
                    box-shadow: 0 0 10px rgba(200, 230, 255, 0.6);
                }
                100% {
                    opacity: 0;
                    transform: scale(0) rotate(360deg);
                    box-shadow: 0 0 0px rgba(200, 230, 255, 0);
                }
            }
            
            @keyframes slideInRight {
                0% {
                    transform: translateX(100%);
                    opacity: 0;
                }
                100% {
                    transform: translateX(0);
                    opacity: 1;
                }
            }
            
            @keyframes slideOutRight {
                0% {
                    transform: translateX(0);
                    opacity: 1;
                }
                100% {
                    transform: translateX(100%);
                    opacity: 0;
                }
            }
        `;
        document.head.appendChild(style);
