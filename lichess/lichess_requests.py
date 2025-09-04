import enum
import os

import requests
from dotenv import load_dotenv

load_dotenv(verbose=True)

LICHESS_TOKEN = os.getenv("LICHESS_TOKEN")
AUTH_HEADER = {"Authorization": f"Bearer {LICHESS_TOKEN}"}

BASE_URL = "https://lichess.org/api"


class DeclineReason(enum.StrEnum):
    GENERIC = "generic"  # I'm not accepting challenges at the moment.
    LATER = "later"  # This is not the right time for me, please ask again later.
    TOO_FAST = "tooFast"  # This time control is too fast for me, please challenge again with a slower game.
    TOO_SLOW = "tooSlow"  # This time control is too slow for me, please challenge again with a faster game.
    TIME_CONTROL = "timeControl"  # I'm not accepting challenges with this time control.
    RATED = "rated"  # Please send me a rated challenge instead.
    CASUAL = "casual"  # Please send me a casual challenge instead.
    STANDARD = "standard"  # I'm not accepting variant challenges right now.
    VARIANT = "variant"  # I'm not willing to play this variant right now.
    NO_BOT = "noBot"  # I'm not accepting challenges from bots.
    ONLY_BOT = "onlyBot"  # I'm only accepting challenges from bots.


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


def decline_challenge(challenge_id, reason: DeclineReason | None = None):
    extras = {"json": {"reason": reason}} if reason else {}
    return requests.post(
        f"{BASE_URL}/challenge/{challenge_id}/decline",
        headers=AUTH_HEADER,
        **extras,
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
    return requests.get(
        f"{BASE_URL}/stream/event",
        headers=AUTH_HEADER,
        stream=True,
    )


def stream_game(game_id):
    return requests.get(
        f"{BASE_URL}/bot/game/stream/{game_id}",
        headers=AUTH_HEADER,
        stream=True,
    )
