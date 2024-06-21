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


# Use Clang Compiler!!!!!!!!
# Warning: May need a long time to build, and may need to set build configuration to MinSizeRel (in vscode cmake extension)
# See README to see how to install mlpack
# Set to false if you can't build the project with mlpack
# If you see squiggles line for the first time, still build it, it is normal. After a successful build, the lines will disappear
set(USE_ML true)

# Declare the list that store the source files
set(SRC_FILES)

# Set to true only when you want to use the hand write scene, otherwise it will take a lot of time to build
# Estimated build time: 3 minutes
set(SERIALIZATION true)

# Decide which CNN model we want to use 
set(CNN_MODEL_VERSION 2)

# Set to true if you want to generate CNN training data 
# Won't support any game
set(GENERATE_TRAINING_DATA false)

# Should set to be true if you want to use AI enemy path finding 
# Modification in mlpack/methods/reinforcement_learning/q_learning_impl.hpp QLearning's constructor
set(FIX_QLEARNING_ERROR false)

# Set to true if you want to precompile mlpack headers
# Warning: It will take a very long time to build
# Estimated time: at least 3 hour
# Don't use
set(BUILD_PCH false)

# Set to true only if you want to train the RL agent
# Won't support any game and any Draw() functions
set(TRAIN_AGENT false)

# Set to true when you want to train an agent from the beginning, not based on previous model
set(RETRAIN_AGENT false)

if(USE_ML AND SERIALIZATION)
    add_definitions(-DMLPACK_ENABLE_ANN_SERIALIZATION)
endif()
    
    
if(FIX_QLEARNING_ERROR)
    add_definitions(-DFIX_MLPACK_ERROR)
endif()

if(USE_ML AND TRAIN_AGENT)
    add_definitions(-DTRAINING_AGENT)
    list(APPEND SRC_FILES 
        Scene/TrainAgentScene.cpp 
        Scene/TrainingEnv.cpp
    )
endif()

# Add your source file here
list(APPEND SRC_FILES
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
    UI/Component/DrawBoard.cpp
    
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
    Scene/StageSelectHandWriteScene.cpp
    Scene/DataGeneratingScene.cpp
)

if(BUILD_PCH)
    list(APPEND SRC_FILES ML_Resource/ml_pch.hpp)
endif()

add_executable(2024_I2P2_TowerDefense_with_answer ${SRC_FILES})

# make #include"ML_Macro.hpp" possible
configure_file(ML_Resource/ML_Macro.hpp.in ML_Macro.hpp)
target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC "${PROJECT_BINARY_DIR}")


if(USE_ML)
    # May need to comment out these two lines if cmake can't configure
    # There are these two lines in ML_Resource/armadillo/CMakeLists.txt also
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-mbig-obj")

    find_package(Threads)
    set(MLPACK_LIBRARIES ${MLPACK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

    file(COPY "ML_Resource/CNN_models" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    add_subdirectory("ML_Resource/armadillo")
    target_link_libraries(2024_I2P2_TowerDefense_with_answer PUBLIC armadillo)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC "${PROJECT_SOURCE_DIR}/ML_Resource/armadillo")
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC "${PROJECT_SOURCE_DIR}/ML_Resource/mlpack")
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC "${PROJECT_SOURCE_DIR}/ML_Resource/cereal_x64-windows/include")
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC "${PROJECT_SOURCE_DIR}/ML_Resource/ensmallen_x64-windows/include")

endif()

# Use precompile headers to reduce compile time in later compilation
if(USE_ML AND BUILD_PCH)
    list(APPEND SRC_FILES ML_Resource/ml_pch.hpp)
    target_precompile_headers(2024_I2P2_TowerDefense_with_answer PRIVATE ML_Resource/ml_pch.hpp)
endif()

IF (WIN32)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC C:/allegro/include)
    target_link_directories(2024_I2P2_TowerDefense_with_answer PUBLIC C:/allegro/lib)
    target_compile_options(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(2024_I2P2_TowerDefense_with_answer PUBLIC liballegro_monolith)
ELSE()
    find_package(PkgConfig)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC lib)
    target_include_directories(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(2024_I2P2_TowerDefense_with_answer ${ALLEGRO5_LIBRARIES})
    target_link_directories(2024_I2P2_TowerDefense_with_answer PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
ENDIF()