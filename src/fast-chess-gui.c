/*
 ============================================================================
 Name        : fast-chess-gui.c
 Author      : Frederico Jordan <fredericojordan@gmail.com>
 Version     :
 Copyright   : Copyright (c) 2016 Frederico Jordan
 Description : Graphical user interface for simple chess game!
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#include "fast-chess.h"

int TILE_SIDE = 60;

char COLOR_SQUEMES[][6] = {
							{ 240, 217, 181, 181, 136,  99 }, // LICHESS
							{ 255, 255, 255, 240, 240, 240 }, // WHITE
							{ 240, 240, 240, 200, 200, 200 }, // LIGHT_GRAY
							{ 164, 164, 164, 136, 136, 136 }, // LICHESS_GRAY
							{ 140, 184, 219,  91, 131, 159 }, // ROYAL BLUE
							{ 255, 255, 255, 140, 184, 219 }, // WHITE/BLUE
							{ 212, 202, 190, 100,  92,  89 }, // CHESSWEBSITE
						  };
int bgColorNum = -1;
char BG_COLOR[6];

int checkColor[] = { 0xFF, 0, 0, 0x80 };
int lastMoveColor[] = {0x22, 0xFF, 0xAA, 0x80};

BOOL heatmap = FALSE;
int atkColor[] = { 0xFF, 0, 0 };
int defColor[] = { 0, 0xFF, 0 };
int heatmapTransparency = 0x30;

SDL_Window* window = NULL; 			// The window we'll be rendering to
SDL_Surface* screenSurface = NULL; 	// The surface contained by the window
SDL_Renderer* renderer = NULL;      // The main renderer

SDL_Texture *bgTexture, *whiteBgTexture, *checkSquare, *lastMoveSquare, *heatmapAtkSquare, *heatmapDefSquare;
SDL_Texture *bPawn, *bKnight, *bBishop, *bRook, *bQueen, *bKing;
SDL_Texture *wPawn, *wKnight, *wBishop, *wRook, *wQueen, *wKing;

TTF_Font *font;

SDL_Texture * loadImage(char * fileLocation) {
	SDL_Surface* imgSurface = NULL;
    imgSurface = IMG_Load( fileLocation );

    if( imgSurface == NULL ) {
        printf( "Unable to load image %s! SDL Error: %s\n", fileLocation, SDL_GetError() );
        return NULL;
    }

    SDL_Texture * dstTexture = SDL_CreateTextureFromSurface(renderer, imgSurface);
    SDL_FreeSurface( imgSurface );

    return dstTexture;
}

void loadImages(void) {
	bPawn   = loadImage("images/black_pawn.png");
	bKnight = loadImage("images/black_knight.png");
	bBishop = loadImage("images/black_bishop.png");
	bRook   = loadImage("images/black_rook.png");
	bQueen  = loadImage("images/black_queen.png");
	bKing   = loadImage("images/black_king.png");

	wPawn   = loadImage("images/white_pawn.png");
	wKnight = loadImage("images/white_knight.png");
	wBishop = loadImage("images/white_bishop.png");
	wRook   = loadImage("images/white_rook.png");
	wQueen  = loadImage("images/white_queen.png");
	wKing   = loadImage("images/white_king.png");
}

void close() {
    SDL_FreeSurface( screenSurface );
    screenSurface = NULL;

    SDL_DestroyWindow( window );
    window = NULL;
    SDL_Quit();
}

SDL_Rect index2rect(int index, char color) {
	int file = index%8;
	int rank = index/8;

	SDL_Rect tile;

	if (color == WHITE) {
		tile.x = file*TILE_SIDE;
		tile.y = (7-rank)*TILE_SIDE;
		tile.w = TILE_SIDE;
		tile.h = TILE_SIDE;
	} else if (color == BLACK) {
		tile.x = (7-file)*TILE_SIDE;
		tile.y = rank*TILE_SIDE;
		tile.w = TILE_SIDE;
		tile.h = TILE_SIDE;
	}

	return tile;
}

SDL_Rect bb2rect(Bitboard bb, char color) {
	return index2rect(bb2index(bb), color);
}

int xy2index(int x, int y, char color) {
	int file=0, rank=0;
	if (color == WHITE) {
		file = (int) (x/TILE_SIDE);
		rank = (int) (7 - (y/TILE_SIDE));
	} else if (color == BLACK) {
		file = (int) (7 - (x/TILE_SIDE));
		rank = (int) (y/TILE_SIDE);
	}
	return 8*rank + file;
}

void int2str(int index) {
	int file = index%8;
	int rank = index/8;

	printf("%c%c", FILES[file], RANKS[rank]);
	fflush(stdout);
}

void xy2str(int x, int y, char color){
	int2str(xy2index(x, y, color));
}

void paintTile(SDL_Surface * destSurface, int position, char color[], char toMove) {
	SDL_Rect tile = index2rect(position, toMove);
	SDL_FillRect( destSurface, &tile, SDL_MapRGB( destSurface->format, color[0], color[1], color[2] ) );
}

SDL_Surface * createBoardSurface(char colors[]) {
	const SDL_PixelFormat fmt = *(screenSurface->format);
	SDL_Surface * bgSurface = SDL_CreateRGBSurface(0, 8*TILE_SIDE, 8*TILE_SIDE, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );

	SDL_FillRect( bgSurface, NULL, SDL_MapRGB( bgSurface->format, colors[0], colors[1], colors[2] ) );

	int i;
	for ( i=0; i<NUM_SQUARES; i++)
		if ( index2bb(i) & DARK_SQUARES )
			paintTile(bgSurface, i, &colors[3], WHITE);

	return bgSurface;
}

void loadBackground(void) {
	SDL_Surface * bgSurface = createBoardSurface(BG_COLOR);

	SDL_DestroyTexture(bgTexture);
	bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);

	SDL_FreeSurface( bgSurface );
}

void loadWhiteBackground(void) {
	const SDL_PixelFormat fmt = *(screenSurface->format);
	SDL_Surface * bgSurface = SDL_CreateRGBSurface(0, 8*TILE_SIDE, 8*TILE_SIDE, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );
	SDL_FillRect( bgSurface, NULL, SDL_MapRGB( bgSurface->format, 255, 255, 255 ) );
	SDL_DestroyTexture(whiteBgTexture);
	whiteBgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
	SDL_FreeSurface( bgSurface );
}

void renderWhiteBackground(void) {
	SDL_Rect boardRect;
	boardRect.x = 0;
	boardRect.y = 0;
	boardRect.w = 8*TILE_SIDE;
	boardRect.h = 8*TILE_SIDE;
	SDL_RenderCopy(renderer, whiteBgTexture, NULL, &boardRect);
}

void renderBackground(void) {
	SDL_Rect boardRect;
	boardRect.x = 0;
	boardRect.y = 0;
	boardRect.w = 8*TILE_SIDE;
	boardRect.h = 8*TILE_SIDE;
	SDL_RenderCopy(renderer, bgTexture, NULL, &boardRect);
}

void renderAlgebricNotation(char color) {
	if ( TILE_SIDE < 40 )
		return;

	const char * const FILES_STR[8] = {"a", "b", "c", "d", "e", "f", "g", "h" };
	const char * const RANKS_STR[8] = {"1", "2", "3", "4", "5", "6", "7", "8" };
	int i;

	for (i=0; i<8; i++) {
		SDL_Color text_color = {BG_COLOR[0+3*(i%2)], BG_COLOR[1+3*(i%2)], BG_COLOR[2+3*(i%2)]};
		SDL_Surface* messageSurface = TTF_RenderText_Solid(font, color==WHITE?RANKS_STR[i]:RANKS_STR[7-i], text_color);
		if ( messageSurface == NULL )
			continue;


		SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
		SDL_FreeSurface( messageSurface );

		SDL_Rect tile = index2rect(8*i, WHITE);
		tile.w /= 6;
		tile.h /= 4;
		SDL_RenderCopy(renderer, messageTexture, NULL, &tile);
		SDL_DestroyTexture(messageTexture);

		messageSurface = TTF_RenderText_Solid(font, color==WHITE?FILES_STR[i]:FILES_STR[7-i], text_color);
		if ( messageSurface == NULL )
			continue;

		messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
		SDL_FreeSurface( messageSurface );

		tile = index2rect(i, WHITE);
		tile.x += 1;
		tile.y += tile.h*3/4;
		tile.w /= 6;
		tile.h /= 4;
		SDL_RenderCopy(renderer, messageTexture, NULL, &tile);
		SDL_DestroyTexture(messageTexture);
	}
}

SDL_Texture * getPieceTexture(int piece) {
	switch(piece) {
	case BLACK|PAWN:
		return bPawn;

	case BLACK|KNIGHT:
		return bKnight;

	case BLACK|BISHOP:
		return bBishop;

	case BLACK|ROOK:
		return bRook;

	case BLACK|QUEEN:
		return bQueen;

	case BLACK|KING:
		return bKing;

	case WHITE|PAWN:
		return wPawn;

	case WHITE|KNIGHT:
		return wKnight;

	case WHITE|BISHOP:
		return wBishop;

	case WHITE|ROOK:
		return wRook;

	case WHITE|QUEEN:
	return wQueen;

	case WHITE|KING:
		return wKing;
	}

	return NULL;
}

void renderPieces(int board[], char color) {
	int i;
	for (i=0; i<NUM_SQUARES; i++) {
		int piece = board[i];

		if ( piece != EMPTY ) {
			SDL_Rect squareRect = index2rect(i, color);
			SDL_RenderCopy(renderer, getPieceTexture(piece), NULL, &squareRect);
		}
	}
}

void renderCheck(int board[], char color) {
	if (isCheck(board, WHITE)) {
		Bitboard kingPos = getKing(board, WHITE);
		SDL_Rect checkRect = bb2rect(kingPos, color);
		SDL_RenderCopy(renderer, checkSquare, NULL, &checkRect);
	} else if (isCheck(board, BLACK)) {
		Bitboard kingPos = getKing(board, BLACK);
		SDL_Rect checkRect = bb2rect(kingPos, color);
		SDL_RenderCopy(renderer, checkSquare, NULL, &checkRect);
	}
}

void renderLastMove(int lastMove, char color) {
	if ( lastMove != 0 ) {
		SDL_Rect to_Rect = bb2rect(index2bb(getTo(lastMove)), color);
		SDL_Rect from_Rect = bb2rect(index2bb(getFrom(lastMove)), color);
		SDL_RenderCopy(renderer, lastMoveSquare, NULL, &to_Rect);
		SDL_RenderCopy(renderer, lastMoveSquare, NULL, &from_Rect);
	}
}

void renderRegularBoard(int board[], char color, Move lastMove) {
	SDL_RenderClear(renderer);
	renderBackground();
	renderAlgebricNotation(color);
	renderCheck(board, color);
	renderLastMove(lastMove, color);
	renderPieces(board, color);
	SDL_RenderPresent(renderer);
}

void renderHeatTiles(int board[], char color) {
	int atkValue, i, j;

	for (i=0; i<NUM_SQUARES; i++) {
		Bitboard target = index2bb(i);
		atkValue = countAttacks(target, board, color) - countAttacks(target, board, opponent(color));

		SDL_Rect targetRect = bb2rect(target, color);

		if ( atkValue < 0 ) {
			for (j=0; j<-atkValue; j++)
				SDL_RenderCopy(renderer, heatmapAtkSquare, NULL, &targetRect);
		} else if ( atkValue > 0 ) {
			for (j=0; j<atkValue; j++)
				SDL_RenderCopy(renderer, heatmapDefSquare, NULL, &targetRect);
		}
	}
}



void renderHeatmapBoard(int board[], char color, Move lastMove) {
	SDL_RenderClear(renderer);
	renderWhiteBackground();
	renderHeatTiles(board, color);
//	renderCheck(board, color);
//	renderLastMove(lastMove, color);
	renderPieces(board, color);
	SDL_RenderPresent(renderer);
}

void renderBoard(int board[], char color, Move lastMove) {
	if (heatmap) {
		renderHeatmapBoard(board, color, lastMove);
	} else {
		renderRegularBoard(board, color, lastMove);
	}
}

void loadHeatTiles(void) {
	const SDL_PixelFormat fmt = *(screenSurface->format);

	SDL_Surface * atkSurf = SDL_CreateRGBSurface(0, 10, 10, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );
	SDL_FillRect( atkSurf, NULL, SDL_MapRGB(screenSurface->format, atkColor[0], atkColor[1], atkColor[2]));
	SDL_DestroyTexture(heatmapAtkSquare);
	heatmapAtkSquare = SDL_CreateTextureFromSurface(renderer, atkSurf);
	SDL_SetTextureBlendMode( heatmapAtkSquare, SDL_BLENDMODE_BLEND );
	SDL_SetTextureAlphaMod( heatmapAtkSquare, heatmapTransparency );
	SDL_FreeSurface( atkSurf );

	SDL_Surface * defSurf = SDL_CreateRGBSurface(0, 10, 10, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );
	SDL_FillRect( defSurf, NULL, SDL_MapRGB(screenSurface->format, defColor[0], defColor[1], defColor[2]));
	SDL_DestroyTexture(heatmapDefSquare);
	heatmapDefSquare = SDL_CreateTextureFromSurface(renderer, defSurf);
	SDL_SetTextureBlendMode( heatmapDefSquare, SDL_BLENDMODE_BLEND );
	SDL_SetTextureAlphaMod( heatmapDefSquare, heatmapTransparency );
	SDL_FreeSurface( defSurf );
}

void loadCheckSquare(void) {
	const SDL_PixelFormat fmt = *(screenSurface->format);
	SDL_Surface * checkSurf = SDL_CreateRGBSurface(0, 10, 10, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask );
	SDL_FillRect( checkSurf, NULL, SDL_MapRGB(&fmt, checkColor[0], checkColor[1], checkColor[2]) );

	SDL_DestroyTexture(checkSquare);
	checkSquare = SDL_CreateTextureFromSurface(renderer, checkSurf);

	SDL_SetTextureBlendMode( checkSquare, SDL_BLENDMODE_BLEND );
	SDL_SetTextureAlphaMod( checkSquare, checkColor[3] );
	SDL_FreeSurface( checkSurf );
}

void loadLastMoveSquare(void) {
	const SDL_PixelFormat fmt = *(screenSurface->format);
	SDL_Surface * lastMoveSurf = SDL_CreateRGBSurface(0, 10, 10, fmt.BitsPerPixel, 0xff000000, 0xff0000, 0xff00, 0xff );

	SDL_FillRect( lastMoveSurf, NULL, SDL_MapRGBA(lastMoveSurf->format, lastMoveColor[0], lastMoveColor[1], lastMoveColor[2], lastMoveColor[3]) );
	SDL_Rect dest = {1,1,8,8};
	SDL_FillRect( lastMoveSurf, &dest, SDL_MapRGBA(lastMoveSurf->format,0,0,0,0) );

	SDL_DestroyTexture(lastMoveSquare);
	lastMoveSquare = SDL_CreateTextureFromSurface(renderer, lastMoveSurf);

	SDL_SetTextureBlendMode( lastMoveSquare, SDL_BLENDMODE_BLEND );
	SDL_FreeSurface( lastMoveSurf );
}

void nextColorScheme(void) {
	int colorCount = (int) (sizeof(COLOR_SQUEMES)/6);

	bgColorNum++;
	bgColorNum %= colorCount;

	int i;
	for (i=0;i<6;i++)
		BG_COLOR[i] = COLOR_SQUEMES[bgColorNum][i];

	loadBackground();
}

void randomColorScheme(void) {
	int colorCount = (int) (sizeof(COLOR_SQUEMES)/6);

	int newColor = rand() % colorCount;
	while ( newColor == bgColorNum ) { newColor = rand() % colorCount; }
	bgColorNum = newColor;

	int i;
	for (i=0;i<6;i++)
		BG_COLOR[i] = COLOR_SQUEMES[bgColorNum][i];

	loadBackground();
}


void loadRandomTintedBackground(void) {
	int i;
	for (i=0; i<3; i++) {
		int value = (rand() % 256);
		BG_COLOR[i]   = (unsigned char) (value + 0.5*(255-value));
		BG_COLOR[i+3] = (unsigned char) value;
	}

	SDL_Surface * bgSurface = createBoardSurface(BG_COLOR);

	SDL_DestroyTexture(bgTexture);
	bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);

	SDL_FreeSurface( bgSurface );
}

void loadRandomBackground(void) {
	int i;
	for (i=0; i<6; i++) {
		BG_COLOR[i]   = (unsigned char) (rand() % 256);
	}

	SDL_Surface * bgSurface = createBoardSurface(BG_COLOR);

	SDL_DestroyTexture(bgTexture);
	bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);

	SDL_FreeSurface( bgSurface );
}

void loadFont() {
	font = TTF_OpenFont("open-sans/OpenSans-Bold.ttf", 48);
}

BOOL init() {
	srand(time(NULL));

    initTablebases();

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return FALSE;
    }

	window = SDL_CreateWindow( "Chess Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 8*TILE_SIDE, 8*TILE_SIDE, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE );

	if( window == NULL ) {
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		return FALSE;
	}

	int imgFlags = IMG_INIT_PNG;

	if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		return FALSE;
	}

	if( TTF_Init() == -1 ) {
	    printf("TTF_Init error: %s\n", TTF_GetError());
	    return FALSE;
	}

	screenSurface = SDL_GetWindowSurface( window );

	renderer = SDL_GetRenderer(window);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	randomColorScheme();
	loadBackground();
	loadWhiteBackground();
	loadCheckSquare();
	loadLastMoveSquare();
	loadImages();
	loadHeatTiles();
	loadFont();

    return TRUE;
}

void setEndTitle(Position * position) {
	if (isCheckmate(position)) {
		if (position->toMove == BLACK) {
			SDL_SetWindowTitle(window, "Chess Game - WHITE wins!");
			printf("WHITE wins!\n");
		} else {
			SDL_SetWindowTitle(window, "Chess Game - BLACK wins!");
			printf("BLACK wins!\n");
		}
	} else if (isStalemate(position)) {
		SDL_SetWindowTitle(window, "Chess Game - Draw by stalemate!");
		printf("Draw by stalemate!\n");
	} else if (hasInsufficientMaterial(position->board)) {
		SDL_SetWindowTitle(window, "Chess Game - Draw by insufficient material!");
		printf("Draw by insufficient material!\n");
	} else if (isOver75MovesRule(position)) {
		SDL_SetWindowTitle(window, "Chess Game - Draw by 75-move rule!");
		printf("Draw by 75-move rule!\n");
	}
	fflush(stdout);
}

void cyclePiece(Game * game, int leavingPos) {
	if (game->position.board[leavingPos] == (WHITE|KING)) {
		game->position.board[leavingPos] = BLACK|PAWN;
	} else {
		game->position.board[leavingPos] += 1;
		game->position.board[leavingPos] %= 15;
	}
}

void handleEvent(SDL_Event event, Game * game, char * color, BOOL * hasAI, int * AIdepth, BOOL * run, BOOL * ongoing, BOOL * editing, int * leavingPos, int * arrivingPos, Move * lastMove) {

	switch (event.type) {
	case SDL_QUIT:
		*run = FALSE;
		break;

	case SDL_MOUSEMOTION:
		break;

	case SDL_MOUSEBUTTONDOWN:
		*leavingPos = xy2index(event.motion.x, event.motion.y, *color);
		break;

	case SDL_MOUSEBUTTONUP:
		*arrivingPos = xy2index(event.motion.x, event.motion.y, *color);

		if (*editing) {
			if (*leavingPos == *arrivingPos) {
				cyclePiece(game, *leavingPos);
			} else {
				movePiece(game->position.board, generateMove(*leavingPos, *arrivingPos));
				renderBoard(game->position.board, *color, *lastMove);
			}
		} else {
			if ( *ongoing && ( !*hasAI || game->position.toMove == *color) ) {
				Move moves[MAX_BRANCHING_FACTOR];
				int moveCount = legalMoves(moves, &(game->position), game->position.toMove);

				int i;
				for (i=0; i<moveCount; i++) {
					if (generateMove(*leavingPos, *arrivingPos) == moves[i]) {
						*lastMove = moves[i];

						printf("Player made move as %s: ", game->position.toMove==WHITE?"white":"black");
						printFullMove(*lastMove, game->position.board);
						printf(".\n");
						fflush(stdout);

						makeMove(game, *lastMove);
						renderBoard(game->position.board, *color, *lastMove);

						if ( hasGameEnded(&(game->position)) ) {
							*ongoing = FALSE;
							setEndTitle(&(game->position));
						}
					}
				}
			}

		}
		break;

	case SDL_KEYDOWN:
		switch( event.key.keysym.sym ) {
		case SDLK_a:
			*hasAI = *hasAI?FALSE:TRUE;
			printf("AI opponent is now %s.\n", *hasAI?"ENABLED":"DISABLED");
			fflush(stdout);
			break;

		case SDLK_c:
			heatmap = FALSE;
			nextColorScheme();
			renderBoard(game->position.board, *color, *lastMove);
			break;

		case SDLK_d:
			dumpContent(game);
			break;

		case SDLK_e:
			*editing = *editing?FALSE:TRUE;
			if (*editing) {
				*lastMove = 0;
			} else {
				game->position.epSquare = -1;
				game->position.castlingRights = CASTLE_KINGSIDE_WHITE|CASTLE_QUEENSIDE_WHITE|CASTLE_KINGSIDE_BLACK|CASTLE_QUEENSIDE_BLACK;
				game->position.halfmoveClock = 0;
				game->position.fullmoveNumber = 1;

				char fen[MAX_FEN_LEN];
				toFen(fen, &(game->position));
				getFenGame(game, fen);
			}
			printf("Editing %s.\n", *editing?"enabled":"disabled");
			fflush(stdout);
			break;

		case SDLK_h:
			heatmap = heatmap?FALSE:TRUE;
			renderBoard(game->position.board, *color, *lastMove);
			printf("Heatmap %s.\n", heatmap?"enabled":"disabled");
			fflush(stdout);
			break;

		case SDLK_i:
			*color = opponent(*color);
			renderBoard(game->position.board, *color, *lastMove);
			printf("Now playing as %s.\n", *color==WHITE?"WHITE":"BLACK");
			fflush(stdout);
			break;

		case SDLK_m:
			printLegalMoves(&(game->position));
			break;

		case SDLK_p:
			dumpPGN(game, *color, *hasAI);
			break;

		case SDLK_q:
		case SDLK_ESCAPE:
			*run = FALSE;
			break;

		case SDLK_r:
			heatmap = FALSE;
			loadRandomBackground();
			renderBoard(game->position.board, *color, *lastMove);
			break;

		case SDLK_s:
//			probe(&game->position);
			printf("Tablebase probing currently not available.\n");
			break;

		case SDLK_t:
			heatmap = FALSE;
			loadRandomTintedBackground();
			renderBoard(game->position.board, *color, *lastMove);
			break;

		case SDLK_u:
			if (*editing) return;
			unmakeMove(game);
			if ( *hasAI ) unmakeMove(game);
			SDL_SetWindowTitle(window, "Chess Game");
			*ongoing = TRUE;
			*lastMove = getLastMove(game);
			renderBoard(game->position.board, *color, *lastMove);
			printf("Last move was undone.\n");
			fflush(stdout);
			break;

		case SDLK_v:
			printf("Board evaluation = %.2f\n", staticEvaluation(&(game->position))/100.0);
			fflush(stdout);
			break;

		case SDLK_UP:
			(*AIdepth)++;
			printf("Search base depth increased to %d.\n", *AIdepth);
			fflush(stdout);
			break;

		case SDLK_DOWN:
			if (*AIdepth==1) {
				printf("Search base depth is 1.\n");
			} else {
				(*AIdepth)--;
				printf("Search base depth decreased to %d.\n", *AIdepth);
			}
			fflush(stdout);
			break;

		default:
//			printf("User pressed key: '%s' key acting as '%s' key\n", SDL_GetScancodeName(event.key.keysym.scancode), SDL_GetKeyName(event.key.keysym.sym));
			printf("Pressing '%s' does nothing!\n", SDL_GetKeyName(event.key.keysym.sym));
			fflush(stdout);
			break;
		}
		break;

		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				if ( event.window.data1 != 8*TILE_SIDE ) {
					TILE_SIDE = (int) (event.window.data1/8);
				} else if ( event.window.data2 != 8*TILE_SIDE ) {
					TILE_SIDE = (int) (event.window.data2/8);
				}
				SDL_SetWindowSize(window, 8*TILE_SIDE, 8*TILE_SIDE);
				renderBoard(game->position.board, *color, *lastMove);
				fflush(stdout);
				break;
			}
	}
}

void play(char color, BOOL hasAI, int AIdepth) {
	hasAI?printf("Playing as %s!\n", color==WHITE?"WHITE":"BLACK"):printf("Playing as both colors!\n");
	fflush(stdout);

	Game game;
	getInitialGame(&game);

	BOOL run = TRUE, ongoing = TRUE, editing = FALSE;
	SDL_Event event;

	int leavingPos = -1, arrivingPos = -1;
	Move lastMove = 0;

	while( run ) {
		renderBoard(game.position.board, color, lastMove);

		if ( hasAI && ongoing && !editing && game.position.toMove == opponent(color) ) {
			SDL_SetWindowTitle(window, "Chess Game - Calculating move...");
			lastMove = getAIMove(&game, AIdepth);
			makeMove(&game, lastMove);
			SDL_SetWindowTitle(window, "Chess Game");
			renderBoard(game.position.board, color, lastMove);

			if ( hasGameEnded(&game.position) ) {
				ongoing = FALSE;
				setEndTitle(&game.position);
			}
		}

		while( SDL_PollEvent( &event ) == 0 );
		handleEvent(event, &game, &color, &hasAI, &AIdepth, &run, &ongoing, &editing, &leavingPos, &arrivingPos, &lastMove);
	}
}

void playAIRandomColor(int depth) {
	char colors[] = {WHITE, BLACK};
	char color = colors[rand()%2];
	play(color, TRUE, depth);
}

void playAlone() { play(WHITE, FALSE, 0); }

int main( int argc, char* args[] ) {
	if ( !init() ) {
		fflush(stdout);
		return EXIT_FAILURE;
	}

	playAIRandomColor(DEFAULT_AI_DEPTH);
//	playAlone();

//	printf("Position\t%d bytes\n", sizeof(Position));
//	printf("Board\t\t%d bytes\n", NUM_SQUARES*sizeof(int));
//	printf("To move\t\t%d bytes\n", sizeof(char));
//	printf("En passant\t%d bytes\n", sizeof(char));
//	printf("Castling\t%d bytes\n", sizeof(char));
//	printf("Half move\t%d bytes\n", sizeof(int));
//	printf("Full move\t%d bytes\n", sizeof(int));
//	fflush(stdout);

	close();

	return EXIT_SUCCESS;
}
