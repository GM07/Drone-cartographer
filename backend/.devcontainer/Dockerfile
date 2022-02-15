ARG VARIANT=3.8
FROM mcr.microsoft.com/vscode/devcontainers/python:${VARIANT}

RUN pip install flask flask-CORS flask-pyMongo flask-socketio gevent gevent-websocket cflib pytest pytest-stub pytest_mock mypy pylint

EXPOSE 5000
