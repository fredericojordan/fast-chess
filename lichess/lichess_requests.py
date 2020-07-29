import os

import requests
from dotenv import load_dotenv

load_dotenv(verbose=True)

LICHESS_TOKEN = os.getenv("LICHESS_TOKEN")
AUTH_HEADER = {"Authorization": f"Bearer {LICHESS_TOKEN}"}

BASE_URL = "https://lichess.org/api"


def my_account():
    return requests.get(f"{BASE_URL}/account", headers=AUTH_HEADER)


def upgrade_to_bot():
    return requests.post(f"{BASE_URL}/bot/account/upgrade", headers=AUTH_HEADER)


def ongoing_games():
    return requests.get(f"{BASE_URL}/account/playing", headers=AUTH_HEADER)


def accept_challenge(challenge_id):
    return requests.post(
        f"{BASE_URL}/challenge/{challenge_id}/accept", headers=AUTH_HEADER
    )


def decline_challenge(challenge_id):
    return requests.post(
        f"{BASE_URL}/challenge/{challenge_id}/decline", headers=AUTH_HEADER
    )


def make_move(game_id, move):
    return requests.post(
        f"{BASE_URL}/bot/game/{game_id}/move/{move}", headers=AUTH_HEADER
    )


def write_in_chat(game_id, message):
    return requests.post(
        f"{BASE_URL}/bot/game/{game_id}/chat",
        data={"room": "player", "text": str(message)},
        headers=AUTH_HEADER,
    )


def stream_incoming_events():
    return requests.get(f"{BASE_URL}/stream/event", headers=AUTH_HEADER, stream=True,)


def stream_game(game_id):
    return requests.get(
        f"{BASE_URL}/bot/game/stream/{game_id}", headers=AUTH_HEADER, stream=True,
    )
