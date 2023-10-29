from gornilo import Verdict


def raise_wrong_data_exc(obj, actual):
    raise VerdictException(
        Verdict.CORRUPT(f"Received incorrect object {obj}"),
        f"Actual object is: {actual}",
    )


def raise_recently_not_found_error(obj, actual):
    raise VerdictException(
        Verdict.CORRUPT(f"Recently added object {obj} not found"),
        f"Actual object is: {actual}",
    )


def raise_grpc_error(msg: str):
    raise VerdictException(
        Verdict.DOWN(msg)
    )


def raise_invalid_grpc_code_error(verdict_msg, message):
    raise VerdictException(
        Verdict.MUMBLE(verdict_msg),
        message,
    )


class VerdictException(Exception):
    def __init__(self, verdict=None, message=None):
        self.verdict = verdict
        self.message = message

    def __str__(self):
        return f'{str(self.verdict._public_message)} : {self.message}'
