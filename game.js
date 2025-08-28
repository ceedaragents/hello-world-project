class Bird {
    constructor(x, y) {
        // Position and size properties
        this.x = x;
        this.y = y;
        this.width = 30;
        this.height = 25;
        
        // Physics properties
        this.velocity = 0;
        this.gravity = 0.0008; // Gravity acceleration per millisecond²
        this.flapStrength = -0.45; // Upward velocity on flap
        this.maxFallSpeed = 0.8; // Terminal velocity
        this.maxRiseSpeed = -0.6; // Maximum upward velocity
        
        // Visual properties
        this.rotation = 0;
        this.maxRotation = Math.PI / 4; // 45 degrees
        this.rotationSpeed = 0.003; // Rotation speed per millisecond
        
        // Animation properties
        this.flapAnimation = 0;
        this.flapSpeed = 0.01;
        this.wingOffset = 0;
    }
    
    update(deltaTime) {
        // Apply gravity
        this.velocity += this.gravity * deltaTime;
        
        // Clamp velocity to max speeds
        this.velocity = Math.max(this.maxRiseSpeed, Math.min(this.maxFallSpeed, this.velocity));
        
        // Update position based on velocity
        this.y += this.velocity * deltaTime;
        
        // Update rotation based on velocity
        if (this.velocity > 0) {
            // Falling - rotate down
            this.rotation = Math.min(this.maxRotation, this.rotation + this.rotationSpeed * deltaTime);
        } else {
            // Rising - rotate up
            this.rotation = Math.max(-this.maxRotation, this.rotation - this.rotationSpeed * deltaTime * 2);
        }
        
        // Update wing flap animation
        this.flapAnimation += this.flapSpeed * deltaTime;
        this.wingOffset = Math.sin(this.flapAnimation) * 3;
    }
    
    flap() {
        this.velocity = this.flapStrength;
        // Reset rotation for immediate visual feedback
        this.rotation = -this.maxRotation * 0.7;
        // Reset flap animation for wing effect
        this.flapAnimation = 0;
    }
    
    render(ctx) {
        ctx.save();
        
        // Translate to bird center for rotation
        ctx.translate(this.x + this.width / 2, this.y + this.height / 2);
        ctx.rotate(this.rotation);
        
        // Bird body (main circle)
        ctx.fillStyle = '#FFD700'; // Golden yellow
        ctx.strokeStyle = '#FFA500'; // Orange outline
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.arc(0, 0, this.width / 2, 0, Math.PI * 2);
        ctx.fill();
        ctx.stroke();
        
        // Bird beak
        ctx.fillStyle = '#FFA500';
        ctx.beginPath();
        ctx.moveTo(this.width / 2 - 5, -3);
        ctx.lineTo(this.width / 2 + 8, 0);
        ctx.lineTo(this.width / 2 - 5, 3);
        ctx.closePath();
        ctx.fill();
        
        // Bird eye
        ctx.fillStyle = '#000000';
        ctx.beginPath();
        ctx.arc(5, -5, 3, 0, Math.PI * 2);
        ctx.fill();
        
        // Eye highlight
        ctx.fillStyle = '#FFFFFF';
        ctx.beginPath();
        ctx.arc(6, -6, 1, 0, Math.PI * 2);
        ctx.fill();
        
        // Wings with animation
        ctx.fillStyle = '#FFA500';
        ctx.strokeStyle = '#FF8C00';
        ctx.lineWidth = 1;
        
        // Wing animation offset
        const wingY = this.wingOffset;
        
        // Left wing
        ctx.beginPath();
        ctx.ellipse(-8, wingY, 12, 8, -Math.PI / 6, 0, Math.PI * 2);
        ctx.fill();
        ctx.stroke();
        
        // Right wing (smaller, partially hidden)
        ctx.beginPath();
        ctx.ellipse(-5, wingY + 2, 8, 6, Math.PI / 8, 0, Math.PI * 2);
        ctx.fill();
        ctx.stroke();
        
        // Tail feathers
        ctx.fillStyle = '#FF8C00';
        ctx.beginPath();
        ctx.ellipse(-this.width / 2 + 2, 2, 8, 4, Math.PI / 4, 0, Math.PI * 2);
        ctx.fill();
        ctx.stroke();
        
        ctx.restore();
    }
    
    // Collision detection methods
    getBounds() {
        return {
            left: this.x,
            right: this.x + this.width,
            top: this.y,
            bottom: this.y + this.height,
            centerX: this.x + this.width / 2,
            centerY: this.y + this.height / 2
        };
    }
    
    checkBoundaryCollision(canvasWidth, canvasHeight) {
        const bounds = this.getBounds();
        
        // Check top boundary
        if (bounds.top <= 0) {
            this.y = 0;
            this.velocity = Math.max(0, this.velocity); // Stop upward movement
            return 'top';
        }
        
        // Check bottom boundary
        if (bounds.bottom >= canvasHeight) {
            this.y = canvasHeight - this.height;
            this.velocity = 0;
            return 'bottom';
        }
        
        return null;
    }
    
    reset(x, y) {
        this.x = x;
        this.y = y;
        this.velocity = 0;
        this.rotation = 0;
        this.flapAnimation = 0;
        this.wingOffset = 0;
    }
}

