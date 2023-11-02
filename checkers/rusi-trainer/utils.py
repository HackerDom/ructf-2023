import random
import string
from typing import Callable

import gornilo
import requests
import api


def wrap_exceptions(
        action: Callable[[gornilo.CheckRequest], gornilo.Verdict],
        request: gornilo.CheckRequest,
) -> gornilo.Verdict:
    try:
        return action(request)
    except api.APIProtocolError as error:
        return gornilo.Verdict.MUMBLE(f'protocol error: {error}')
    except api.APIValidationError as error:
        return gornilo.Verdict.MUMBLE(f'validation error: {error}')
    except requests.ConnectionError:
        return gornilo.Verdict.DOWN('connection error')
    except requests.Timeout:
        return gornilo.Verdict.DOWN('timeout error')
    except requests.RequestException:
        return gornilo.Verdict.MUMBLE('http error')
    except requests.JSONDecodeError:
        return gornilo.Verdict.MUMBLE('JSON parse error')


def random_string(length: int, alpha: str = string.ascii_letters + string.digits) -> str:
    symbols = random.choices(alpha, k=length)
    return ''.join(symbols)

