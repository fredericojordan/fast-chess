import json
import logging
import os
import signal
import subprocess
from pprint import pprint

import requests
from dotenv import load_dotenv

load_dotenv(verbose=True)

LICHESS_USER = "fred-fast-chess"
LICHESS_TOKEN = os.getenv("LICHESS_TOKEN")
AUTH_HEADER = {"Authorization": f"Bearer {LICHESS_TOKEN}"}

BASE_URL = "https://lichess.org"

TERMINATE = False
LOGGER = logging.getLogger(__name__)


def signal_handler(signal, frame):
    global TERMINATE
    LOGGER.debug("Recieved SIGINT. Terminating client.")
    TERMINATE = True


signal.signal(signal.SIGINT, signal_handler)


def get_my_account():
    return requests.get(f"{BASE_URL}/api/account", headers=AUTH_HEADER)


def upgrade_to_bot():
    return requests.post(f"{BASE_URL}/api/bot/account/upgrade", headers=AUTH_HEADER)


def get_ongoing_games():
    return requests.get(f"{BASE_URL}/api/account/playing", headers=AUTH_HEADER)


def accept_challenge(challenge_id):
    return requests.post(
        f"{BASE_URL}/api/challenge/{challenge_id}/accept", headers=AUTH_HEADER
    )


def make_move(game_id, move):
    LOGGER.debug(f"Making move on game {game_id}: {move}")
    return requests.post(
        f"{BASE_URL}/api/bot/game/{game_id}/move/{move}", headers=AUTH_HEADER
    )


def write_in_chat(game_id, message):
    return requests.post(
        f"{BASE_URL}/api/bot/game/{game_id}/chat",
        data={"room": "player", "text": str(message)},
        headers=AUTH_HEADER,
    )


def stream_incoming_events():
    return requests.get(
        f"{BASE_URL}/api/stream/event", headers=AUTH_HEADER, stream=True,
    )


def stream_game(game_id):
    return requests.get(
        f"{BASE_URL}/api/bot/game/stream/{game_id}", headers=AUTH_HEADER, stream=True,
    )


def watch_event_stream():
    while not TERMINATE:
        try:
            response = stream_incoming_events()
            lines = response.iter_lines()
            for line in lines:
                if line:
                    event = json.loads(line.decode("utf-8"))
                    process_event(event)
        except:
            pass


def watch_game_stream(game_id):
    LOGGER.debug(f"Streaming game {game_id}")
    while not TERMINATE:
        try:
            response = stream_game(game_id)
            lines = response.iter_lines()
            for line in lines:
                if line:
                    game_event = json.loads(line.decode("utf-8"))
                    process_game_event(game_event, game_id)
        except:
            pass


def ongoing_games():
    response = get_ongoing_games()
    if response.status_code == 200:
        for game in response.json().get("nowPlaying"):
            yield game


def process_game_event(event, game_id):
    pprint(event)

    event_type = event.get("type")
    if not event_type:
        return

    if event_type == "gameFull":
        process_game_full(event)

    if event_type == "gameState":
        process_game_state(event, game_id)


def process_event(event):
    pprint(event)

    event_type = event.get("type")
    if not event_type:
        return

    if event_type == "challenge":
        process_challenge(event)

    if event_type == "gameStart":
        watch_game_stream(event["game"]["id"])


def process_challenge(event):
    challenger_id = event["challenge"]["challenger"]["id"]

    if challenger_id == "fredericojordan":
        accept_challenge(event["challenge"]["id"])


def complete_fen(game):
    to_play = "b" if game.get("color") == "black" else "w"
    board_fen = game.get("fen")
    return f"{board_fen} {to_play} - - 0 1"


def process_game_full(game):
    my_color = "black" if game["black"]["id"] == LICHESS_USER else "white"
    to_play = "white" if len(game["state"]["moves"].split()) % 2 == 0 else "black"

    if my_color == to_play:
        move = get_fastchess_move_from_moves(game["state"]["moves"])
        r = make_move(game["id"], move)


def process_game_state(game_state, game_id):
    move = get_fastchess_move_from_moves(game_state["moves"])
    make_move(game_id, move)


def process_ongoing_game(game):
    watch_game_stream(game["gameId"])

    # if game["isMyTurn"]:
    #     fastchess_move = get_fastchess_move_from_fen(complete_fen(game))
    #     make_move(game["gameId"], fastchess_move)


def get_fastchess_move_from_fen(fen):
    response = subprocess.run(["../chess", "-f", f"{fen}"], capture_output=True)
    return response.stdout.decode("utf-8")


def get_fastchess_move_from_moves(moves):
    response = subprocess.run(["../chess", "-m", f"{moves}"], capture_output=True)
    return response.stdout.decode("utf-8")


if __name__ == "__main__":
    for game in ongoing_games():
        process_ongoing_game(game)

    watch_event_stream()
