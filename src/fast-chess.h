/*
 * fast-chess.h
 *
 *  Created on: 20 de set de 2016
 *      Author: fvj
 */

#ifndef FAST_CHESS_H_
#define FAST_CHESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define NUM_SQUARES 64

#define COLOR_MASK 1<<3
#define WHITE      0<<3
#define BLACK      1<<3

#define PIECE_MASK 0x7
#define EMPTY  0
#define PAWN   1
#define KNIGHT 2
#define BISHOP 3
#define ROOK   4
#define QUEEN  5
#define KING   6

#define ALL_SQUARES    0xFFFFFFFFFFFFFFFF
#define FILE_A         0x0101010101010101
#define FILE_B         0x0202020202020202
#define FILE_C         0x0404040404040404
#define FILE_D         0x0808080808080808
#define FILE_E         0x1010101010101010
#define FILE_F         0x2020202020202020
#define FILE_G         0x4040404040404040
#define FILE_H         0x8080808080808080
#define RANK_1         0x00000000000000FF
#define RANK_2         0x000000000000FF00
#define RANK_3         0x0000000000FF0000
#define RANK_4         0x00000000FF000000
#define RANK_5         0x000000FF00000000
#define RANK_6         0x0000FF0000000000
#define RANK_7         0x00FF000000000000
#define RANK_8         0xFF00000000000000
#define DIAG_A1H8      0x8040201008040201
#define ANTI_DIAG_H1A8 0x0102040810204080
#define LIGHT_SQUARES  0x55AA55AA55AA55AA
#define DARK_SQUARES   0xAA55AA55AA55AA55

#define CASTLE_KINGSIDE_WHITE  1<<0
#define CASTLE_QUEENSIDE_WHITE 1<<1
#define CASTLE_KINGSIDE_BLACK  1<<2
#define CASTLE_QUEENSIDE_BLACK 1<<3

#define BOOL  char
#define TRUE  1
#define FALSE 0

typedef uint_fast64_t Bitboard;
typedef int Move;

#define MOVE_BUFFER_SIZE 100

#define AI_DEPTH 3

typedef struct Game {
   int board[NUM_SQUARES];
   char toMove;
   char ep_square;
   char castling_rights;
   unsigned int halfmove_clock;
   unsigned int fullmove_number;
} Game;

typedef struct Node {
	Move move;
	int score;
} Node;

