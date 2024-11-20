#include <iostream>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <windows.h>

using namespace std;

const int WIDTH = 10;  // Ancho del tablero
const int HEIGHT = 20; // Altura del tablero

int board[HEIGHT][WIDTH] = {0}; // Tablero de juego
bool gameOver = false;

// Piezas del Tetris (matrices de 4x4)
int tetromino[7][4][4] = {
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // I
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // Z
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // S
    {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // T
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // O
    {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // L
    {{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}  // J
};

int currentX = WIDTH / 2 - 2, currentY = 0;
int currentPiece;

// Ocultar cursor
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false; // Ocultar cursor
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

// Mover cursor
void setCursorPosition(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Dibujar tablero con pieza
void drawBoardWithPiece() {
    string buffer;
    int tempBoard[HEIGHT][WIDTH];

    // Copiar tablero
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            tempBoard[y][x] = board[y][x];
        }
    }

    // Dibujar pieza
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino[currentPiece][i][j]) {
                int newX = currentX + j;
                int newY = currentY + i;
                if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT) {
                    tempBoard[newY][newX] = 1;
                }
            }
        }
    }

    // Construir buffer
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buffer += (tempBoard[y][x] != 0) ? "O" : ".";
        }
        buffer += "\n";
    }

    // Dibujar buffer
    setCursorPosition(0, 0);
    cout << buffer;
}

// Verificar colisión
bool checkCollision(int x, int y) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino[currentPiece][i][j]) {
                int newX = x + j;
                int newY = y + i;
                if (newX < 0 || newX >= WIDTH || newY >= HEIGHT || board[newY][newX] != 0)
                    return true;
            }
        }
    }
    return false;
}

// Fijar pieza
void fijarPieza() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetromino[currentPiece][i][j]) {
                board[currentY + i][currentX + j] = 1;
            }
        }
    }
}

// Eliminar líneas completas
void eliminarLineasCompletas() {
    for (int y = HEIGHT - 1; y >= 0; y--) {
        bool completa = true;
        for (int x = 0; x < WIDTH; x++) {
            if (board[y][x] == 0) {
                completa = false;
                break;
            }
        }
        if (completa) {
            // Desplazar líneas hacia abajo
            for (int i = y; i > 0; i--) {
                for (int j = 0; j < WIDTH; j++) {
                    board[i][j] = board[i - 1][j];
                }
            }
            // Limpiar la primera línea
            for (int j = 0; j < WIDTH; j++) {
                board[0][j] = 0;
            }
            y++; // Revisar la misma línea nuevamente
        }
    }
}

// Mover pieza
void moverPieza(int dx, int dy) {
    if (!checkCollision(currentX + dx, currentY + dy)) {
        currentX += dx;
        currentY += dy;
    } else if (dy != 0) { // Colisión abajo
        fijarPieza();
        eliminarLineasCompletas(); // Eliminar líneas completas
        currentPiece = rand() % 7; // Nueva pieza aleatoria
        currentX = WIDTH / 2 - 2;
        currentY = 0;
        if (checkCollision(currentX, currentY)) {
            gameOver = true; // Juego terminado
        }
    }
}

// Rotar pieza
void rotarPieza() {
    int piezaRotada[4][4];
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            piezaRotada[x][3 - y] = tetromino[currentPiece][y][x];
        }
    }
    if (!checkCollision(currentX, currentY)) {
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 4; x++)
                tetromino[currentPiece][y][x] = piezaRotada[y][x];
    }
}

// Bucle principal
void bucleJuego() {
    currentPiece = rand() % 7; // Pieza inicial aleatoria
    while (!gameOver) {
        drawBoardWithPiece(); // Dibuja tablero y pieza
        if (_kbhit()) { // Detectar teclas
            switch (_getch()) {
                case 'a': moverPieza(-1, 0); break; // Izquierda
                case 'd': moverPieza(1, 0); break;  // Derecha
                case 's': moverPieza(0, 1); break;  // Abajo
                case 'w': rotarPieza(); break;      // Rotar
            }
        }
        moverPieza(0, 1); // Caída automática
        Sleep(500); // Velocidad ajustada
    }
    setCursorPosition(0, HEIGHT + 1); // Cursor fuera del tablero
    cout << "¡Juego Terminado!" << endl;
}

int main() {
    srand(time(0)); // Inicializar generador aleatorio
    hideCursor();   // Ocultar cursor
    bucleJuego();   // Iniciar bucle
    return 0;
}
