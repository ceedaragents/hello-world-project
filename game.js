const Game = {
    canvas: null,
    ctx: null,
    width: 800,
    height: 600,
    isRunning: false,
    animationId: null,
    
    init() {
        this.canvas = document.getElementById('gameCanvas');
        
        if (!this.canvas) {
            console.error('Canvas element not found');
            return false;
        }
        
        this.ctx = this.canvas.getContext('2d');
        
        if (!this.ctx) {
            console.error('Could not get 2D context');
            return false;
        }
        
        this.setupCanvas();
        this.setupEventListeners();
        this.handleResize();
        
        console.log('Game initialized successfully');
        console.log(`Canvas dimensions: ${this.canvas.width}x${this.canvas.height}`);
        
        this.drawBackground();
        
        return true;
    },
    
    setupCanvas() {
        const dpr = window.devicePixelRatio || 1;
        
        this.canvas.width = this.width;
        this.canvas.height = this.height;
        
        if (dpr > 1) {
            const rect = this.canvas.getBoundingClientRect();
            this.canvas.width = rect.width * dpr;
            this.canvas.height = rect.height * dpr;
            this.ctx.scale(dpr, dpr);
            this.canvas.style.width = rect.width + 'px';
            this.canvas.style.height = rect.height + 'px';
        }
    },
    
    setupEventListeners() {
        window.addEventListener('resize', () => this.handleResize());
        
        document.addEventListener('keydown', (e) => {
            if (e.code === 'Space') {
                e.preventDefault();
                this.handleInput();
            }
        });
        
        this.canvas.addEventListener('click', () => {
            this.handleInput();
        });
        
        this.canvas.addEventListener('touchstart', (e) => {
            e.preventDefault();
            this.handleInput();
        });
    },
    
    handleResize() {
        const container = this.canvas.parentElement;
        const containerWidth = container.clientWidth - 40;
        const containerHeight = window.innerHeight - 100;
        
        const scaleX = containerWidth / this.width;
        const scaleY = containerHeight / this.height;
        const scale = Math.min(scaleX, scaleY, 1);
        
        if (scale < 1) {
            this.canvas.style.width = (this.width * scale) + 'px';
            this.canvas.style.height = (this.height * scale) + 'px';
        } else {
            this.canvas.style.width = this.width + 'px';
            this.canvas.style.height = this.height + 'px';
        }
    },
    
    handleInput() {
        console.log('Input received');
    },
    
    drawBackground() {
        this.ctx.fillStyle = '#87CEEB';
        this.ctx.fillRect(0, 0, this.width, this.height * 0.8);
        
        this.ctx.fillStyle = '#90EE90';
        this.ctx.fillRect(0, this.height * 0.8, this.width, this.height * 0.2);
        
        this.ctx.fillStyle = '#333';
        this.ctx.font = '24px Arial';
        this.ctx.textAlign = 'center';
        this.ctx.textBaseline = 'middle';
        this.ctx.fillText('Flappy Bird - Canvas Ready', this.width / 2, this.height / 2);
        
        this.ctx.font = '16px Arial';
        this.ctx.fillText('Press SPACE or Click/Tap to interact', this.width / 2, this.height / 2 + 40);
    },
    
    start() {
        if (this.isRunning) return;
        
        this.isRunning = true;
        console.log('Game started');
        this.gameLoop();
    },
    
    stop() {
        if (!this.isRunning) return;
        
        this.isRunning = false;
        if (this.animationId) {
            cancelAnimationFrame(this.animationId);
            this.animationId = null;
        }
        console.log('Game stopped');
    },
    
    gameLoop() {
        if (!this.isRunning) return;
        
        this.update();
        this.render();
        
        this.animationId = requestAnimationFrame(() => this.gameLoop());
    },
    
    update() {
    },
    
    render() {
        this.drawBackground();
    }
};

document.addEventListener('DOMContentLoaded', () => {
    if (Game.init()) {
        console.log('Game ready to start');
    } else {
        console.error('Failed to initialize game');
    }
});