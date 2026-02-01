        // Create realistic star field
        function createStars() {
            const starsContainer = document.querySelector('.stars');
            const starCount = 100;
            
            for (let i = 0; i < starCount; i++) {
                const star = document.createElement('div');
                star.className = 'star';
                
                const size = Math.random() > 0.9 ? Math.random() * 2 + 2 : Math.random() * 1.5 + 0.5;
                star.style.width = size + 'px';
                star.style.height = size + 'px';
                
                const x = Math.random() * 100;
                const y = Math.random() * 100;
                
                star.style.left = x + '%';
                star.style.top = y + '%';
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

        // Form submission
        document.getElementById('contactForm').addEventListener('submit', function(e) {
            e.preventDefault();
            
            // Get form data
            const firstName = this.querySelector('input[placeholder*="first name"]').value;
            const lastName = this.querySelector('input[placeholder*="last name"]').value;
            const email = this.querySelector('input[type="email"]').value;
            const organization = this.querySelector('input[placeholder*="organization"]').value;
            const subject = this.querySelector('.form-select').value;
            const message = this.querySelector('.form-textarea').value;
            
            // Create email content
            const emailSubject = `PRISM Contact: ${subject || 'General Inquiry'}`;
            const emailBody = `From: ${firstName} ${lastName}
Email: ${email}
Organization: ${organization || 'Not specified'}
Subject: ${subject || 'General Inquiry'}

Message:
${message}`;
            
            // Create mailto link
            const mailtoLink = `mailto:CODERR2@my.erau.edu?subject=${encodeURIComponent(emailSubject)}&body=${encodeURIComponent(emailBody)}`;
            
            // Add visual feedback
            const submitBtn = this.querySelector('.submit-btn');
            const originalText = submitBtn.textContent;
            
            submitBtn.textContent = 'Opening Email...';
            submitBtn.style.transform = 'translateY(-2px) scale(0.98)';
            
            // Open email client
            window.location.href = mailtoLink;
            
            setTimeout(() => {
                submitBtn.textContent = 'Email Opened ✓';
                submitBtn.style.background = 'linear-gradient(135deg, rgba(76, 175, 80, 0.4), rgba(139, 195, 74, 0.4))';
                
                setTimeout(() => {
                    submitBtn.textContent = originalText;
                    submitBtn.style.transform = '';
                    submitBtn.style.background = '';
                }, 2000);
            }, 500);
        });

        // Enhanced form interactions
        const inputs = document.querySelectorAll('.form-input, .form-textarea, .form-select');
        inputs.forEach(input => {
            input.addEventListener('focus', function() {
                this.parentElement.style.transform = 'translateY(-2px)';
                this.parentElement.style.transition = 'transform 0.3s ease';
            });
            
            input.addEventListener('blur', function() {
                this.parentElement.style.transform = '';
            });
        });

        // Initialize effects
        createStars();

        // Dynamic nebula effects
        let nebulaPhase = 0;
        setInterval(() => {
            nebulaPhase += 0.005;
            const nebula = document.querySelector('.nebula-cloud');
            const intensity = Math.sin(nebulaPhase) * 0.2 + 0.8;
            nebula.style.opacity = intensity;
        }, 100);

        // Smooth navigation prism rotation
        const navPrism = document.querySelector('.nav-prism');
        let navRotation = 0;
        
        function updateNavPrism() {
            navRotation += 0.5;
            if (navRotation >= 360) navRotation -= 360;
            
            navPrism.style.transform = `rotateZ(${navRotation}deg)`;
            requestAnimationFrame(updateNavPrism);
        }
        
        updateNavPrism();
