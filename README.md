# 2024 I2P Mini Project 2 - Tower Defense

## Features
* Potion Class
	* Frost Spell: In reverse play mode, it will freeze the turret inside its radius, make them unable to shoot, in other play modes, it will freeze the enemy, force them to stop
	* Shield Spell: Only works in reverse play mode, it will create shields for the enemies that are insiede its radius, provides them additional protection
	* Berserk Spell: In reverse play mode, it will speed up the enemy, in other play modes, it will increase the rate of fire of the turrets
* Landmine Class
	* Explode Landmine: Only works in normal, survival and black mode, after deployment, it need some time to activate, and after activation, if enemy step on the landmine, it will kill nearby enemies directly
* Hover Image Button
	* A UI button that will show detail message when the mouse is hovering.
* Survival Play Mode
	* An Endles play scene, the game will only end when the player die from 10 enemies reaching the end point, and the difficulty will increase with time
* Reverse Play Mode
	* A play scene where user control enemy spawning and their path, and the program spawn turret automatically based on some strategy
* Dark Mode
    * The field will usually be completely dark, and only when it flashes every few seconds can the players see the situation on the field.
* Better Turret Aiming
	* Use some easy math to approximate where the enemies will be when the bullet travel to the enemy to increase the accuracy in long range (not 100% accurate)
* Difficulty Selection Scene
	* Allow user to choose difficulty, in normal, black and survival play mode, it will determine how many enemy will spawn, while in reverse play mode, it will decide how much time the player have.
* Turret Upgrade
	* For Every turret on the map (reverse play mode excluded), user can either upgrade its range or rate of fire, both of the operation will cost some money	
* Map Create Scene
	* A scene where user can create their own map, support invalid map checking.
* Revive Scene
	* After the first time when the player die, it will enter revive scene to answer some math question, if correct, the game will go back to the previous play scene and kill enemies that are too closed to the end point
* Data Generating Scene
	* A scene where user can create their own handwritten digit images, used to fine-tune the CNN model that is trained for seleting stage by classifying user's handwritten digit.
* Stage Select Hand Write Scene
	* A scene where user can write two digit images, and a CNN model will classify the images then decide which stage to go to, support ruling invalid stages.

## Contribution

| **Features**                              | **張立璿**  | **郭明懿** | **陳廷森** |
| :---------------------------------------- | :-------:  | :-------: | :-------: |
| Potion Class: `Frost Spell`               |     -      |     -     |    100%   |
| Potion Class: `Shield Spell`              |     -      |     -     |    100%   |
| Potion Class: `Berserk Spell`             |     -      |     -     |    100%   |
| Landmine Class: `Explode Landmine`        |     -      |     -     |    100%   |
| New UI: `Hover Image Button`              |    70%     |     -     |    30%    |
| New Game Mode: `Survival Play Mode`       |    80%     |     -     |    20%    |
| New Game Mode: `Reverse Play Mode`        |    70%     |     -     |    30%    |
| New Game Mode: `Dark Mode`                |      -     |  100%     |      -    |
| New Game Feature: `Better Turret Aiming`  |   100%     |     -     |      -    |
| New Game Feature: `Difficulty Selection`  |    70%     |   30%     |      -    |
| New Game Feature: `Turret Upgrade`        |     -      |     -     |    100%   |
| New Scene: `Map Create Scene`             |   100%     |     -     |      -    |
| New Scene: `Revive Scene`                 |    60%     |   15%     |    25%    |
| New Scene: `Data Generating Scene`        |   100%     |     -     |      -    |
| Stage Select: `AI Digit Classification`   |    70%     |   15%     |    15%    |

## Appendix: How to install mlpack
* tested under the following environment
    * windows
    * compiler: llvm-mingw-20231128-urct-x86_64
    * vscode
* 安裝 [vcpkg](https://learn.microsoft.com/zh-tw/vcpkg/get_started/get-started?pivots=shell-cmd)
* 將 `vcpkg` 放到環境變數 (整個資料夾)
* command line 輸入： `vcpkg install openblas`
* `vcpkg/packages` 下會有 `openblas_x64-windows`，將其中的 `bin` 資料夾放到環境變數
![alt text](image.png)
