cmake_minimum_required(VERSION 3.27)
project(2024_I2P2_TowerDefense_with_answer)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(.)
include_directories(Resource)

file(COPY Resource/audios DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(2024_I2P2_TowerDefense_with_answer
        main.cpp
        
        Engine/Allegro5Exception.cpp
        Engine/AudioHelper.cpp
        Engine/Collider.cpp
        Engine/GameEngine.cpp
        Engine/Group.cpp
        Engine/IControl.cpp
        Engine/IObject.cpp
        Engine/IScene.cpp
        Engine/LOG.cpp
        Engine/Point.cpp
        Engine/Resources.cpp
        Engine/Sprite.cpp
        
        Bullet/Bullet.cpp
        Bullet/FireBullet.cpp
        Bullet/MissileBullet.cpp
        Bullet/AdvancedMissileBullet.cpp
        Bullet/LaserBullet.cpp

        UI/Animation/ExplosionEffect.cpp
        UI/Animation/DirtyEffect.cpp
        UI/Component/Image.cpp
        UI/Component/TextInput.cpp
        UI/Component/ImageButton.cpp
        UI/Component/HoverImageButton.cpp
        UI/Component/EnemyButton.cpp
        UI/Component/Label.cpp
        UI/Animation/Plane.cpp
        UI/Component/Slider.cpp
        
        Enemy/Enemy.cpp
        Enemy/PlaneEnemy.cpp
        Enemy/SoldierEnemy.cpp
        Enemy/AdvancedTankEnemy.cpp
        Enemy/TankEnemy.cpp
        
        Turret/Turret.cpp
        Turret/LaserTurret.cpp
        Turret/MachineGunTurret.cpp
        Turret/MissileTurret.cpp
        Turret/AdvancedMissileTurret.cpp
        Turret/TurretButton.cpp
        Turret/HoverTurretButton.cpp
        Turret/Shovel.cpp
        Turret/Potion.cpp
        Turret/FrostPotion.cpp
        Turret/BerserkPotion.cpp
        Turret/ShieldPotion.cpp
        
        Scene/WinScene.cpp
        Scene/MapCreateScene.cpp
        Scene/ModeSelectScene.cpp
        Scene/ReversePlayScene.cpp
        Scene/StartScene.cpp
        Scene/LoseScene.cpp
        Scene/PlayScene.cpp
        Scene/StageSelectScene.cpp
        Scene/DifficultySelectScene.cpp
        Scene/SettingsScene.cpp
        Scene/ScoreBoardScene.cpp
        Scene/NormalPlayScene.cpp
        Scene/ReviveScene.cpp
        Scene/SurvivalPlayScene.cpp
    )


# target_include_directories(2024_I2P2_TowerDefense_with_answer PRIVATE C:/vcpkg/packages/mlpack_x64-windows/include)
# target_include_directories(2024_I2P2_TowerDefense_with_answer PRIVATE C:/vcpkg/packages/armadillo_x64-windows/include)
# target_include_directories(2024_I2P2_TowerDefense_with_answer PRIVATE C:/vcpkg/packages/cereal_x64-windows/include)
# target_include_directories(2024_I2P2_TowerDefense_with_answer PRIVATE C:/vcpkg/packages/ensmallen_x64-windows/include)

IF (WIN32)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC C:/allegro/include)
    target_link_directories(2024_I2P2_TowerDefense_with_answer PUBLIC C:/allegro/lib)
    target_compile_options(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(2024_I2P2_TowerDefense_with_answer liballegro_monolith)
ELSE()
    find_package(PkgConfig)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC lib)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(2024_I2P2_TowerDefense_with_answer ${ALLEGRO5_LIBRARIES})
    target_link_directories(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
ENDIF()