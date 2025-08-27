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
        tileIdCounter: 0,
        dictionary: null,
        currentMove: {
            tiles: [],
            words: [],
            score: 0
        },
        validationCache: new Map()
    },
    
    init() {
        console.log('Scrabble game initialized');
        
        this.state.isInitialized = true;
        this.state.currentPlayer = 1; // Initialize with Player 1
        this.state.scores = { 1: 0 }; // Initialize score tracking
        this.createBoard();
        this.initializeTileBag();
        this.createTileRack();
        this.setupEventListeners();
        this.drawInitialTiles();
        this.loadDictionary();
        this.updateTileCount();
        this.updatePlayerScoreDisplay();
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
            
            if (!this.state.currentMove.tiles.includes(tile)) {
                this.state.currentMove.tiles.push(tile);
            }
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
        
        // Show tile points animation when placed
        this.showTilePoints(tile, { row, col });
        
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
            
            const tileIndex = this.state.currentMove.tiles.indexOf(tile);
            if (tileIndex > -1) {
                this.state.currentMove.tiles.splice(tileIndex, 1);
            }
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
        this.updateTileCount();
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
        
        const validateBtn = document.getElementById('validate-word-btn');
        if (validateBtn) {
            validateBtn.addEventListener('click', () => {
                this.validateCurrentMove();
            });
        }
        
        const submitBtn = document.getElementById('submit-word-btn');
        if (submitBtn) {
            submitBtn.addEventListener('click', () => {
                this.submitMove();
            });
        }
        
        const clearBtn = document.getElementById('clear-board-btn');
        if (clearBtn) {
            clearBtn.addEventListener('click', () => {
                this.clearCurrentMove();
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
    
    calculateMoveScore() {
        const moveData = {
            words: [],
            totalScore: 0,
            baseScore: 0,
            multipliers: [],
            bingoBonus: false
        };

        // Get all words formed in this move
        const wordsFormed = this.state.currentMove.words;
        if (!wordsFormed || wordsFormed.length === 0) {
            return moveData;
        }

        // Track which tiles are newly placed
        const newTilePositions = new Set();
        this.state.currentMove.tiles.forEach(tile => {
            newTilePositions.add(`${tile.boardRow},${tile.boardCol}`);
        });

        // Calculate score for each word
        wordsFormed.forEach(wordObj => {
            const wordScore = this.calculateWordScore(wordObj, newTilePositions);
            moveData.words.push(wordScore);
            moveData.totalScore += wordScore.total;
        });

        // Check for bingo bonus (using all 7 tiles)
        if (this.checkBingoBonus()) {
            moveData.bingoBonus = true;
            moveData.totalScore += 50;
        }

        moveData.baseScore = moveData.words.reduce((sum, w) => sum + w.baseScore, 0);

        return moveData;
    },

    calculateWordScore(wordObj, newTilePositions) {
        let baseScore = 0;
        let wordMultiplier = 1;
        const letterScores = [];
        const multipliers = [];

        // Process each tile in the word
        wordObj.tiles.forEach(tile => {
            const position = `${tile.boardRow},${tile.boardCol}`;
            const isNewTile = newTilePositions.has(position);
            
            let letterValue = this.config.tilePoints[tile.letter] || 0;
            let letterMultiplier = 1;
            
            // Only apply multipliers to newly placed tiles
            if (isNewTile) {
                const squareType = this.getSquareType(tile.boardRow, tile.boardCol);
                
                switch(squareType) {
                    case 'double-letter':
                        letterMultiplier = 2;
                        multipliers.push({ type: 'DL', position: [tile.boardRow, tile.boardCol] });
                        break;
                    case 'triple-letter':
                        letterMultiplier = 3;
                        multipliers.push({ type: 'TL', position: [tile.boardRow, tile.boardCol] });
                        break;
                    case 'double-word':
                    case 'center': // Center is also a double word score
                        wordMultiplier *= 2;
                        multipliers.push({ type: 'DW', position: [tile.boardRow, tile.boardCol] });
                        break;
                    case 'triple-word':
                        wordMultiplier *= 3;
                        multipliers.push({ type: 'TW', position: [tile.boardRow, tile.boardCol] });
                        break;
                }
            }
            
            const letterScore = letterValue * letterMultiplier;
            baseScore += letterScore;
            
            letterScores.push({
                letter: tile.letter,
                value: letterValue,
                multiplier: letterMultiplier,
                score: letterScore,
                isNew: isNewTile,
                position: [tile.boardRow, tile.boardCol]
            });
        });

        const wordText = wordObj.tiles.map(t => t.letter).join('');
        
        return {
            word: wordText,
            baseScore: baseScore,
            wordMultiplier: wordMultiplier,
            total: baseScore * wordMultiplier,
            letterScores: letterScores,
            multipliers: multipliers,
            direction: wordObj.direction,
            startPosition: [wordObj.startRow, wordObj.startCol]
        };
    },

    getLetterScore(tile, position) {
        const baseValue = this.config.tilePoints[tile.letter] || 0;
        const squareType = this.getSquareType(position.row, position.col);
        
        let multiplier = 1;
        if (squareType === 'double-letter') multiplier = 2;
        else if (squareType === 'triple-letter') multiplier = 3;
        
        return {
            base: baseValue,
            multiplier: multiplier,
            total: baseValue * multiplier,
            squareType: squareType
        };
    },

    checkBingoBonus() {
        // Check if all 7 tiles were used in this move
        return this.state.currentMove.tiles.length === 7;
    },

    updateScoreDisplay(moveData) {
        // Update the score preview
        const scorePreview = document.getElementById('score-preview');
        if (scorePreview) {
            if (moveData && moveData.totalScore > 0) {
                let html = `<div class="score-preview-content">`;
                html += `<div class="score-total">+${moveData.totalScore} points</div>`;
                
                // Show word breakdown
                if (moveData.words && moveData.words.length > 0) {
                    html += `<div class="score-breakdown">`;
                    moveData.words.forEach(word => {
                        html += `<div class="word-score">`;
                        html += `<span class="word-text">${word.word}</span>`;
                        html += `<span class="word-points">${word.baseScore}`;
                        if (word.wordMultiplier > 1) {
                            html += ` × ${word.wordMultiplier}`;
                        }
                        html += ` = ${word.total}</span>`;
                        html += `</div>`;
                    });
                    
                    if (moveData.bingoBonus) {
                        html += `<div class="bingo-bonus">Bingo! +50</div>`;
                    }
                    
                    html += `</div>`;
                }
                
                html += `</div>`;
                scorePreview.innerHTML = html;
                scorePreview.style.display = 'block';
            } else {
                scorePreview.style.display = 'none';
            }
        }

        // Update current player score
        this.updatePlayerScoreDisplay();
    },

    updatePlayerScoreDisplay() {
        const scoreDisplay = document.getElementById('current-score');
        if (scoreDisplay && this.state.currentPlayer) {
            const playerScore = this.state.scores[this.state.currentPlayer] || 0;
            scoreDisplay.textContent = playerScore;
        }
    },

    commitScore(moveData) {
        if (!this.state.currentPlayer || !moveData) return;
        
        // Initialize scores if needed
        if (!this.state.scores) {
            this.state.scores = {};
        }
        
        if (!this.state.scores[this.state.currentPlayer]) {
            this.state.scores[this.state.currentPlayer] = 0;
        }
        
        // Add score to player total
        this.state.scores[this.state.currentPlayer] += moveData.totalScore;
        
        // Add to score history
        this.addToScoreHistory(moveData);
        
        // Update display
        this.updatePlayerScoreDisplay();
        
        // Show score animation
        this.showScoreAnimation(moveData.totalScore);
        
        console.log(`Player ${this.state.currentPlayer} scored ${moveData.totalScore} points. Total: ${this.state.scores[this.state.currentPlayer]}`);
    },

    addToScoreHistory(moveData) {
        if (!this.state.scoreHistory) {
            this.state.scoreHistory = [];
        }
        
        const historyEntry = {
            player: this.state.currentPlayer,
            timestamp: new Date().toISOString(),
            score: moveData.totalScore,
            words: moveData.words.map(w => w.word),
            bingoBonus: moveData.bingoBonus,
            details: moveData
        };
        
        this.state.scoreHistory.push(historyEntry);
        
        // Keep only last 10 entries for display
        if (this.state.scoreHistory.length > 10) {
            this.state.scoreHistory = this.state.scoreHistory.slice(-10);
        }
        
        this.updateScoreHistoryDisplay();
    },

    updateScoreHistoryDisplay() {
        const historyContainer = document.getElementById('score-history');
        if (!historyContainer || !this.state.scoreHistory) return;
        
        const recentHistory = this.state.scoreHistory.slice(-5).reverse();
        
        let html = '<div class="score-history-list">';
        recentHistory.forEach(entry => {
            html += `<div class="history-entry">`;
            html += `<div class="history-player">Player ${entry.player}</div>`;
            html += `<div class="history-words">${entry.words.join(', ')}</div>`;
            html += `<div class="history-score">+${entry.score}`;
            if (entry.bingoBonus) {
                html += ' (Bingo!)';
            }
            html += `</div>`;
            html += `</div>`;
        });
        html += '</div>';
        
        historyContainer.innerHTML = html;
    },

    showScoreAnimation(points) {
        // Create floating score element
        const scoreFloat = document.createElement('div');
        scoreFloat.className = 'score-float';
        scoreFloat.textContent = `+${points}`;
        
        // Position near the board center
        const board = document.getElementById('game-board');
        const rect = board.getBoundingClientRect();
        scoreFloat.style.left = `${rect.left + rect.width / 2}px`;
        scoreFloat.style.top = `${rect.top + rect.height / 2}px`;
        
        document.body.appendChild(scoreFloat);
        
        // Animate and remove
        setTimeout(() => {
            scoreFloat.classList.add('animate');
        }, 10);
        
        setTimeout(() => {
            scoreFloat.remove();
        }, 2000);
    },

    highlightMultipliers(tiles) {
        // Highlight special squares being used
        tiles.forEach(tile => {
            const square = document.querySelector(`[data-row="${tile.boardRow}"][data-col="${tile.boardCol}"]`);
            if (square) {
                const squareType = this.getSquareType(tile.boardRow, tile.boardCol);
                if (squareType !== 'normal') {
                    square.classList.add('multiplier-active');
                    
                    // Remove highlight after animation
                    setTimeout(() => {
                        square.classList.remove('multiplier-active');
                    }, 1500);
                }
            }
        });
    },

    showTilePoints(tile, position) {
        // Show point value floating up when tile is placed
        const square = document.querySelector(`[data-row="${position.row}"][data-col="${position.col}"]`);
        if (!square) return;
        
        const points = this.config.tilePoints[tile.letter] || 0;
        const pointsDisplay = document.createElement('div');
        pointsDisplay.className = 'tile-points-float';
        pointsDisplay.textContent = `+${points}`;
        
        const rect = square.getBoundingClientRect();
        pointsDisplay.style.left = `${rect.left + rect.width / 2}px`;
        pointsDisplay.style.top = `${rect.top}px`;
        
        document.body.appendChild(pointsDisplay);
        
        setTimeout(() => {
            pointsDisplay.classList.add('animate');
        }, 10);
        
        setTimeout(() => {
            pointsDisplay.remove();
        }, 1000);
    },
    
    updateTileCount() {
        const tileCountElement = document.getElementById('tiles-count');
        if (tileCountElement) {
            tileCountElement.textContent = this.state.tileBag.length;
        }
    },
    
    async loadDictionary() {
        try {
            this.showLoadingIndicator('Loading dictionary...');
            
            const cachedDictionary = await this.loadFromCache();
            if (cachedDictionary) {
                this.state.dictionary = cachedDictionary;
                console.log(`Dictionary loaded from cache: ${this.state.dictionary.size} words`);
                this.hideLoadingIndicator();
                return;
            }
            
            const response = await fetch('data/sowpods.txt');
            if (!response.ok) {
                throw new Error(`Failed to load dictionary: ${response.status}`);
            }
            
            const text = await response.text();
            const words = text.trim().split('\n').map(word => word.toUpperCase().trim());
            this.state.dictionary = new Set(words);
            
            await this.saveToCache(words);
            
            console.log(`Dictionary loaded: ${this.state.dictionary.size} words`);
            this.hideLoadingIndicator();
            this.enableGameControls();
        } catch (error) {
            console.error('Failed to load dictionary:', error);
            this.showError('Failed to load dictionary. Some features may not work.');
            this.hideLoadingIndicator();
        }
    },
    
    async loadFromCache() {
        if (!window.indexedDB) return null;
        
        return new Promise((resolve) => {
            const request = indexedDB.open('ScrabbleDictionary', 1);
            
            request.onerror = () => resolve(null);
            
            request.onupgradeneeded = (event) => {
                const db = event.target.result;
                if (!db.objectStoreNames.contains('dictionary')) {
                    db.createObjectStore('dictionary');
                }
            };
            
            request.onsuccess = (event) => {
                const db = event.target.result;
                const transaction = db.transaction(['dictionary'], 'readonly');
                const store = transaction.objectStore('dictionary');
                const getRequest = store.get('sowpods');
                
                getRequest.onsuccess = () => {
                    const data = getRequest.result;
                    if (data && data.words) {
                        resolve(new Set(data.words));
                    } else {
                        resolve(null);
                    }
                };
                
                getRequest.onerror = () => resolve(null);
            };
        });
    },
    
    async saveToCache(words) {
        if (!window.indexedDB) return;
        
        return new Promise((resolve) => {
            const request = indexedDB.open('ScrabbleDictionary', 1);
            
            request.onsuccess = (event) => {
                const db = event.target.result;
                const transaction = db.transaction(['dictionary'], 'readwrite');
                const store = transaction.objectStore('dictionary');
                store.put({ words: words, timestamp: Date.now() }, 'sowpods');
                resolve();
            };
            
            request.onerror = () => resolve();
        });
    },
    
    validateWord(word) {
        if (!this.state.dictionary) {
            console.warn('Dictionary not loaded');
            return false;
        }
        
        if (!word || word.length < 2) {
            return false;
        }
        
        const upperWord = word.toUpperCase().replace(/[^A-Z]/g, '');
        
        if (this.state.validationCache.has(upperWord)) {
            return this.state.validationCache.get(upperWord);
        }
        
        const isValid = this.state.dictionary.has(upperWord);
        this.state.validationCache.set(upperWord, isValid);
        
        return isValid;
    },
    
    findWordsOnBoard() {
        const words = [];
        const currentMoveTiles = this.state.currentMove.tiles;
        
        if (currentMoveTiles.length === 0) {
            return words;
        }
        
        const horizontalWord = this.findHorizontalWord(currentMoveTiles[0]);
        if (horizontalWord && horizontalWord.length > 1) {
            words.push(horizontalWord);
        }
        
        const verticalWord = this.findVerticalWord(currentMoveTiles[0]);
        if (verticalWord && verticalWord.length > 1) {
            words.push(verticalWord);
        }
        
        currentMoveTiles.forEach(tile => {
            const perpHorizontal = this.findHorizontalWord(tile);
            if (perpHorizontal && perpHorizontal.length > 1 && 
                !words.some(w => this.wordsAreEqual(w, perpHorizontal))) {
                words.push(perpHorizontal);
            }
            
            const perpVertical = this.findVerticalWord(tile);
            if (perpVertical && perpVertical.length > 1 && 
                !words.some(w => this.wordsAreEqual(w, perpVertical))) {
                words.push(perpVertical);
            }
        });
        
        return words;
    },
    
    findHorizontalWord(tile) {
        const row = tile.boardRow;
        let startCol = tile.boardCol;
        let endCol = tile.boardCol;
        
        while (startCol > 0 && this.getTileAtPosition(row, startCol - 1)) {
            startCol--;
        }
        
        while (endCol < this.config.boardSize - 1 && this.getTileAtPosition(row, endCol + 1)) {
            endCol++;
        }
        
        const word = {
            tiles: [],
            text: '',
            direction: 'horizontal',
            startRow: row,
            startCol: startCol,
            endRow: row,
            endCol: endCol
        };
        
        for (let col = startCol; col <= endCol; col++) {
            const tileAtPos = this.getTileAtPosition(row, col);
            if (tileAtPos) {
                word.tiles.push(tileAtPos);
                word.text += tileAtPos.letter === 'BLANK' ? '_' : tileAtPos.letter;
            }
        }
        
        return word;
    },
    
    findVerticalWord(tile) {
        const col = tile.boardCol;
        let startRow = tile.boardRow;
        let endRow = tile.boardRow;
        
        while (startRow > 0 && this.getTileAtPosition(startRow - 1, col)) {
            startRow--;
        }
        
        while (endRow < this.config.boardSize - 1 && this.getTileAtPosition(endRow + 1, col)) {
            endRow++;
        }
        
        const word = {
            tiles: [],
            text: '',
            direction: 'vertical',
            startRow: startRow,
            startCol: col,
            endRow: endRow,
            endCol: col
        };
        
        for (let row = startRow; row <= endRow; row++) {
            const tileAtPos = this.getTileAtPosition(row, col);
            if (tileAtPos) {
                word.tiles.push(tileAtPos);
                word.text += tileAtPos.letter === 'BLANK' ? '_' : tileAtPos.letter;
            }
        }
        
        return word;
    },
    
    wordsAreEqual(word1, word2) {
        return word1.startRow === word2.startRow && 
               word1.startCol === word2.startCol &&
               word1.endRow === word2.endRow &&
               word1.endCol === word2.endCol;
    },
    
    validateCurrentMove() {
        const words = this.findWordsOnBoard();
        
        if (words.length === 0) {
            this.showValidationFeedback('No valid words formed', false);
            this.updateScoreDisplay(null);
            return false;
        }
        
        const invalidWords = [];
        const validWords = [];
        
        for (const word of words) {
            if (this.validateWord(word.text)) {
                validWords.push(word);
            } else {
                invalidWords.push(word);
            }
        }
        
        if (invalidWords.length > 0) {
            const invalidTexts = invalidWords.map(w => w.text).join(', ');
            this.showValidationFeedback(`Invalid words: ${invalidTexts}`, false);
            this.highlightInvalidWords(invalidWords);
            this.updateScoreDisplay(null);
            return false;
        }
        
        const validTexts = validWords.map(w => w.text).join(', ');
        this.showValidationFeedback(`Valid words: ${validTexts}`, true);
        this.highlightValidWords(validWords);
        
        this.state.currentMove.words = validWords;
        
        // Calculate and display score preview
        const moveData = this.calculateMoveScore();
        this.updateScoreDisplay(moveData);
        
        return true;
    },
    
    showValidationFeedback(message, isValid) {
        const existingFeedback = document.querySelector('.validation-feedback');
        if (existingFeedback) {
            existingFeedback.remove();
        }
        
        const feedback = document.createElement('div');
        feedback.className = `validation-feedback ${isValid ? 'valid' : 'invalid'}`;
        feedback.textContent = message;
        
        const container = document.querySelector('.game-controls') || document.body;
        container.appendChild(feedback);
        
        setTimeout(() => {
            feedback.classList.add('fade-out');
            setTimeout(() => feedback.remove(), 300);
        }, 3000);
    },
    
    highlightValidWords(words) {
        this.clearWordHighlights();
        
        words.forEach(word => {
            word.tiles.forEach(tile => {
                const square = document.querySelector(
                    `.board-square[data-row="${tile.boardRow}"][data-col="${tile.boardCol}"]`
                );
                if (square) {
                    square.classList.add('valid-word');
                }
            });
        });
    },
    
    highlightInvalidWords(words) {
        this.clearWordHighlights();
        
        words.forEach(word => {
            word.tiles.forEach(tile => {
                const square = document.querySelector(
                    `.board-square[data-row="${tile.boardRow}"][data-col="${tile.boardCol}"]`
                );
                if (square) {
                    square.classList.add('invalid-word');
                }
            });
        });
    },
    
    clearWordHighlights() {
        document.querySelectorAll('.valid-word, .invalid-word').forEach(square => {
            square.classList.remove('valid-word', 'invalid-word');
        });
    },
    
    showLoadingIndicator(message = 'Loading...') {
        const existingLoader = document.querySelector('.loading-indicator');
        if (existingLoader) {
            existingLoader.remove();
        }
        
        const loader = document.createElement('div');
        loader.className = 'loading-indicator';
        loader.innerHTML = `
            <div class="loader-spinner"></div>
            <div class="loader-message">${message}</div>
        `;
        
        document.body.appendChild(loader);
    },
    
    hideLoadingIndicator() {
        const loader = document.querySelector('.loading-indicator');
        if (loader) {
            loader.remove();
        }
    },
    
    showError(message) {
        const error = document.createElement('div');
        error.className = 'error-message';
        error.textContent = message;
        
        document.body.appendChild(error);
        
        setTimeout(() => {
            error.classList.add('fade-out');
            setTimeout(() => error.remove(), 300);
        }, 5000);
    },
    
    enableGameControls() {
        const submitBtn = document.getElementById('submit-word-btn');
        if (submitBtn) {
            submitBtn.disabled = false;
        }
    },
    
    submitMove() {
        if (!this.validateCurrentMove()) {
            this.showValidationFeedback('Please fix invalid words before submitting', false);
            return;
        }
        
        if (this.state.currentMove.tiles.length === 0) {
            this.showValidationFeedback('No tiles placed on board', false);
            return;
        }
        
        // Calculate and commit the score
        const moveData = this.calculateMoveScore();
        this.commitScore(moveData);
        
        // Show feedback with proper scoring details
        let message = `Move submitted! Score: ${moveData.totalScore} points`;
        if (moveData.bingoBonus) {
            message += ' (Bingo!)';
        }
        this.showValidationFeedback(message, true);
        
        // Highlight multipliers used
        this.highlightMultipliers(this.state.currentMove.tiles);
        
        // Clear the move state
        this.state.currentMove.tiles = [];
        this.state.currentMove.words = [];
        this.state.currentMove.score = 0;
        
        this.refillRack();
        this.clearWordHighlights();
        this.updateScoreDisplay(null); // Clear score preview
    },
    
    clearCurrentMove() {
        const tilesToReturn = [...this.state.currentMove.tiles];
        
        tilesToReturn.forEach(tile => {
            for (let i = 0; i < this.config.tilesPerPlayer; i++) {
                if (this.state.playerRack[i] === null) {
                    const slot = document.querySelector(`.tile-slot[data-slot-index="${i}"]`);
                    if (slot) {
                        this.returnTileToRack(tile, slot);
                        break;
                    }
                }
            }
        });
        
        this.state.currentMove.tiles = [];
        this.state.currentMove.words = [];
        this.state.currentMove.score = 0;
        
        this.clearWordHighlights();
        this.showValidationFeedback('Board cleared', true);
    }
};

document.addEventListener('DOMContentLoaded', () => {
    console.log('DOM Content Loaded - Initializing Scrabble Game');
    
    ScrabbleGame.init();
    
    window.ScrabbleGame = ScrabbleGame;
});