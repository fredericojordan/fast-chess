import json
import logging
import multiprocessing
import signal
import subprocess
import sys

import lichess_requests as li

LICHESS_USER = "fred-fast-chess"

LOGGER = logging.getLogger(__name__)
LOGGER.setLevel(logging.DEBUG)

handler = logging.StreamHandler(sys.stdout)
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter("%(asctime)s %(levelname)s > %(message)s")
handler.setFormatter(formatter)
LOGGER.addHandler(handler)


MAX_GAMES = 3
TERMINATE = False


def signal_handler(_signal, _frame):
    global TERMINATE
    LOGGER.debug("Received SIGINT. Terminating client.")
    TERMINATE = True


signal.signal(signal.SIGINT, signal_handler)


def watch_event_stream(event_queue):
    LOGGER.debug("Watching incoming events")

    while not TERMINATE:
        try:
            response = li.stream_incoming_events()
            lines = response.iter_lines()
            for line in lines:
                if line:
                    event = json.loads(line.decode("utf-8"))
                    LOGGER.debug(f"Event: {event}")
                    event_queue.put_nowait(event)
        except:
            pass


def watch_game_stream(game_id, event_queue):
    LOGGER.debug(f"Streaming game {game_id}")

    response = li.stream_game(game_id)
    lines = response.iter_lines()

    initial_state = json.loads(next(lines).decode("utf-8"))
    LOGGER.debug(f"{game_id} initial state: {initial_state}")

    if is_my_turn(initial_state["state"], initial_state):
        move = get_fastchess_move_from_movelist(initial_state["state"]["moves"])
        LOGGER.debug(f"{game_id} initial move: {move}")
        li.make_move(initial_state["id"], move)

    while not TERMINATE:
        try:
            binary_chunk = next(lines)
        except StopIteration:
            break

        try:
            if binary_chunk:
                game_event = json.loads(binary_chunk.decode("utf-8"))
                process_game_event(game_event, initial_state)
        except:
            break

    event_queue.put_nowait({"type": "fastchess:gameFinished"})


def process_game_event(event, initial_state):
    LOGGER.debug(f"Game event: {event}")

    event_type = event.get("type")
    if not event_type:
        return

    if event_type == "gameFull":
        pass

    if event_type == "gameState":
        process_game_state(event, initial_state)


def process_challenge(challenge):
    if challenge["challenger"]["id"] == "fredericojordan":
        LOGGER.debug(f"Accepting challenge {challenge['id']}")
        li.accept_challenge(challenge["id"])
        return True

    LOGGER.debug(f"Declining challenge {challenge['id']}")
    li.decline_challenge(challenge["id"])
    return False


def complete_fen(game):
    to_play = "b" if game.get("color") == "black" else "w"
    board_fen = game.get("fen")
    return f"{board_fen} {to_play} - - 0 1"


def process_game_state(game_state, initial_state):
    if not game_state.get("status") == "started":
        return

    if is_my_turn(game_state, initial_state):
        move = get_fastchess_move_from_movelist(game_state["moves"])
        LOGGER.debug(f"Making move on game {initial_state['id']}: {move}")
        li.make_move(initial_state["id"], move)


def is_my_turn(game_state, initial_state):
    my_color = "black" if initial_state["black"]["id"] == LICHESS_USER else "white"
    to_play = "white" if len(game_state["moves"].split()) % 2 == 0 else "black"
    return my_color == to_play


def get_fastchess_move_from_fen(fen):
    LOGGER.debug(f"Fetching move from: {fen}")
    response = subprocess.run(["./chess", "-f", f"{fen}"], capture_output=True)
    return response.stdout.decode("utf-8")


def get_fastchess_move_from_movelist(moves):
    LOGGER.debug(f"Fetching move from: {moves}")
    if moves:
        response = subprocess.run(["./chess", "-m", f"{moves}"], capture_output=True)
    else:
        response = subprocess.run(["./chess", "-m"], capture_output=True)

    return response.stdout.decode("utf-8")


def start():
    manager = multiprocessing.Manager()
    challenges = manager.list()
    event_queue = manager.Queue()
    event_stream = multiprocessing.Process(
        target=watch_event_stream, args=(event_queue,)
    )
    event_stream.start()

    ongoing_games = 0
    queued_games = 0

    with multiprocessing.pool.Pool(MAX_GAMES + 1) as pool:
        while not TERMINATE:
            event = event_queue.get()
            if event["type"] == "fastchess:gameFinished":
                ongoing_games -= 1
            elif event["type"] == "challenge":
                challenges.append(event["challenge"])
            elif event["type"] == "gameStart":
                queued_games -= 1
                ongoing_games += 1
                pool.apply_async(
                    watch_game_stream,
                    [event["game"]["id"], event_queue],
                )

            while (queued_games + ongoing_games) < MAX_GAMES and challenges:
                try:
                    challenge = challenges.pop(0)
                    accepted = process_challenge(challenge)
                    if accepted:
                        queued_games += 1
                except:
                    pass

    event_stream.terminate()
    event_stream.join()


if __name__ == "__main__":
    start()
