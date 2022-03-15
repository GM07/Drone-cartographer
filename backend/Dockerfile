ARG VARIANT=3.8
FROM mcr.microsoft.com/vscode/devcontainers/python:${VARIANT}

RUN pip install flask flask-CORS flask-pyMongo flask-socketio gevent gevent-websocket cflib pytest pytest-stub pytest_mock mypy pylint mongomock yapf

RUN apt update

RUN apt -y install curl

RUN curl -sSL https://get.docker.com/ | sh

COPY ./services ./services

COPY ./server.py ./server.py

COPY ./constants.py ./constants.py

EXPOSE 5000

CMD python3 server.py
