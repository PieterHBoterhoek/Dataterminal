# Dataterminal [CMD]

Dataterminal is a custom cli that executes different usefull actions based on commands inside a Json file.

Commands like
- "yt" -> "https://www.youtube.com"
- "gh" -> "https://github.com"
- "phasmo" -> "steam://run/739630"

# Current commands
- Url commands
- Steam games
- Text responses
- help (lists all available commands)
- cir (disables random words)
- --reset (resets json path)
- exit (closes the program)

# Json setup
- Edit the Json file to add/delete commands.
- First assign the shortcut (key) for example "yt".
- Then assign the type so it knows if it is a Url or Steam game.
- And to finish up assign the value for Url just copypaste the Url in there, for steam games search the gameId on [SteamDB](https://steamdb.info/) and paste it in.
- You don't have to recompile.

# Project Setup
- Download this project (or just the cli.exe).
- Compile main.cpp and run it inside your terminal, or just run cli.exe.
- Alternatively you could set a path in your system enviroments and just type the name in CMD then and it would also work.
- Enter your commands.json path.

# Debugging
If you get a error trying to set your json path, first check if it is spelled correctly.
When that does not work try to insert it without quotes.

Example for a correct commands.json path:
C:\Users\yoshi\Documents\Dev\DataTerminalCMD\commands.json

Use --reset to reset the path.
