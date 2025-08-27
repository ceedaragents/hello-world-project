'use strict';

const ScrabbleGame = {
    config: {
        boardSize: 15,
        tilePoints: {
            A: 1, B: 3, C: 3, D: 2, E: 1, F: 4, G: 2, H: 4,
            I: 1, J: 8, K: 5, L: 1, M: 3, N: 1, O: 1, P: 3,
            Q: 10, R: 1, S: 1, T: 1, U: 1, V: 4, W: 4, X: 8,
            Y: 4, Z: 10
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
        score: {}
    },
    
    init() {
        console.log('Scrabble game initialized');
        
        this.state.isInitialized = true;
        this.createBoard();
        this.setupEventListeners();
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
        });
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