static char FILES[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
static char RANKS[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};

static Bitboard FILES_BB[8] = { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
static Bitboard RANKS_BB[8] = { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };

static int INITIAL_BOARD[NUM_SQUARES] = { WHITE|ROOK, WHITE|KNIGHT, WHITE|BISHOP, WHITE|QUEEN, WHITE|KING, WHITE|BISHOP, WHITE|KNIGHT, WHITE|ROOK,
                                          WHITE|PAWN, WHITE|PAWN,   WHITE|PAWN,   WHITE|PAWN,  WHITE|PAWN, WHITE|PAWN,   WHITE|PAWN,   WHITE|PAWN,
                                          EMPTY,      EMPTY,        EMPTY,        EMPTY,       EMPTY,      EMPTY,        EMPTY,        EMPTY,
                                          EMPTY,      EMPTY,        EMPTY,        EMPTY,       EMPTY,      EMPTY,        EMPTY,        EMPTY,
                                          EMPTY,      EMPTY,        EMPTY,        EMPTY,       EMPTY,      EMPTY,        EMPTY,        EMPTY,
                                          EMPTY,      EMPTY,        EMPTY,        EMPTY,       EMPTY,      EMPTY,        EMPTY,        EMPTY,
                                          BLACK|PAWN, BLACK|PAWN,   BLACK|PAWN,   BLACK|PAWN,  BLACK|PAWN, BLACK|PAWN,   BLACK|PAWN,   BLACK|PAWN,
                                          BLACK|ROOK, BLACK|KNIGHT, BLACK|BISHOP, BLACK|QUEEN, BLACK|KING, BLACK|BISHOP, BLACK|KNIGHT, BLACK|ROOK };

static int PIECE_VALUES[] = { 0, 100, 300, 300, 500, 900, 42000 };

#define DOUBLED_PAWN_PENALTY      10
#define ISOLATED_PAWN_PENALTY     20
#define BACKWARDS_PAWN_PENALTY    8
#define PASSED_PAWN_BONUS         20
#define ROOK_SEMI_OPEN_FILE_BONUS 10
#define ROOK_OPEN_FILE_BONUS      15
#define ROOK_ON_SEVENTH_BONUS     20

static int PAWN_BONUS[] = {0,   0,   0,   0,   0,   0,   0,   0,
						   0,   0,   0, -40, -40,   0,   0,   0,
						   1,   2,   3, -10, -10,   3,   2,   1,
						   2,   4,   6,   8,   8,   6,   4,   2,
						   3,   6,   9,  12,  12,   9,   6,   3,
						   4,   8,  12,  16,  16,  12,   8,   4,
						   5,  10,  15,  20,  20,  15,  10,   5,
						   0,   0,   0,   0,   0,   0,   0,   0};

static int KNIGHT_BONUS[] = {-10, -30, -10, -10, -10, -10, -30, -10,
							 -10,   0,   0,   0,   0,   0,   0, -10,
							 -10,   0,   5,   5,   5,   5,   0, -10,
							 -10,   0,   5,  10,  10,   5,   0, -10,
							 -10,   0,   5,  10,  10,   5,   0, -10,
							 -10,   0,   5,   5,   5,   5,   0, -10,
							 -10,   0,   0,   0,   0,   0,   0, -10,
							 -10, -10, -10, -10, -10, -10, -10, -10};

static int BISHOP_BONUS[] = {-10, -10, -20, -10, -10, -20, -10, -10,
							 -10,   0,   0,   0,   0,   0,   0, -10,
							 -10,   0,   5,   5,   5,   5,   0, -10,
							 -10,   0,   5,  10,  10,   5,   0, -10,
							 -10,   0,   5,  10,  10,   5,   0, -10,
							 -10,   0,   5,   5,   5,   5,   0, -10,
							 -10,   0,   0,   0,   0,   0,   0, -10,
							 -10, -10, -10, -10, -10, -10, -10, -10};

static int KING_BONUS[] = { 0,  20,  40, -20,   0, -20,  40,  20,
						  -20, -20, -20, -20, -20, -20, -20, -20,
						  -40, -40, -40, -40, -40, -40, -40, -40,
						  -40, -40, -40, -40, -40, -40, -40, -40,
						  -40, -40, -40, -40, -40, -40, -40, -40,
						  -40, -40, -40, -40, -40, -40, -40, -40,
						  -40, -40, -40, -40, -40, -40, -40, -40,
						  -40, -40, -40, -40, -40, -40, -40, -40};

static int KING_ENDGAME_BONUS[] = { 0,  10,  20,  30,  30,  20,  10,   0,
								   10,  20,  30,  40,  40,  30,  20,  10,
								   20,  30,  40,  50,  50,  40,  30,  20,
								   30,  40,  50,  60,  60,  50,  40,  30,
								   30,  40,  50,  60,  60,  50,  40,  30,
								   20,  30,  40,  50,  50,  40,  30,  20,
								   10,  20,  30,  40,  40,  30,  20,  10,
								    0,  10,  20,  30,  30,  20,  10,   0};

Game getInitialGame(void);

// ========= UTILITY =========

Bitboard index2bb(int index);
int str2index(char *str);
Bitboard str2bb(char *str);
BOOL isSet(Bitboard bb, int index);
Bitboard lsb(Bitboard bb);
Bitboard msb(Bitboard bb);
int bb2index(Bitboard bb);
char getFile(int position);
char getRank(int position);
Move generateMove(int leavingSquare, int arrivingSquare);
int getFrom(Move move);
int getTo(Move move);
char piece2str(int piece);
void printBitboard(Bitboard bitboard);
void printBoard(int board[]);
Bitboard not(Bitboard bb);
char opposingColor(char color);
void flipVertical(int board[]);

// ====== BOARD FILTERS ======

Bitboard getColoredPieces(int board[], char color);
Bitboard getEmptySquares(int board[]);
Bitboard getOccupiedSquares(int board[]);
Bitboard getPieces(int board[], int pieceType);
char countPieces(Bitboard bitboard);

// ======= DIRECTIONS ========

Bitboard east(Bitboard bb);
Bitboard west(Bitboard bb);
Bitboard north(Bitboard bb);
Bitboard south(Bitboard bb);
Bitboard NE(Bitboard bb);
Bitboard NW(Bitboard bb);
Bitboard SE(Bitboard bb);
Bitboard SW(Bitboard bb);
Bitboard WNW(Bitboard moving_piece);
Bitboard ENE(Bitboard moving_piece);
Bitboard NNW(Bitboard moving_piece);
Bitboard NNE(Bitboard moving_piece);
Bitboard ESE(Bitboard moving_piece);
Bitboard WSW(Bitboard moving_piece);
Bitboard SSE(Bitboard moving_piece);
Bitboard SSW(Bitboard moving_piece);

// ========== PAWN ===========

Bitboard getPawns(int board[]);
Bitboard pawnSimplePushes(Bitboard moving_piece, int board[], char color);
Bitboard pawnDoublePushes(Bitboard moving_piece, int board[], char color);
Bitboard pawnPushes(Bitboard moving_piece, int board[], char color);
Bitboard pawnEastAttacks(Bitboard moving_piece, int board[], char color);
Bitboard pawnWestAttacks(Bitboard moving_piece, int board[], char color);
Bitboard pawnAttacks(Bitboard moving_piece, int board[], char color);
Bitboard pawnSimpleCaptures(Bitboard moving_piece, int board[], char color);
Bitboard pawnEpCaptures(Bitboard moving_piece, Game game, char color);
Bitboard pawnCaptures(Bitboard moving_piece, Game game, char color);
Bitboard pawnMoves(Bitboard moving_piece, Game game, char color);
BOOL isDoublePush(int leaving, int arriving);
int getEpSquare(int leaving);
void removeCapturedEp(Game game);
BOOL isOpenFile(Bitboard bb, int board[]);
BOOL isSemiOpenFile(Bitboard bb, int board[]);

// ========== KNIGHT =========

Bitboard getKnights(int board[]);
Bitboard knightAttacks(Bitboard moving_piece);
Bitboard knightMoves(Bitboard moving_piece, int board[], char color);

// ========== KING ===========

Bitboard getKing(int board[], char color);
Bitboard kingAttacks(Bitboard moving_piece);
Bitboard kingMoves(Bitboard moving_piece, int board[], char color) ;
BOOL canCastleKingside(Game game, char color);
BOOL canCastleQueenside(Game game, char color);
char removeCastlingRights(Game game, char removed_rights);

// ========== BISHOP =========

Bitboard getBishops(int board[]);
Bitboard NE_ray(Bitboard bb);
Bitboard SE_ray(Bitboard bb);
Bitboard NW_ray(Bitboard bb);
Bitboard SW_ray(Bitboard bb);
Bitboard NE_attack(Bitboard single_piece, int board[], char color);
Bitboard NW_attack(Bitboard single_piece, int board[], char color);
Bitboard SE_attack(Bitboard single_piece, int board[], char color);
Bitboard SW_attack(Bitboard single_piece, int board[], char color);
Bitboard diagonalAttacks(Bitboard single_piece, int board[], char color);
Bitboard antiDiagonalAttacks(Bitboard single_piece, int board[], char color);
Bitboard bishopAttacks(Bitboard moving_pieces, int board[], char color);
Bitboard bishopMoves(Bitboard moving_piece, int board[], char color);

// ========== ROOK ===========

Bitboard getRooks(int board[]);
Bitboard northRay(Bitboard moving_pieces);
Bitboard southRay(Bitboard moving_pieces);
Bitboard eastRay(Bitboard moving_pieces);
Bitboard westRay(Bitboard moving_pieces);
Bitboard northAttack(Bitboard single_piece, int board[], char color);
Bitboard southAttack(Bitboard single_piece, int board[], char color);
Bitboard fileAttacks(Bitboard single_piece, int board[], char color);
Bitboard eastAttack(Bitboard single_piece, int board[], char color);
Bitboard westAttack(Bitboard single_piece, int board[], char color);
Bitboard rankAttacks(Bitboard single_piece, int board[], char color);
Bitboard rookAttacks(Bitboard moving_piece, int board[], char color);
Bitboard rookMoves(Bitboard moving_piece, int board[], char color);

// ========== QUEEN ==========

Bitboard getQueens(int board[]);
Bitboard queenAttacks(Bitboard moving_piece, int board[], char color);
Bitboard queenMoves(Bitboard moving_piece, int board[], char color);

// ======== MAKE MOVE ========

void movePiece(int board[], Move move);
Game makeMove(Game game, Move move);

// ======== MOVE GEN =========

Bitboard getMoves(Bitboard movingPiece, Game game, char color);
int pseudoLegalMoves(Move * moves, Game game, char color);
Bitboard getAttacks(Bitboard movingPiece, int board[], char color);
int countAttacks(Bitboard target, int board[], char color);
BOOL isAttacked(Bitboard target, int board[], char color);
BOOL isCheck(int board[], char color);
BOOL isLegalMove(Game game, Move move);
int legalMoves(Move * legalMoves, Game game, char color);

// ====== GAME CONTROL =======

BOOL isCheckmate(Game game);
BOOL isStalemate(Game game);
BOOL hasInsufficientMaterial(Game game);
BOOL isEndgame(int board[]);
BOOL isOver75MovesRule(Game game);
BOOL hasGameEnded(Game game);
void printOutcome(Game game);

// ========== EVAL ===========

int winScore(char color);
int materialSum(int board[], char color);
int materialBalance(int board[]);
int positionalBonus(int board[], char color);
int positionalBalance(int board[]);
int evaluateEndNode(Game game);
int evaluateGame(Game game);

// ========= SEARCH ==========

Node simpleEvaluation(Game game);
Node alpha_beta(Game game, char depth, int alpha, int beta);
Move getRandomMove(Game game);
Move getAIMove(Game game);
Move getPlayerMove();

// ========= PLAY LOOP =======

void playWhite();

// ===========================


void tests(void);

#endif /* FAST_CHESS_H_ */
