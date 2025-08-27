'use strict';

const ScrabbleGame = {
    config: {
        boardSize: 15,
        tilePoints: {
            A: 1, B: 3, C: 3, D: 2, E: 1, F: 4, G: 2, H: 4,
            I: 1, J: 8, K: 5, L: 1, M: 3, N: 1, O: 1, P: 3,
            Q: 10, R: 1, S: 1, T: 1, U: 1, V: 4, W: 4, X: 8,
            Y: 4, Z: 10, BLANK: 0
        },
        tileDistribution: {
            A: 9, B: 2, C: 2, D: 4, E: 12, F: 2, G: 3, H: 2,
            I: 9, J: 1, K: 1, L: 4, M: 2, N: 6, O: 8, P: 2,
            Q: 1, R: 6, S: 4, T: 6, U: 4, V: 2, W: 2, X: 1,
            Y: 2, Z: 1, BLANK: 2
        },
        maxPlayers: 4,
        tilesPerPlayer: 7,
        specialSquares: {
            tripleWord: [
                [0, 0], [0, 7], [0, 14],
                [7, 0], [7, 14],
                [14, 0], [14, 7], [14, 14]
            ],
            doubleWord: [
                [1, 1], [2, 2], [3, 3], [4, 4],
                [1, 13], [2, 12], [3, 11], [4, 10],
                [13, 1], [12, 2], [11, 3], [10, 4],
                [13, 13], [12, 12], [11, 11], [10, 10],
                [7, 7]
            ],
            tripleLetter: [
                [1, 5], [1, 9],
                [5, 1], [5, 5], [5, 9], [5, 13],
                [9, 1], [9, 5], [9, 9], [9, 13],
                [13, 5], [13, 9]
            ],
            doubleLetter: [
                [0, 3], [0, 11],
                [2, 6], [2, 8],
                [3, 0], [3, 7], [3, 14],
                [6, 2], [6, 6], [6, 8], [6, 12],
                [7, 3], [7, 11],
                [8, 2], [8, 6], [8, 8], [8, 12],
                [11, 0], [11, 7], [11, 14],
                [12, 6], [12, 8],
                [14, 3], [14, 11]
            ],
            center: [[7, 7]]
        }
    },
    
    state: {
        isInitialized: false,
        currentPlayer: null,
        players: [],
        board: [],
        tileBag: [],
        playerRack: [],
        tiles: {},
        score: {},
        draggedTile: null,
        tileIdCounter: 0
    },
    
    init() {
        console.log('Scrabble game initialized');
        
        this.state.isInitialized = true;
        this.createBoard();
        this.initializeTileBag();
        this.createTileRack();
        this.setupEventListeners();
        this.drawInitialTiles();
    },
    
    initializeTileBag() {
        this.state.tileBag = [];
        this.state.tiles = {};
        
        for (const [letter, count] of Object.entries(this.config.tileDistribution)) {
            for (let i = 0; i < count; i++) {
                const tileId = `tile-${this.state.tileIdCounter++}`;
                const tile = {
                    id: tileId,
                    letter: letter,
                    points: this.config.tilePoints[letter],
                    location: 'bag'
                };
                this.state.tileBag.push(tile);
                this.state.tiles[tileId] = tile;
            }
        }
        
        this.shuffleTileBag();
        console.log(`Tile bag initialized with ${this.state.tileBag.length} tiles`);
    },
    
    shuffleTileBag() {
        for (let i = this.state.tileBag.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [this.state.tileBag[i], this.state.tileBag[j]] = 
                [this.state.tileBag[j], this.state.tileBag[i]];
        }
    },
    
    createTileRack() {
        const rackElement = document.getElementById('tile-rack');
        if (!rackElement) return;
        
        rackElement.innerHTML = '';
        for (let i = 0; i < this.config.tilesPerPlayer; i++) {
            const slot = document.createElement('div');
            slot.className = 'tile-slot';
            slot.dataset.slotIndex = i;
            rackElement.appendChild(slot);
        }
        
        this.state.playerRack = new Array(this.config.tilesPerPlayer).fill(null);
    },
    
    drawInitialTiles() {
        const tilesToDraw = Math.min(this.config.tilesPerPlayer, this.state.tileBag.length);
        for (let i = 0; i < tilesToDraw; i++) {
            this.drawTileToRack(i);
        }
    },
    
    drawTileToRack(slotIndex) {
        if (this.state.tileBag.length === 0) {
            console.log('No more tiles in bag');
            return null;
        }
        
        if (this.state.playerRack[slotIndex] !== null) {
            return null;
        }
        
        const tile = this.state.tileBag.pop();
        tile.location = 'rack';
        tile.rackIndex = slotIndex;
        this.state.playerRack[slotIndex] = tile;
        
        const tileElement = this.createTileElement(tile);
        const slot = document.querySelector(`.tile-slot[data-slot-index="${slotIndex}"]`);
        if (slot) {
            slot.appendChild(tileElement);
        }
        
        return tile;
    },
    
    createTileElement(tile) {
        const tileDiv = document.createElement('div');
        tileDiv.className = 'tile';
        tileDiv.id = tile.id;
        tileDiv.draggable = true;
        
        if (tile.letter === 'BLANK') {
            tileDiv.classList.add('blank');
        }
        
        const letterSpan = document.createElement('span');
        letterSpan.className = 'tile-letter';
        letterSpan.textContent = tile.letter === 'BLANK' ? '' : tile.letter;
        
        const pointsSpan = document.createElement('span');
        pointsSpan.className = 'tile-points';
        pointsSpan.textContent = tile.points;
        
        tileDiv.appendChild(letterSpan);
        tileDiv.appendChild(pointsSpan);
        
        this.setupTileDragEvents(tileDiv, tile);
        
        return tileDiv;
    },
    
    setupTileDragEvents(tileElement, tile) {
        tileElement.addEventListener('dragstart', (e) => {
            this.handleDragStart(e, tile);
        });
        
        tileElement.addEventListener('dragend', (e) => {
            this.handleDragEnd(e, tile);
        });
        
        tileElement.addEventListener('touchstart', (e) => {
            this.handleTouchStart(e, tile);
        }, { passive: false });
        
        tileElement.addEventListener('touchmove', (e) => {
            this.handleTouchMove(e, tile);
        }, { passive: false });
        
        tileElement.addEventListener('touchend', (e) => {
            this.handleTouchEnd(e, tile);
        }, { passive: false });
    },
    
    handleDragStart(e, tile) {
        this.state.draggedTile = tile;
        e.target.classList.add('dragging');
        e.dataTransfer.effectAllowed = 'move';
        e.dataTransfer.setData('text/plain', tile.id);
        
        console.log(`Started dragging tile: ${tile.letter} (${tile.points} points)`);
    },
    
    handleDragEnd(e, tile) {
        e.target.classList.remove('dragging');
        this.state.draggedTile = null;
        
        document.querySelectorAll('.valid-drop, .invalid-drop').forEach(el => {
            el.classList.remove('valid-drop', 'invalid-drop');
        });
    },
    
    handleTouchStart(e, tile) {
        e.preventDefault();
        this.state.draggedTile = tile;
        const touch = e.touches[0];
        
        const tileElement = e.target.closest('.tile');
        tileElement.classList.add('dragging-active');
        
        tileElement.style.position = 'fixed';
        tileElement.style.zIndex = '1000';
        tileElement.style.left = `${touch.clientX - 25}px`;
        tileElement.style.top = `${touch.clientY - 25}px`;
    },
    
    handleTouchMove(e, tile) {
        e.preventDefault();
        const touch = e.touches[0];
        const tileElement = e.target.closest('.tile');
        
        if (tileElement) {
            tileElement.style.left = `${touch.clientX - 25}px`;
            tileElement.style.top = `${touch.clientY - 25}px`;
            
            const elementBelow = document.elementFromPoint(touch.clientX, touch.clientY);
            if (elementBelow && elementBelow.classList.contains('board-square')) {
                this.highlightDropTarget(elementBelow);
            }
        }
    },
    
    handleTouchEnd(e, tile) {
        e.preventDefault();
        const tileElement = e.target.closest('.tile');
        const touch = e.changedTouches[0];
        
        tileElement.classList.remove('dragging-active');
        tileElement.style.position = '';
        tileElement.style.zIndex = '';
        tileElement.style.left = '';
        tileElement.style.top = '';
        
        const elementBelow = document.elementFromPoint(touch.clientX, touch.clientY);
        
        if (elementBelow) {
            if (elementBelow.classList.contains('board-square')) {
                this.handleTileDrop(elementBelow, tile);
            } else if (elementBelow.classList.contains('tile-slot')) {
                this.returnTileToRack(tile, elementBelow);
            }
        }
        
        this.state.draggedTile = null;
        document.querySelectorAll('.valid-drop, .invalid-drop').forEach(el => {
            el.classList.remove('valid-drop', 'invalid-drop');
        });
    },
    
    highlightDropTarget(square) {
        document.querySelectorAll('.valid-drop, .invalid-drop').forEach(el => {
            el.classList.remove('valid-drop', 'invalid-drop');
        });
        
        const row = parseInt(square.dataset.row);
        const col = parseInt(square.dataset.col);
        
        if (this.canPlaceTile(row, col)) {
            square.classList.add('valid-drop');
        } else {
            square.classList.add('invalid-drop');
        }
    },
    
    canPlaceTile(row, col) {
        return this.state.board[row][col].tile === null;
    },
    
    placeTileOnBoard(tile, row, col) {
        if (!this.canPlaceTile(row, col)) {
            return false;
        }
        
        if (tile.location === 'rack') {
            this.state.playerRack[tile.rackIndex] = null;
            delete tile.rackIndex;
        } else if (tile.location === 'board') {
            this.state.board[tile.boardRow][tile.boardCol].tile = null;
        }
        
        this.state.board[row][col].tile = tile;
        tile.location = 'board';
        tile.boardRow = row;
        tile.boardCol = col;
        
        const square = document.querySelector(`.board-square[data-row="${row}"][data-col="${col}"]`);
        const tileElement = document.getElementById(tile.id);
        
        if (square && tileElement) {
            square.appendChild(tileElement);
            square.classList.add('occupied');
        }
        
        console.log(`Placed ${tile.letter} at ${this.getCoordinateString(row, col)}`);
        return true;
    },
    
    returnTileToRack(tile, slot) {
        const slotIndex = parseInt(slot.dataset.slotIndex);
        
        if (this.state.playerRack[slotIndex] !== null) {
            return false;
        }
        
        if (tile.location === 'board') {
            const square = document.querySelector(
                `.board-square[data-row="${tile.boardRow}"][data-col="${tile.boardCol}"]`
            );
            if (square) {
                square.classList.remove('occupied');
            }
            this.state.board[tile.boardRow][tile.boardCol].tile = null;
            delete tile.boardRow;
            delete tile.boardCol;
        } else if (tile.location === 'rack' && tile.rackIndex !== slotIndex) {
            this.state.playerRack[tile.rackIndex] = null;
        }
        
        tile.location = 'rack';
        tile.rackIndex = slotIndex;
        this.state.playerRack[slotIndex] = tile;
        
        const tileElement = document.getElementById(tile.id);
        if (slot && tileElement) {
            slot.appendChild(tileElement);
        }
        
        return true;
    },
    
    handleTileDrop(square, tile) {
        const row = parseInt(square.dataset.row);
        const col = parseInt(square.dataset.col);
        
        this.placeTileOnBoard(tile, row, col);
    },
    
    getTileAtPosition(row, col) {
        if (row < 0 || row >= this.config.boardSize || 
            col < 0 || col >= this.config.boardSize) {
            return null;
        }
        return this.state.board[row][col].tile;
    },
    
    shuffleRackTiles() {
        const rackTiles = this.state.playerRack.filter(tile => tile !== null);
        const emptySlots = [];
        
        for (let i = 0; i < this.config.tilesPerPlayer; i++) {
            if (this.state.playerRack[i] === null) {
                emptySlots.push(i);
            }
            this.state.playerRack[i] = null;
        }
        
        for (let i = rackTiles.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [rackTiles[i], rackTiles[j]] = [rackTiles[j], rackTiles[i]];
        }
        
        const availableSlots = [...Array(this.config.tilesPerPlayer).keys()];
        rackTiles.forEach((tile, index) => {
            const slotIndex = availableSlots[index];
            this.state.playerRack[slotIndex] = tile;
            tile.rackIndex = slotIndex;
            
            const tileElement = document.getElementById(tile.id);
            const slot = document.querySelector(`.tile-slot[data-slot-index="${slotIndex}"]`);
            if (tileElement && slot) {
                slot.appendChild(tileElement);
            }
        });
        
        console.log('Rack tiles shuffled');
    },
    
    refillRack() {
        let tilesDrawn = 0;
        for (let i = 0; i < this.config.tilesPerPlayer; i++) {
            if (this.state.playerRack[i] === null && this.state.tileBag.length > 0) {
                this.drawTileToRack(i);
                tilesDrawn++;
            }
        }
        console.log(`Drew ${tilesDrawn} new tiles. ${this.state.tileBag.length} tiles remaining in bag.`);
    },
    
    setupEventListeners() {
        const squares = document.querySelectorAll('.board-square');
        squares.forEach(square => {
            square.addEventListener('click', (e) => {
                const row = parseInt(e.target.dataset.row);
                const col = parseInt(e.target.dataset.col);
                const coords = this.getCoordinateString(row, col);
                console.log(`Square clicked: ${coords} (row: ${row}, col: ${col})`);
            });
            
            square.addEventListener('dragover', (e) => {
                e.preventDefault();
                if (this.state.draggedTile) {
                    this.highlightDropTarget(square);
                }
            });
            
            square.addEventListener('drop', (e) => {
                e.preventDefault();
                if (this.state.draggedTile) {
                    this.handleTileDrop(square, this.state.draggedTile);
                }
            });
            
            square.addEventListener('dragleave', (e) => {
                square.classList.remove('valid-drop', 'invalid-drop');
            });
        });
        
        const rackSlots = document.querySelectorAll('.tile-slot');
        rackSlots.forEach(slot => {
            slot.addEventListener('dragover', (e) => {
                e.preventDefault();
            });
            
            slot.addEventListener('drop', (e) => {
                e.preventDefault();
                if (this.state.draggedTile) {
                    this.returnTileToRack(this.state.draggedTile, slot);
                }
            });
        });
        
        const drawBtn = document.getElementById('draw-tiles-btn');
        if (drawBtn) {
            drawBtn.addEventListener('click', () => {
                this.refillRack();
            });
        }
        
        const shuffleBtn = document.getElementById('shuffle-tiles-btn');
        if (shuffleBtn) {
            shuffleBtn.addEventListener('click', () => {
                this.shuffleRackTiles();
            });
        }
    },
    
    getCoordinateString(row, col) {
        const colLetter = String.fromCharCode(65 + col);
        const rowNumber = row + 1;
        return `${colLetter}${rowNumber}`;
    },
    
    getSquareType(row, col) {
        const { specialSquares } = this.config;
        
        if (row === 7 && col === 7) {
            return 'center';
        }
        
        for (const [r, c] of specialSquares.tripleWord) {
            if (r === row && c === col) return 'triple-word';
        }
        
        for (const [r, c] of specialSquares.doubleWord) {
            if (r === row && c === col) return 'double-word';
        }
        
        for (const [r, c] of specialSquares.tripleLetter) {
            if (r === row && c === col) return 'triple-letter';
        }
        
        for (const [r, c] of specialSquares.doubleLetter) {
            if (r === row && c === col) return 'double-letter';
        }
        
        return 'regular';
    },
    
    getSquareLabel(type) {
        const labels = {
            'triple-word': '3W',
            'double-word': '2W',
            'triple-letter': '3L',
            'double-letter': '2L',
            'center': '★',
            'regular': ''
        };
        return labels[type] || '';
    },
    
    createBoard() {
        const gameContainer = document.getElementById('game-container');
        if (!gameContainer) return;
        
        gameContainer.innerHTML = '';
        
        const boardWrapper = document.createElement('div');
        boardWrapper.className = 'board-wrapper';
        
        const boardGrid = document.createElement('div');
        boardGrid.className = 'board-grid';
        boardGrid.id = 'scrabble-board';
        
        this.addCoordinateLabels(boardWrapper, 'top');
        
        const boardWithSideLabels = document.createElement('div');
        boardWithSideLabels.className = 'board-with-labels';
        
        this.addCoordinateLabels(boardWithSideLabels, 'left');
        boardWithSideLabels.appendChild(boardGrid);
        this.addCoordinateLabels(boardWithSideLabels, 'right');
        
        boardWrapper.appendChild(boardWithSideLabels);
        this.addCoordinateLabels(boardWrapper, 'bottom');
        
        this.state.board = [];
        for (let row = 0; row < this.config.boardSize; row++) {
            this.state.board[row] = [];
            for (let col = 0; col < this.config.boardSize; col++) {
                const square = document.createElement('div');
                square.className = 'board-square';
                square.dataset.row = row;
                square.dataset.col = col;
                
                const squareType = this.getSquareType(row, col);
                square.classList.add(squareType);
                
                const label = this.getSquareLabel(squareType);
                if (label) {
                    const labelSpan = document.createElement('span');
                    labelSpan.className = 'square-label';
                    labelSpan.textContent = label;
                    square.appendChild(labelSpan);
                }
                
                boardGrid.appendChild(square);
                this.state.board[row][col] = {
                    tile: null,
                    type: squareType
                };
            }
        }
        
        gameContainer.appendChild(boardWrapper);
        console.log('15x15 Scrabble board created successfully');
    },
    
    addCoordinateLabels(container, position) {
        const labelContainer = document.createElement('div');
        labelContainer.className = `coordinate-labels ${position}`;
        
        if (position === 'top' || position === 'bottom') {
            labelContainer.innerHTML = '<div class="label-spacer"></div>';
            for (let i = 0; i < this.config.boardSize; i++) {
                const label = document.createElement('div');
                label.className = 'coordinate-label';
                label.textContent = String.fromCharCode(65 + i);
                labelContainer.appendChild(label);
            }
            labelContainer.innerHTML += '<div class="label-spacer"></div>';
        } else {
            for (let i = 0; i < this.config.boardSize; i++) {
                const label = document.createElement('div');
                label.className = 'coordinate-label';
                label.textContent = i + 1;
                labelContainer.appendChild(label);
            }
        }
        
        container.appendChild(labelContainer);
    },
    
    startGame() {
        console.log('Start game method ready for implementation');
    },
    
    endTurn() {
        console.log('End turn method ready for implementation');
    },
    
    calculateScore(word) {
        console.log('Score calculation method ready for implementation');
        return 0;
    },
    
    validateWord(word) {
        console.log('Word validation method ready for implementation');
        return false;
    }
};

document.addEventListener('DOMContentLoaded', () => {
    console.log('DOM Content Loaded - Initializing Scrabble Game');
    
    ScrabbleGame.init();
    
    window.ScrabbleGame = ScrabbleGame;
});