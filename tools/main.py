import socket
import time


baseHost = socket.gethostname()
basePort = 5000
baseInterval: int = 1000

if __name__ == "__main__":
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("127.0.0.1", 7000))

    sock.send("first message\0other ".encode())
    time.sleep(1)
    sock.send("message\0".encode())
    time.sleep(1)
    sock.send("one\0two\0\0".encode())
    time.sleep(1)
    sock.send(" three\0".encode())
