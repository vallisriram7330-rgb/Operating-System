import random

MAX_CYL = 199
HEAD = 53
REQUEST_COUNT = 10_000


def head_movement(requests, algorithm):
    pending = list(requests)
    order = []

    if algorithm == "FCFS":
        order = pending[:]

    elif algorithm == "SSTF":
        position = HEAD
        while pending:
            request = min(pending, key=lambda value: abs(value - position))
            order.append(request)
            pending.remove(request)
            position = request

    elif algorithm == "SCAN":
        lower = sorted((x for x in pending if x < HEAD), reverse=True)
        higher = sorted(x for x in pending if x >= HEAD)
        order = lower + [0] + higher

    elif algorithm == "LOOK":
        lower = sorted((x for x in pending if x < HEAD), reverse=True)
        higher = sorted(x for x in pending if x >= HEAD)
        order = lower + higher

    else:
        raise ValueError(f"Unsupported algorithm: {algorithm}")

    position = HEAD
    total = 0
    for request in order:
        total += abs(request - position)
        position = request

    return total, total / len(requests)


def main():
    random.seed(42)

    for load_name, count in (
        ("Light", 100),
        ("Medium", 1_000),
        ("Heavy", REQUEST_COUNT),
    ):
        requests = [random.randint(0, MAX_CYL) for _ in range(count)]
        print(f"\n{load_name} load ({count} requests)")
        for algorithm in ("FCFS", "SSTF", "SCAN", "LOOK"):
            total, average = head_movement(requests, algorithm)
            print(
                f"{algorithm:5s} | Total movement: {total:8d} | "
                f"Average movement/request: {average:.2f}"
            )


if __name__ == "__main__":
    main()
