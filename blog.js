        // Create enhanced star field with ice crystal effects
        function createStars() {
            const starsContainer = document.querySelector('.stars');
            const starCount = 150;
            
            for (let i = 0; i < starCount; i++) {
                const star = document.createElement('div');
                star.className = 'star';
                
                // Vary star sizes and brightness
                const size = Math.random() > 0.85 ? Math.random() * 3 + 2 : Math.random() * 1.5 + 0.5;
                star.style.width = size + 'px';
                star.style.height = size + 'px';
                
                // Add ice-blue tint to some stars
                if (Math.random() > 0.7) {
                    star.style.background = 'radial-gradient(circle, rgba(200, 230, 255, 0.9), rgba(150, 200, 255, 0.6))';
                    star.style.boxShadow = '0 0 10px rgba(200, 230, 255, 0.6)';
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

        // Enhanced custom cursor with ice effect
        const cursor = document.querySelector('.custom-cursor');
        let cursorTrail = [];
        
        document.addEventListener('mousemove', (e) => {
            cursor.style.left = e.clientX - 10 + 'px';
            cursor.style.top = e.clientY - 10 + 'px';
            
            // Create ice crystal trail effect
            if (Math.random() > 0.95) {
                createCursorTrail(e.clientX, e.clientY);
            }
        });

        function createCursorTrail(x, y) {
            const trail = document.createElement('div');
            trail.style.position = 'fixed';
            trail.style.left = x - 3 + 'px';
            trail.style.top = y - 3 + 'px';
            trail.style.width = '6px';
            trail.style.height = '6px';
            trail.style.background = 'rgba(200, 230, 255, 0.6)';
            trail.style.borderRadius = '50%';
            trail.style.pointerEvents = 'none';
            trail.style.zIndex = '999';
            trail.style.animation = 'trailFade 2s ease-out forwards';
            
            document.body.appendChild(trail);
            
            setTimeout(() => {
                if (trail.parentNode) {
                    trail.parentNode.removeChild(trail);
                }
            }, 2000);
        }

        // Dynamic ice belt effects
        let iceBeltPhase = 0;
        setInterval(() => {
            iceBeltPhase += 0.01;
            const iceSystem = document.querySelector('.ice-belt-system');
            if (iceSystem) {
                const shimmer = Math.sin(iceBeltPhase) * 0.3 + 0.7;
                iceSystem.style.filter = `brightness(${shimmer}) contrast(1.1)`;
            }
        }, 100);

        // Blog post interactions
        const blogPosts = document.querySelectorAll('.blog-post');
        blogPosts.forEach(post => {
            post.addEventListener('mouseenter', function() {
                this.style.transform = 'translateY(-8px) scale(1.02)';
                
                // Add sparkle effect
                if (Math.random() > 0.7) {
                    createSparkle(this);
                }
            });
            
            post.addEventListener('mouseleave', function() {
                this.style.transform = 'translateY(0px) scale(1)';
            });
        });

        function createSparkle(element) {
            const sparkle = document.createElement('div');
            sparkle.style.position = 'absolute';
            sparkle.style.width = '4px';
            sparkle.style.height = '4px';
            sparkle.style.background = 'rgba(200, 230, 255, 0.8)';
            sparkle.style.borderRadius = '50%';
            sparkle.style.top = Math.random() * 100 + '%';
            sparkle.style.left = Math.random() * 100 + '%';
            sparkle.style.animation = 'sparkleEffect 1.5s ease-out forwards';
            sparkle.style.pointerEvents = 'none';
            sparkle.style.zIndex = '5';
            
            element.style.position = 'relative';
            element.appendChild(sparkle);
            
            setTimeout(() => {
                if (sparkle.parentNode) {
                    sparkle.parentNode.removeChild(sparkle);
                }
            }, 1500);
        }

        // Enhanced frozen nebula effects
        let nebulaPhase = 0;
        setInterval(() => {
            nebulaPhase += 0.008;
            const nebula = document.querySelector('.frozen-nebula');
            if (nebula) {
                const intensity = Math.sin(nebulaPhase) * 0.3 + 0.6;
                const hueShift = Math.sin(nebulaPhase * 0.5) * 20;
                nebula.style.opacity = intensity;
                nebula.style.filter = `blur(3px) hue-rotate(${hueShift}deg)`;
            }
        }, 120);

        // Smooth navigation prism rotation with ice crystal effect
        const navPrism = document.querySelector('.nav-prism');
        let navRotation = 0;
        
        function updateNavPrism() {
            navRotation += 0.3;
            if (navRotation >= 360) navRotation -= 360;
            
            if (navPrism) {
                navPrism.style.transform = `rotateZ(${navRotation}deg)`;
            }
            requestAnimationFrame(updateNavPrism);
        }
        
        updateNavPrism();

        // Floating ice crystal physics simulation
        function updateCrystalPositions() {
            const crystals = document.querySelectorAll('.ice-crystal');
            crystals.forEach((crystal, index) => {
                const time = Date.now() * 0.001 + index * 2;
                const x = Math.sin(time * 0.5) * 30;
                const y = Math.cos(time * 0.3) * 20;
                const rotation = time * 20;
                
                crystal.style.transform = `translate(${x}px, ${y}px) rotate(${rotation}deg)`;
            });
            
            requestAnimationFrame(updateCrystalPositions);
        }
        
        updateCrystalPositions();

        // Parallax scroll effect for space elements
        window.addEventListener('scroll', () => {
            const scrollY = window.pageYOffset;
            const iceSystem = document.querySelector('.ice-belt-system');
            const nebula = document.querySelector('.frozen-nebula');
            const comet = document.querySelector('.ice-comet');
            
            if (iceSystem) {
                iceSystem.style.transform = `translateY(${scrollY * 0.3}px) rotate(${scrollY * 0.1}deg)`;
            }
            
            if (nebula) {
                nebula.style.transform = `translateY(${scrollY * 0.2}px)`;
            }
            
            if (comet) {
                comet.style.transform = `translateY(${scrollY * 0.4}px)`;
            }
        });
                
                // Add click effect
                this.style.transform = 'translateY(-8px) scale(0.98)';
                this.style.transition = 'transform 0.1s ease';
                
                setTimeout(() => {
                    this.style.transform = 'translateY(-8px) scale(1.02)';
                    this.style.transition = 'transform 0.3s ease';
                }, 100);
                
                // Simulate navigation (you would replace this with actual routing)
                console.log('Navigating to blog post:', this.querySelector('.post-title').textContent);
            });
        });

        // Sidebar interactions
        const sidebarLinks = document.querySelectorAll('.sidebar-link');
        sidebarLinks.forEach(link => {
            link.addEventListener('mouseenter', function() {
                this.style.color = 'rgba(200, 230, 255, 1)';
                this.style.textShadow = '0 0 10px rgba(200, 230, 255, 0.5)';
            });
            
            link.addEventListener('mouseleave', function() {
                this.style.color = 'rgba(255, 255, 255, 0.8)';
                this.style.textShadow = 'none';
            });
        });

        // Initialize all effects
        createStars();

        // Wait for DOM to be fully loaded before initializing
        document.addEventListener('DOMContentLoaded', function() {
            // Re-initialize if elements weren't ready
            if (document.querySelectorAll('.star').length === 0) {
                createStars();
            }
            
            // Start all animations
            updateNavPrism();
            updateCrystalPositions();
        });
