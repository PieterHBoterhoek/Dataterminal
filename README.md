# Dataterminal [CMD]

Dataterminal is a custom cli that executes different usefull actions based on commands inside a Json file.

Commands like
- "yt" -> "https://www.youtube.com" (Url)
- "gh" -> "https://github.com" (Url)
- "phasmo" -> "steam://run/739630" (Steam games)
- "hello" -> "hello back" (Responses)

# Current commands
- Url commands
- Steam games
- Text responses
- help (lists all available commands)
- cir (disable/enable random words)
- coi (disable/enable close on input)
- --setloadmsg (sets a custom loadmsg)
- --removeloadmsg (removes the custom loadmsg)
- --reset (resets json path)
- exit (closes the program)

# What this project uses
- C++, well duhh
- Nlohmann json https://github.com/nlohmann/json

# Json setup
- Edit the Json file to add/remove commands.
- First assign the shortcut (key) for example "yt".
- Then assign the type so it knows if it is a Url/Steam game/Response.
- And to finish up assign the value for Url. Just copypaste the Url in there, for steam games search the gameId on [SteamDB](https://steamdb.info/) and paste it in.
- You don't have to recompile.

# Project Setup
- Download this project (or just the cli.exe).
- Compile main.cpp and run it inside your terminal.
- Enter your commands.json path, and run a command.
- <b>Windows alternative setup:</b>
- Alternatively you could set a path in your system enviroments and just type the name in CMD.
- <b>Linux alternative setup:</b>
- Alternatively you could bind it to a key so you can press that key to run it in a new terminal.

# Debugging
There is now a debug function to check your prefs, jsonpath and loadmsgpath.

If you get a error trying to set your json path, first check if it is spelled correctly.
When that does not work try to insert it without quotes.

Example for a correct commands.json path:
C:\Users\yoshi\Documents\Dev\DataTerminalCMD\commands.json

Use --reset to reset the path, if that doesn't work delete the .cmdrc file located in C:\Users\yourname.

# Preview
<img width="1536" height="806" alt="image" src="https://github.com/user-attachments/assets/e848663b-b0b8-4666-a51c-d2efb31387ea" />

<img width="1516" height="808" alt="image" src="https://github.com/user-attachments/assets/8a1c9c85-5654-4297-845a-4d128f483130" />

