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
        this.updateUI();
    }
    
    update(deltaTime) {
        if (this.gameState === this.GAME_STATES.PLAYING) {
        }
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
        this.drawText('Press SPACE to Start', this.canvas.width / 2, this.canvas.height / 2, '24px Arial', '#FFFFFF', 'center');
        this.drawText('Click canvas or press SPACE to play', this.canvas.width / 2, this.canvas.height / 2 + 40, '16px Arial', '#FFFFFF', 'center');
    }
    
    renderPlayingState() {
        this.drawText('Game Running!', this.canvas.width / 2, 50, '20px Arial', '#FFFFFF', 'center');
        this.drawText('Press SPACE to flap', this.canvas.width / 2, this.canvas.height - 50, '16px Arial', '#FFFFFF', 'center');
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