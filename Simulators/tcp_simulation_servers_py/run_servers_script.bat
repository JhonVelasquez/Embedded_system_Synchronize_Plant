@echo off

start /B
start cmd /k "echo FG1 SERVER:  && C:/USERS/JVLSQ/.PYENV/PYENV-WIN/VERSIONS/3.7.5/PYTHON.EXE e:/AGH/Thesis/tcp_simulation_servers_py/tcp_server_fg1.py"

start /B
start cmd /k "echo RPI SERVER: && C:/USERS/JVLSQ/.PYENV/PYENV-WIN/VERSIONS/3.7.5/PYTHON.EXE e:/AGH/Thesis/tcp_simulation_servers_py/tcp_server_rpi.py"

start /B
start cmd /k "echo KEYENCE SERVER: && C:/USERS/JVLSQ/.PYENV/PYENV-WIN/VERSIONS/3.7.5/PYTHON.EXE e:/AGH/Thesis/tcp_simulation_servers_py/tcp_server_keyence.py"