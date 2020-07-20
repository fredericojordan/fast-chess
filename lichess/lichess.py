import json
import os
import subprocess
from pprint import pprint

import requests
from dotenv import load_dotenv

load_dotenv(verbose=True)

LICHESS_TOKEN = os.getenv("LICHESS_TOKEN")
AUTH_HEADER = {"Authorization": f"Bearer {LICHESS_TOKEN}"}

BASE_URL = "https://lichess.org"


def get_my_account():
    return requests.get(f"{BASE_URL}/api/account", headers=AUTH_HEADER)


def upgrade_to_bot():
    return requests.post(f"{BASE_URL}/api/bot/account/upgrade", headers=AUTH_HEADER)


def get_ongoing_games():
    return requests.get(f"{BASE_URL}/api/account/playing", headers=AUTH_HEADER)


def accept_challenge(challenge_id):
    print(f"ACCEPTING {challenge_id}")
    r = requests.post(
        f"{BASE_URL}/api/challenge/{challenge_id}/accept", headers=AUTH_HEADER
    )
    pprint(r.json())
    return r


def make_move(game_id, move):
    return requests.post(
        f"{BASE_URL}/api/bot/game/{game_id}/move/{move}", headers=AUTH_HEADER
    )


def stream_incoming_events():
    return requests.get(
        f"{BASE_URL}/api/stream/event", headers=AUTH_HEADER, stream=True,
    )


def stream_game(game_id):
    return requests.get(
        f"{BASE_URL}/api/bot/game/stream/{game_id}", headers=AUTH_HEADER, stream=True,
    )


def watch_incoming_events():
    while True:
        try:
            response = stream_incoming_events()
            lines = response.iter_lines()
            for line in lines:
                if line:
                    event = json.loads(line.decode("utf-8"))
                    process_event(event)
        except:
            pass


def watch_incoming_moves(game_id):
    while True:
        try:
            response = stream_game(game_id)
            lines = response.iter_lines()
            for line in lines:
                if line:
                    event_move = json.loads(line.decode("utf-8"))
                    process_move(event_move)
        except:
            pass


def ongoing_games():
    response = get_ongoing_games()
    if response.status_code == 200:
        for game in response.json().get("nowPlaying"):
            yield game


def process_move(move):
    pprint(move)


def process_event(event):
    pprint(event)

    event_type = event.get("type")
    if not event_type:
        return

    if event_type == "challenge":
        process_challenge(event)

    if event_type == "gameStart":
        pass


def process_challenge(event):
    challenger_id = event["challenge"]["challenger"]["id"]

    if challenger_id == "fredericojordan":
        accept_challenge(event["challenge"]["id"])


def complete_fen(game):
    to_play = "b" if game.get("color") == "black" else "w"
    board_fen = game.get("fen")
    return f"{board_fen} {to_play} - - 0 1"


def process_game(game):
    if game.get("isMyTurn"):
        fastchess_move = get_fastchess_move(game)
        make_move(game.get("gameId"), fastchess_move)


def get_fastchess_move(game):
    position_fen = complete_fen(game)
    response = subprocess.run(["../chess", f"{position_fen}"], capture_output=True)
    return response.stdout.decode("utf-8")


if __name__ == "__main__":
    # watch_incoming_events()
    watch_incoming_moves("GEv5ltw2")

    # for game in ongoing_games():
    #     pprint(game)
    #     process_game(game)
