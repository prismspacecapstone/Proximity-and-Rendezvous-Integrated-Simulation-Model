        // Create realistic star field
        function createStars() {
            const starsContainer = document.querySelector('.stars');
            const starCount = 80;
            
            for (let i = 0; i < starCount; i++) {
                const star = document.createElement('div');
                star.className = 'star';
                
                const size = Math.random() > 0.9 ? Math.random() * 2 + 2 : Math.random() * 1.5 + 0.5;
                star.style.width = size + 'px';
                star.style.height = size + 'px';
                
                let x = Math.random() * 100;
                let y = Math.random() * 100;
                
                if (y < 20 && Math.random() < 0.7) {
                    y = 20 + Math.random() * 80;
                }
                
                star.style.left = x + '%';
                star.style.top = y + '%';
                star.style.animationDelay = Math.random() * 4 + 's';
                star.style.animationDuration = (Math.random() * 3 + 3) + 's';
                
                starsContainer.appendChild(star);
            }
        }

        // Create neural network
        function createNeuralNetwork() {
            const networkContainer = document.querySelector('.neural-network');
            const nodeCount = 12;
            const nodes = [];
            
            for (let i = 0; i < nodeCount; i++) {
                const node = document.createElement('div');
                node.className = 'network-node';
                
                const x = Math.random() * 90 + 5;
                const y = Math.random() * 90 + 5;
                
                node.style.left = x + '%';
                node.style.top = y + '%';
                node.style.animationDelay = Math.random() * 4 + 's';
                
                networkContainer.appendChild(node);
                nodes.push({ element: node, x, y });
            }
            
            for (let i = 0; i < nodes.length; i++) {
                for (let j = i + 1; j < nodes.length; j++) {
                    const node1 = nodes[i];
                    const node2 = nodes[j];
                    
                    const distance = Math.sqrt(
                        Math.pow(node1.x - node2.x, 2) + 
                        Math.pow(node1.y - node2.y, 2)
                    );
                    
                    if (distance < 30) {
                        const connection = document.createElement('div');
                        connection.className = 'network-connection';
                        
                        const angle = Math.atan2(node2.y - node1.y, node2.x - node1.x);
                        
                        connection.style.left = node1.x + '%';
                        connection.style.top = node1.y + '%';
                        connection.style.width = distance + '%';
                        connection.style.transform = `rotate(${angle}rad)`;
                        connection.style.animationDelay = Math.random() * 8 + 's';
                        
                        networkContainer.appendChild(connection);
                    }
                }
            }
        }

        // Create asteroid belt
        function createAsteroidBelt() {
            const beltContainer = document.querySelector('.asteroid-belt');
            const asteroidCount = 15;
            
            for (let i = 0; i < asteroidCount; i++) {
                const asteroid = document.createElement('div');
                asteroid.className = 'asteroid';
                
                asteroid.style.left = (i * 12 + Math.random() * 10) + 'px';
                asteroid.style.top = Math.random() * 50 + 'px';
                asteroid.style.animationDelay = Math.random() * 15 + 's';
                asteroid.style.animationDuration = (Math.random() * 10 + 10) + 's';
                
                beltContainer.appendChild(asteroid);
            }
        }

        // Create space particles
        function createSpaceParticles() {
            const particlesContainer = document.querySelector('.space-particles');
            const particleCount = 25;
            
            for (let i = 0; i < particleCount; i++) {
                const particle = document.createElement('div');
                particle.className = 'particle';
                
                particle.style.left = Math.random() * 100 + '%';
                particle.style.animationDelay = Math.random() * 15 + 's';
                particle.style.animationDuration = (Math.random() * 10 + 10) + 's';
                
                particlesContainer.appendChild(particle);
            }
        }

        // Custom cursor
        const cursor = document.querySelector('.custom-cursor');
        document.addEventListener('mousemove', (e) => {
            cursor.style.left = e.clientX - 8 + 'px';
            cursor.style.top = e.clientY - 8 + 'px';
        });

        // Smooth prism interaction
        const prism = document.querySelector('.geometric-prism');
        const prismContainer = document.querySelector('.prism-container');
        let isHoveringPrism = false;
        let currentRotation = 0;
        let animationId;

        function updatePrismRotation() {
            const targetSpeed = isHoveringPrism ? 3 : 20; // Fast when hovering, slow when not
            const rotationSpeed = 360 / (targetSpeed * 60); // degrees per frame at 60fps
            
            currentRotation += rotationSpeed;
            if (currentRotation >= 360) currentRotation -= 360;
            
            const floatY = Math.sin(Date.now() * 0.001) * 10;
            prism.style.transform = `translateY(${floatY}px) rotateZ(${currentRotation}deg)`;
            
            animationId = requestAnimationFrame(updatePrismRotation);
        }

        prismContainer.addEventListener('mouseenter', () => {
            isHoveringPrism = true;
        });

        prismContainer.addEventListener('mouseleave', () => {
            isHoveringPrism = false;
        });

        // Start smooth rotation (disable CSS animation)
        prism.style.animation = 'none';
        updatePrismRotation();

        // Subtle mouse interaction with prism
        document.addEventListener('mousemove', (e) => {
            const centerX = window.innerWidth / 2;
            const centerY = window.innerHeight / 2;
            
            const deltaX = (e.clientX - centerX) / centerX * 10;
            const deltaY = (e.clientY - centerY) / centerY * 5;
            
            // Only apply mouse movement when not hovering (to not interfere with smooth rotation)
            if (!isHoveringPrism) {
                const floatY = Math.sin(Date.now() * 0.001) * 10;
                prism.style.transform = `translateY(${floatY - deltaY}px) rotateZ(${currentRotation + deltaX}deg)`;
            }
        });

        // Initialize all effects
        createStars();
        createNeuralNetwork();
        createAsteroidBelt();
        createSpaceParticles();

        // Dynamic quantum field effects
        let quantumPhase = 0;
        setInterval(() => {
            quantumPhase += 0.01;
            const quantumField = document.querySelector('.quantum-field');
            const intensity = Math.sin(quantumPhase) * 0.3 + 0.7;
            quantumField.style.opacity = intensity * 0.8;
        }, 100);