class Pipe {
    constructor(x, canvasHeight, gapHeight = 150, gapY = null) {
        // Position properties
        this.x = x;
        this.width = 50;
        this.canvasHeight = canvasHeight;
        
        // Gap properties
        this.gapHeight = gapHeight;
        // If gapY not provided, randomize gap position
        this.gapY = gapY || this.randomizeGapPosition();
        
        // Calculate pipe heights
        this.topHeight = this.gapY - this.gapHeight / 2;
        this.bottomY = this.gapY + this.gapHeight / 2;
        this.bottomHeight = this.canvasHeight - this.bottomY;
        
        // Scoring
        this.passed = false;
    }
    
    randomizeGapPosition() {
        // Keep gap center between 25% and 75% of canvas height
        const minGapY = this.canvasHeight * 0.25 + this.gapHeight / 2;
        const maxGapY = this.canvasHeight * 0.75 - this.gapHeight / 2;
        return minGapY + Math.random() * (maxGapY - minGapY);
    }
    
    update(deltaTime, speed) {
        this.x -= speed * deltaTime;
    }
    
    render(ctx) {
        ctx.save();
        
        // Pipe styling
        ctx.fillStyle = '#32CD32'; // Lime green
        ctx.strokeStyle = '#228B22'; // Forest green
        ctx.lineWidth = 3;
        
        // Draw top pipe
        if (this.topHeight > 0) {
            ctx.fillRect(this.x, 0, this.width, this.topHeight);
            ctx.strokeRect(this.x, 0, this.width, this.topHeight);
            
            // Top pipe cap
            ctx.fillRect(this.x - 5, this.topHeight - 20, this.width + 10, 20);
            ctx.strokeRect(this.x - 5, this.topHeight - 20, this.width + 10, 20);
        }
        
        // Draw bottom pipe
        if (this.bottomHeight > 0) {
            ctx.fillRect(this.x, this.bottomY, this.width, this.bottomHeight);
            ctx.strokeRect(this.x, this.bottomY, this.width, this.bottomHeight);
            
            // Bottom pipe cap
            ctx.fillRect(this.x - 5, this.bottomY, this.width + 10, 20);
            ctx.strokeRect(this.x - 5, this.bottomY, this.width + 10, 20);
        }
        
        ctx.restore();
    }
    
    getBounds() {
        return {
            // Top pipe bounds
            topPipe: {
                left: this.x,
                right: this.x + this.width,
                top: 0,
                bottom: this.topHeight
            },
            // Bottom pipe bounds
            bottomPipe: {
                left: this.x,
                right: this.x + this.width,
                top: this.bottomY,
                bottom: this.canvasHeight
            }
        };
    }
    
    checkCollision(bird) {
        const birdBounds = bird.getBounds();
        const pipeBounds = this.getBounds();
        
        // Check collision with top pipe
        if (this.topHeight > 0 && 
            birdBounds.right > pipeBounds.topPipe.left &&
            birdBounds.left < pipeBounds.topPipe.right &&
            birdBounds.bottom > pipeBounds.topPipe.top &&
            birdBounds.top < pipeBounds.topPipe.bottom) {
            return 'top';
        }
        
        // Check collision with bottom pipe
        if (this.bottomHeight > 0 &&
            birdBounds.right > pipeBounds.bottomPipe.left &&
            birdBounds.left < pipeBounds.bottomPipe.right &&
            birdBounds.bottom > pipeBounds.bottomPipe.top &&
            birdBounds.top < pipeBounds.bottomPipe.bottom) {
            return 'bottom';
        }
        
        return null;
    }
    
    checkBirdPassed(bird) {
        if (!this.passed && bird.x > this.x + this.width) {
            this.passed = true;
            return true;
        }
        return false;
    }
    
    isOffScreen() {
        return this.x + this.width < 0;
    }
}

