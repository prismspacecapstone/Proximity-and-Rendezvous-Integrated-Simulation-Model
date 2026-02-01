        // Generate stars
        function generateStars() {
            const starsContainer = document.querySelector('.stars');
            const numStars = 200;
            
            for (let i = 0; i < numStars; i++) {
                const star = document.createElement('div');
                star.className = 'star';
                
                // Random position
                star.style.left = Math.random() * 100 + '%';
                star.style.top = Math.random() * 100 + '%';
                
                // Random size
                const size = Math.random() * 3 + 1;
                star.style.width = size + 'px';
                star.style.height = size + 'px';
                
                // Random animation delay
                star.style.animationDelay = Math.random() * 4 + 's';
                
                // Random opacity
                star.style.opacity = Math.random() * 0.8 + 0.2;
                
                starsContainer.appendChild(star);
            }
        }

        // Generate quantum particles
        function generateQuantumParticles() {
            const particlesContainer = document.querySelector('.quantum-particles');
            const numParticles = 15;
            
            for (let i = 0; i < numParticles; i++) {
                const particle = document.createElement('div');
                particle.className = 'quantum-particle';
                
                // Random position within container
                particle.style.left = Math.random() * 100 + '%';
                particle.style.top = Math.random() * 100 + '%';
                
                // Random animation delay
                particle.style.animationDelay = Math.random() * 8 + 's';
                
                // Random size variation
                const size = Math.random() * 3 + 2;
                particle.style.width = size + 'px';
                particle.style.height = size + 'px';
                
                // Random colors
                const colors = [
                    'rgba(0, 191, 255, 0.8)',
                    'rgba(138, 43, 226, 0.6)',
                    'rgba(255, 215, 0, 0.7)',
                    'rgba(255, 255, 255, 0.9)'
                ];
                particle.style.background = colors[Math.floor(Math.random() * colors.length)];
                
                particlesContainer.appendChild(particle);
            }
        }

        // Generate space debris
        function generateSpaceDebris() {
            const debrisContainer = document.querySelector('.space-debris');
            const numDebris = 12;
            
            for (let i = 0; i < numDebris; i++) {
                const debris = document.createElement('div');
                debris.className = 'debris-piece';
                
                // Random position within container
                debris.style.left = Math.random() * 100 + '%';
                debris.style.top = Math.random() * 100 + '%';
                
                // Random animation delay
                debris.style.animationDelay = Math.random() * 12 + 's';
                
                // Random size and shape
                const width = Math.random() * 8 + 4;
                const height = Math.random() * 8 + 4;
                debris.style.width = width + 'px';
                debris.style.height = height + 'px';
                debris.style.borderRadius = Math.random() * 3 + 'px';
                
                // Random gray color
                const grayValue = Math.floor(Math.random() * 100 + 100);
                debris.style.background = `rgba(${grayValue}, ${grayValue}, ${grayValue}, ${Math.random() * 0.5 + 0.4})`;
                
                debrisContainer.appendChild(debris);
            }
        }

        // Custom cursor
        let cursor = document.querySelector('.custom-cursor');
        
        document.addEventListener('mousemove', (e) => {
            cursor.style.left = e.clientX + 'px';
            cursor.style.top = e.clientY + 'px';
        });

        // Hover effects for interactive elements
        document.addEventListener('mouseenter', (e) => {
            if (e.target.classList.contains('donate-btn') || 
                e.target.classList.contains('tier-card') || 
                e.target.classList.contains('prism-nav')) {
                cursor.style.transform = 'scale(1.5)';
                cursor.style.background = 'radial-gradient(circle, rgba(255, 215, 0, 1) 0%, rgba(138, 43, 226, 0.8) 50%, transparent 70%)';
            }
        }, true);

        document.addEventListener('mouseleave', (e) => {
            if (e.target.classList.contains('donate-btn') || 
                e.target.classList.contains('tier-card') || 
                e.target.classList.contains('prism-nav')) {
                cursor.style.transform = 'scale(1)';
                cursor.style.background = 'radial-gradient(circle, rgba(255, 215, 0, 0.9) 0%, rgba(138, 43, 226, 0.6) 50%, transparent 70%)';
            }
        }, true);

        // Donation handling functions
        function handleDonation(amount) {
            if (amount === 'custom') {
                const customInput = document.getElementById('customAmount');
                customInput.focus();
                return;
            }
            
            // Simulate donation process
            alert(`Initiating donation of ${amount}. In a real implementation, this would redirect to a payment processor.`);
        }

        function handleCustomDonation() {
            const customAmount = document.getElementById('customAmount').value;
            if (!customAmount || customAmount <= 0) {
                alert('Please enter a valid donation amount.');
                return;
            }
            
            alert(`Initiating custom donation of ${customAmount}. In a real implementation, this would redirect to a payment processor.`);
        }

        // Initialize everything when page loads
        document.addEventListener('DOMContentLoaded', () => {
            generateStars();
            generateQuantumParticles();
            generateSpaceDebris();
        });

        // Add some interactivity to space elements
        document.addEventListener('click', (e) => {
            // Create a ripple effect when clicking on space
            if (e.target === document.body || e.target.classList.contains('space-container')) {
                createRipple(e.clientX, e.clientY);
            }
        });

        function createRipple(x, y) {
            const ripple = document.createElement('div');
            ripple.style.position = 'fixed';
            ripple.style.left = x + 'px';
            ripple.style.top = y + 'px';
            ripple.style.width = '10px';
            ripple.style.height = '10px';
            ripple.style.background = 'radial-gradient(circle, rgba(255, 215, 0, 0.6) 0%, transparent 70%)';
            ripple.style.borderRadius = '50%';
            ripple.style.pointerEvents = 'none';
            ripple.style.zIndex = '999';
            ripple.style.animation = 'rippleEffect 1s ease-out forwards';
            
            document.body.appendChild(ripple);
            
            setTimeout(() => {
                ripple.remove();
            }, 1000);
        }

        // Add ripple animation
        const style = document.createElement('style');
        style.textContent = `
            @keyframes rippleEffect {
                0% {
                    transform: translate(-50%, -50%) scale(0);
                    opacity: 1;
                }
                100% {
                    transform: translate(-50%, -50%) scale(10);
                    opacity: 0;
                }
            }
        `;
        document.head.appendChild(style);
