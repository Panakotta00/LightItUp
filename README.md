# LightItUp [![Build Status](https://jenkins.massivebytes.net/job/LightItUp/job/development/badge/icon)](https://jenkins.massivebytes.net/job/LightItUp/job/development)
"Light It Up" is a Satisfactory Mod which adds a couple of Lamps to the game.

## Contributors
- Panakotta00
- RosszEmber
- Deantendo
- Trxnce
- PortalGamesMais

## Lamptypes
- Lamp Pole: A High lamp to light up big areas, consumes power
- Street Lamp: A lamp to light up roads and paths, consumes power
- Powerpole Lamp: A powerpole with four lamps attached to it, consumes power
- Construction Light: A lamp to light up kind of big areas and surfaces sideways, consumes fuel
- Portable Lamp: A lamp which workes like a beacon but also sends beam into the sky as long it's powered, consumes fuel
- Spider Lamp: A smaller 360-Degree area light, consumes power

## Lampmodes
Some lamps have the lampmode functionallity which allowes to use following configs:
- On: Lamp should be always on
- Off: Lamp should be always off
- Auto: Lamp should be on or off depending on daytime
lamps which are fuel powered can be turned off via the "Active"-Switch int the bottom right corner of ther GUI

## Lampgroups
Some lamps support lampgroups. Lampgroups are intended for sharing Lampconfigurations between multiple Lamps. So you can group one street and turn it on, off by one click, without the needance to set each lamp individually. You can create and remove custom groups.

## Gallery
https://imgur.com/a/4eqNSkz

## Build your self

If you want to build the mod your self, you need to go through some setup processes.
1. download the Satisfactory Mod Loader source & install the satisfactor unreal engine
2. add in /Source directory symbolic links to the folders "FactoryGame" and "SML" in the Satisfactory Mod Loader Source. Also add a symbolic directory link to /Plugins/Alpakit from /Plugins/Alpakit in your Satisfactory Mod Loader Source
3. switch the unreal engine version from the .uproject to the satisfactory engine
4. open the generated .sln in Visual Studio and build the LightItUp project for "shipping" and "development editor"
5. double click the .uproject to open it with the unreal editor
6. open Alpakit and hit "Alpakit!"
7. You can find the .dll in /Binaries/Win64 and the .pak in /Mods