class FlappyBirdGame {
    constructor() {
        this.canvas = document.getElementById('gameCanvas');
        this.ctx = this.canvas.getContext('2d');
        this.scoreElement = document.getElementById('score');
        this.gameStateElement = document.getElementById('gameState');
        
        this.GAME_STATES = {
            READY: 'READY',
            PLAYING: 'PLAYING',
            GAMEOVER: 'GAMEOVER'
        };
        
        this.gameState = this.GAME_STATES.READY;
        this.score = 0;
        this.lastTime = 0;
        this.frameCount = 0;
        this.fpsCounter = 0;
        this.fpsLastTime = 0;
        
        // Initialize bird
        this.bird = new Bird(this.canvas.width / 4, this.canvas.height / 2);
        
        // Initialize pipes
        this.pipes = [];
        this.pipeSpeed = 0.15; // Pipes moving speed (pixels per millisecond)
        this.pipeSpacing = 250; // Distance between pipe pairs
        this.lastPipeTime = 0;
        this.pipeInterval = 2000; // Time between pipe spawns (ms)
        this.pipeGapHeight = 120; // Gap height (3-4 bird heights: 25*4 = 100, using 120 for fairness)
        
        this.init();
    }
    
    init() {
        this.setupEventListeners();
        this.updateUI();
        this.gameLoop();
    }
    
    setupEventListeners() {
        document.addEventListener('keydown', (e) => {
            if (e.code === 'Space') {
                e.preventDefault();
                this.handleSpacePress();
            }
        });
        
        this.canvas.addEventListener('click', () => {
            this.handleSpacePress();
        });
    }
    
    handleSpacePress() {
        switch (this.gameState) {
            case this.GAME_STATES.READY:
                this.startGame();
                break;
            case this.GAME_STATES.PLAYING:
                this.bird.flap();
                break;
            case this.GAME_STATES.GAMEOVER:
                this.resetGame();
                break;
        }
    }
    
    startGame() {
        this.gameState = this.GAME_STATES.PLAYING;
        this.updateUI();
    }
    
    resetGame() {
        this.gameState = this.GAME_STATES.READY;
        this.score = 0;
        // Reset bird position and physics
        this.bird.reset(this.canvas.width / 4, this.canvas.height / 2);
        // Clear all pipes
        this.pipes = [];
        this.lastPipeTime = 0;
        this.updateUI();
    }
    
    update(deltaTime) {
        if (this.gameState === this.GAME_STATES.PLAYING) {
            // Update bird physics
            this.bird.update(deltaTime);
            
            // Update pipes
            this.updatePipes(deltaTime);
            
            // Check pipe collisions
            if (this.checkPipeCollisions()) {
                this.gameState = this.GAME_STATES.GAMEOVER;
                this.updateUI();
                return;
            }
            
            // Check scoring (bird passed through pipes)
            this.checkScoring();
            
            // Check boundary collisions
            const collision = this.bird.checkBoundaryCollision(this.canvas.width, this.canvas.height);
            if (collision === 'bottom') {
                // Game over when bird hits bottom
                this.gameState = this.GAME_STATES.GAMEOVER;
                this.updateUI();
            }
        }
    }
    
    updatePipes(deltaTime) {
        // Spawn new pipes
        if (Date.now() - this.lastPipeTime > this.pipeInterval) {
            this.spawnPipe();
            this.lastPipeTime = Date.now();
        }
        
        // Update existing pipes
        this.pipes.forEach(pipe => {
            pipe.update(deltaTime, this.pipeSpeed);
        });
        
        // Remove off-screen pipes (memory management)
        this.pipes = this.pipes.filter(pipe => !pipe.isOffScreen());
    }
    
    spawnPipe() {
        const x = this.canvas.width + 50; // Start off-screen to the right
        const pipe = new Pipe(x, this.canvas.height, this.pipeGapHeight);
        this.pipes.push(pipe);
    }
    
    checkPipeCollisions() {
        for (let pipe of this.pipes) {
            const collision = pipe.checkCollision(this.bird);
            if (collision) {
                return true;
            }
        }
        return false;
    }
    
    checkScoring() {
        this.pipes.forEach(pipe => {
            if (pipe.checkBirdPassed(this.bird)) {
                this.score++;
                this.updateUI();
            }
        });
    }
    
    render() {
        this.clearCanvas();
        
        switch (this.gameState) {
            case this.GAME_STATES.READY:
                this.renderReadyState();
                break;
            case this.GAME_STATES.PLAYING:
                this.renderPlayingState();
                break;
            case this.GAME_STATES.GAMEOVER:
                this.renderGameOverState();
                break;
        }
        
        this.renderFPS();
    }
    
    clearCanvas() {
        this.ctx.fillStyle = '#70C5CE';
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
        
        this.drawBackground();
    }
    
    drawBackground() {
        const gradient = this.ctx.createLinearGradient(0, 0, 0, this.canvas.height);
        gradient.addColorStop(0, '#87CEEB');
        gradient.addColorStop(0.7, '#70C5CE');
        gradient.addColorStop(1, '#4682B4');
        
        this.ctx.fillStyle = gradient;
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
        
        this.drawClouds();
    }
    
