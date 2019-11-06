# cpu_memory_percentage

## Installation

```
git clone https://github.com/erw7/cpu_memory_percentage.git
cd cpu_memory_percentage
g++ -o cpu_memory_percentage.exe main.cpp -lpdh
cp cpu_memory_percentage.exe $HOME/bin
```

## Usage

Set the following in tmux.conf.

```
tmux set-option -g status-right "#($HOME/bin/cpu_memory_percentage.exe)"
```
