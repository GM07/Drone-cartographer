ARG VARIANT=3.8
FROM mcr.microsoft.com/vscode/devcontainers/python:${VARIANT}

RUN pip install flask flask-CORS flask-pyMongo flask-socketio gevent gevent-websocket cflib

EXPOSE 5000