    drawClouds() {
        this.ctx.fillStyle = 'rgba(255, 255, 255, 0.8)';
        
        const time = Date.now() * 0.001;
        const clouds = [
            { x: 50 + Math.sin(time * 0.5) * 30, y: 80, size: 30 },
            { x: 200 + Math.sin(time * 0.3) * 20, y: 120, size: 40 },
            { x: 350 + Math.sin(time * 0.4) * 25, y: 60, size: 35 },
            { x: 100 + Math.sin(time * 0.6) * 15, y: 200, size: 25 }
        ];
        
        clouds.forEach(cloud => {
            this.drawCloud(cloud.x, cloud.y, cloud.size);
        });
    }
    
    drawCloud(x, y, size) {
        this.ctx.beginPath();
        this.ctx.arc(x, y, size, 0, Math.PI * 2);
        this.ctx.arc(x + size * 0.7, y, size * 0.8, 0, Math.PI * 2);
        this.ctx.arc(x + size * 1.4, y, size * 0.6, 0, Math.PI * 2);
        this.ctx.arc(x + size * 0.35, y - size * 0.5, size * 0.7, 0, Math.PI * 2);
        this.ctx.arc(x + size * 1.05, y - size * 0.4, size * 0.5, 0, Math.PI * 2);
        this.ctx.fill();
    }
    
    renderReadyState() {
        // Render the bird in ready state
        this.bird.render(this.ctx);
        
        this.drawText('Press SPACE to Start', this.canvas.width / 2, this.canvas.height / 2, '24px Arial', '#FFFFFF', 'center');
        this.drawText('Click canvas or press SPACE to play', this.canvas.width / 2, this.canvas.height / 2 + 40, '16px Arial', '#FFFFFF', 'center');
    }
    
    renderPlayingState() {
        // Render pipes first (behind bird)
        this.pipes.forEach(pipe => {
            pipe.render(this.ctx);
        });
        
        // Render the bird
        this.bird.render(this.ctx);
        
        // Optional UI text (can be removed for cleaner gameplay)
        this.drawText('Press SPACE to flap', this.canvas.width / 2, this.canvas.height - 30, '14px Arial', '#FFFFFF', 'center');
    }
    
    renderGameOverState() {
        this.drawText('Game Over!', this.canvas.width / 2, this.canvas.height / 2 - 40, '32px Arial', '#FF6B6B', 'center');
        this.drawText(`Final Score: ${this.score}`, this.canvas.width / 2, this.canvas.height / 2, '20px Arial', '#FFFFFF', 'center');
        this.drawText('Press SPACE to Restart', this.canvas.width / 2, this.canvas.height / 2 + 40, '18px Arial', '#FFFFFF', 'center');
    }
    
    renderFPS() {
        if (this.fpsCounter > 0) {
            this.drawText(`FPS: ${this.fpsCounter}`, 10, 25, '14px Arial', '#FFFFFF', 'left');
        }
    }
    
    drawText(text, x, y, font, color, align = 'left') {
        this.ctx.save();
        this.ctx.font = font;
        this.ctx.fillStyle = color;
        this.ctx.textAlign = align;
        this.ctx.strokeStyle = '#000000';
        this.ctx.lineWidth = 3;
        this.ctx.strokeText(text, x, y);
        this.ctx.fillText(text, x, y);
        this.ctx.restore();
    }
    
    updateUI() {
        this.scoreElement.textContent = `Score: ${this.score}`;
        
        switch (this.gameState) {
            case this.GAME_STATES.READY:
                this.gameStateElement.textContent = 'Press SPACE to start';
                break;
            case this.GAME_STATES.PLAYING:
                this.gameStateElement.textContent = 'Playing - SPACE to flap';
                break;
            case this.GAME_STATES.GAMEOVER:
                this.gameStateElement.textContent = 'Game Over - SPACE to restart';
                break;
        }
    }
    
    calculateFPS(currentTime) {
        this.frameCount++;
        
        if (currentTime - this.fpsLastTime >= 1000) {
            this.fpsCounter = Math.round((this.frameCount * 1000) / (currentTime - this.fpsLastTime));
            this.frameCount = 0;
            this.fpsLastTime = currentTime;
        }
    }
    
    gameLoop(currentTime = 0) {
        const deltaTime = currentTime - this.lastTime;
        this.lastTime = currentTime;
        
        this.calculateFPS(currentTime);
        
        this.update(deltaTime);
        this.render();
        
        requestAnimationFrame((time) => this.gameLoop(time));
    }
}

document.addEventListener('DOMContentLoaded', () => {
    const game = new FlappyBirdGame();
});