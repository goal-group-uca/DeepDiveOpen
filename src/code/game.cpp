#include "game.h"
#include <barrier>
#include <deque>
#include <functional>
#include <fstream>
#include <string>
#include <shared_mutex>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#if GRAPHICS_API == OPENGL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#endif

#include "AIAPI.h"
#include "batch.h"
#include "camera.h"
#include "entity.h"
#include "vertex.h"
#include "definitions.h"
#include "shader.h"
#include "texture.h"
#include "graphics.h"
#include "gui.h"
#include "GUIfunctions.h"
#include "input.h"
#include "inputFunctions.h"
#include "utilities.h"
#include "tickFunctions.h"
#include "worldGen.h"


namespace VoxelEng {

    ////////////
    //Classes.//
    ////////////


    // 'game' class.

    bool game::initialised_ = false,
         game::graphicalModeInitialised_ = false,
         game::AImodeON_ = false,
         game::useComplexLighting_ = false;

    window* game::mainWindow_ = nullptr;

    std::thread* game::chunkManagementThread_ = nullptr,
               * game::playerInputThread_ = nullptr,
               * game::tickManagementThread_ = nullptr;

    std::atomic<bool> game::threadsExecute[3] = {false, false, false};
    std::atomic<engineMode> game::loopSelection_ = engineMode::AIMENULOOP;
    std::atomic<double> game::timeStep_ = 0.0f;

    skybox game::defaultSkybox_(140, 170, 255, 1.0f);

    unsigned int game::saveSlot_ = 0,
                 game::blockReachRange_ = 5,
                 game::nMeshingThreads_ = 1;
    float game::FOV_ = 110.0f,
          game::zNear_ = 0.1f,
          game::zFar_ = 500.0f;

    camera* game::playerCamera_ = nullptr;

    

    texture* game::blockTextureAtlas_ = nullptr;

    std::unordered_map<vec3, std::vector<vertex>> const* game::chunksToDraw_ = nullptr;
    const std::vector<model>* game::batchesToDraw_ = nullptr;

    shader* game::defaultShader_ = nullptr;
    vertexBuffer* game::vbo_ = nullptr;
    vertexArray* game::va_ = nullptr;
    vertexBufferLayout* game::layout_ = nullptr;
    renderer* game::renderer_ = nullptr;

    #if GRAPHICS_API == OPENGL

        glm::mat4 game::MVPmatrix_;

    #else



    #endif

    slotAccessType game::slotAccessType_ = slotAccessType::load;


    void game::init() {

        if (initialised_)
            logger::errorLog("Game is already initialised");
        else {

            // Put here any game initialisation that does not involve
            // the engine's graphical mode.

            loopSelection_ = engineMode::AIMENULOOP;
            timeStep_ = 0.0f;
            AImodeON_ = false;

            worldGen::init();

            initialised_ = true;

        }

    }
    
    void game::initGraphicalMode() {
    
        if (graphicalModeInitialised_)
            logger::errorLog("Engine's graphical mode already initialised");
        else if (AImodeON_)
            logger::errorLog("Graphical mode is not allowed with AI mode turned ON.");
        else {

            mainWindow_ = new window(800, 800, "VoxelEng");

            saveSlot_ = 0,
            blockReachRange_ = 5,
            nMeshingThreads_ = 1;
            
            FOV_ = 110.0f,
            zNear_ = 0.1f,
            zFar_ = 500.0f;

            useComplexLighting_ = false;

            #if GRAPHICS_API == OPENGL

                MVPmatrix_ = glm::mat4();

            #endif

            slotAccessType_ = slotAccessType::load;

            // Initialise the graphics API/libraries if not done yet.
            if (!graphics::initialised())
                graphics::init(*mainWindow_);

            // Load player system.
            player::init(FOV_, zNear_, zFar_, *mainWindow_, blockReachRange_);
            playerCamera_ = &player::getCamera();

            // Init user control input system.
            input::init();
            inputFunctions::init();

            // Input functions registration.
            input::setControlAction(controlCode::space, inputFunctions::moveUp);
            input::setControlAction(controlCode::leftShift, inputFunctions::moveDown);
            input::setControlAction(controlCode::w, inputFunctions::moveNorth);
            input::setControlAction(controlCode::s, inputFunctions::moveSouth);
            input::setControlAction(controlCode::a, inputFunctions::moveEast);
            input::setControlAction(controlCode::d, inputFunctions::moveWest);
            input::setControlAction(controlCode::r, inputFunctions::switchComplexLighting, false);


            // Load model system.
            models::init();

            // Custom model loading.
            models::loadCustomModel("Resources/Models/Warden.obj", 2);

            // Load chunk system and chunk management system if not loaded.
            if (!chunk::initialised())
                chunk::init();

            if (!chunkManager::initialised())
                chunkManager::init();

            // Load entity manager system.
            if (!entityManager::initialised()) 
                entityManager::init();


            // World settings.
            world::init();
            world::setSkybox(defaultSkybox_);


            // Load texture atlas and configure it.
            blockTextureAtlas_ = new texture("Resources/Textures/atlas.png");
            texture::setBlockAtlas(*blockTextureAtlas_);
            texture::setBlockAtlasResolution(16);


            // Load shaders.
            defaultShader_ = new shader("Resources/Shaders/vertexShader.shader", "Resources/Shaders/fragmentShader.shader");


            // Load graphics API data structures.
            vbo_ = new vertexBuffer();
            va_ = new vertexArray();
            layout_ = new vertexBufferLayout();
            renderer_ = new renderer();


            /*
            GUI initialization and GUI elements registration.
            WARNING. The engine currently only supports initialization of GUIElements
            before the main menu loop starts for the first time in the game's execution.
            */
            GUImanager::init(*mainWindow_, *defaultShader_, *renderer_);

            // Main menu.
            GUImanager::addGUIBox("mainMenu", 0.5, 0.5, 0.3, 0.35, 995, true, GUIcontainer::both);
            GUImanager::addGUIButton("mainMenu.loadButton", 0.5, 0.65, 0.10, 0.05, 961, true, GUIcontainer::both, "mainMenu", 1);
            GUImanager::addGUIButton("mainMenu.saveButton", 0.5, 0.45, 0.10, 0.05, 993, false, GUIcontainer::both, "mainMenu", 1);
            GUImanager::addGUIButton("mainMenu.exitButton", 0.5, 0.25, 0.10, 0.05, 929, true, GUIcontainer::both, "mainMenu", 1);
            GUImanager::addGUIButton("mainMenu.newButton", 0.5, 0.45, 0.10, 0.05, 1019, true, GUIcontainer::both, "mainMenu", 1);

            // Load menu.
            GUImanager::addGUIBox("loadMenu", 0.5, 0.5, 0.3, 0.35, 1009, false, GUIcontainer::both);
            GUImanager::addGUIButton("loadMenu.exitButton", 0.5, 0.1, 0.10, 0.05, 1017, false, GUIcontainer::both, "loadMenu", 1);

            // Save menu.
            GUImanager::addGUIBox("saveMenu", 0.5, 0.5, 0.3, 0.35, 1013, false, GUIcontainer::both);
            GUImanager::addGUIButton("saveMenu.exitButton", 0.5, 0.1, 0.10, 0.05, 1017, false, GUIcontainer::both, "saveMenu", 1);

            // Save slot buttons.
            GUImanager::addGUIButton("saveSlot1", 0.3, 0.65, 0.10, 0.05, 999, false, GUIcontainer::both, "loadMenu", 1);
            GUImanager::addGUIButton("saveSlot2", 0.7, 0.65, 0.10, 0.05, 1001, false, GUIcontainer::both, "loadMenu", 1);
            GUImanager::addGUIButton("saveSlot3", 0.3, 0.45, 0.10, 0.05, 1003, false, GUIcontainer::both, "loadMenu", 1);
            GUImanager::addGUIButton("saveSlot4", 0.7, 0.45, 0.10, 0.05, 1005, false, GUIcontainer::both, "loadMenu", 1);
            GUImanager::addGUIButton("saveSlot5", 0.5, 0.275, 0.10, 0.05, 1007, false, GUIcontainer::both, "loadMenu", 1);


            /*
            Set up GUIElements' keys and key functions.
            */

            // Main Menu/Level.
            GUImanager::bindActKeyFunction("mainMenu", GUIfunctions::changeStateLevelMenu, controlCode::e);
            GUImanager::bindActMouseButtonFunction("mainMenu.loadButton", GUIfunctions::showLoadMenu, controlCode::leftButton);
            GUImanager::bindActMouseButtonFunction("mainMenu.saveButton", GUIfunctions::showSaveMenu, controlCode::leftButton);
            GUImanager::bindActMouseButtonFunction("mainMenu.exitButton", GUIfunctions::exit, controlCode::leftButton);
            GUImanager::bindActMouseButtonFunction("mainMenu.newButton", GUIfunctions::enterNewLevel, controlCode::leftButton);

            // Load menu.
            GUImanager::bindActMouseButtonFunction("loadMenu.exitButton", GUIfunctions::hideLoadMenu, controlCode::leftButton);

            // Save menu.
            GUImanager::bindActMouseButtonFunction("saveMenu.exitButton", GUIfunctions::hideSaveMenu, controlCode::leftButton);

            // Save slot buttons.
            GUImanager::bindActMouseButtonFunction("saveSlot1", GUIfunctions::accessSaveSlot, controlCode::leftButton);
            GUImanager::bindActMouseButtonFunction("saveSlot2", GUIfunctions::accessSaveSlot, controlCode::leftButton);
            GUImanager::bindActMouseButtonFunction("saveSlot3", GUIfunctions::accessSaveSlot, controlCode::leftButton);
            GUImanager::bindActMouseButtonFunction("saveSlot4", GUIfunctions::accessSaveSlot, controlCode::leftButton);
            GUImanager::bindActMouseButtonFunction("saveSlot5", GUIfunctions::accessSaveSlot, controlCode::leftButton);


            // Finish connecting some objects.
            mainWindow_->playerCamera() = playerCamera_;

            glfwSetMouseButtonCallback(mainWindow_->windowAPIpointer(), player::mouseButtonCallback);
            glfwSetWindowSizeCallback(mainWindow_->windowAPIpointer(), window::windowSizeCallback);


            // Configure the vertex layout for 3D rendering.
            layout_->push<GLfloat>(3);
            layout_->push<GLfloat>(2);
            layout_->push<normalVec>(1);


            // Bind the currently used VAO, shaders and atlases for 3D rendering.
            va_->bind();
            vbo_->bind();
            va_->addLayout(*layout_);
            defaultShader_->bind();
            blockTextureAtlas_->bind();


            graphicalModeInitialised_ = true;
        
        }
    
    }

    void game::mainLoop() {
    
        do {

            switch (loopSelection_) {

            case engineMode::AIMENULOOP:  // AI game menu.

                aiMenuLoop();

                break;

            case engineMode::GRAPHICALMENU: // Main menu/game loop.

                mainMenuLoop();

                break;

            case engineMode::INITLEVEL: // Level game loop.

                gameLoop();

                break;

            default:
                break;

            }

        } while (loopSelection_ != engineMode::EXIT);
    
    }

    void game::aiMenuLoop() {
    
        if (loopSelection_ == engineMode::AIMENULOOP) {
        
            unsigned int nGames = 0,
                chosenOption = 0;

            logger::say("AI menu. Please select one of the following options.");
            nGames = AIAPI::aiGame::listAIGames();
            logger::say(std::to_string(nGames + 1) + "). Enter level editor mode");
            logger::say(std::to_string(nGames + 2) + "). Exit");

            do {

                while (!validatedCinInput<unsigned int>(chosenOption) || chosenOption == 0 || chosenOption > nGames + 2)
                    logger::say("Invalid option. Please try again");

                if (chosenOption <= nGames) {

                    AIAPI::aiGame::selectGame(chosenOption - 1);
                    AIAPI::aiGame::startGame();

                    // Once back from the selected game, display options again.
                    logger::say("AI menu. Please select one of the following options.");
                    nGames = AIAPI::aiGame::listAIGames();
                    logger::say(std::to_string(nGames + 1) + "). Enter level editor mode");
                    logger::say(std::to_string(nGames + 2) + "). Exit");

                }
                else {

                    if (chosenOption == nGames + 1)
                        setLoopSelection(engineMode::GRAPHICALMENU);
                    else
                        setLoopSelection(engineMode::EXIT);

                }

            } while (loopSelection_ == engineMode::AIMENULOOP);

        }

    }

    void game::mainMenuLoop() {

        if (game::loopSelection_ == engineMode::GRAPHICALMENU) {

            // Set 3D rendering mode uniforms.
            defaultShader_->setUniform1i("u_renderMode", 1);
            graphics::setDepthTest(false);

            /*
            Rendering loop.
            */
            unsigned int nVertices = 0;
            while (game::loopSelection_ == engineMode::GRAPHICALMENU) {

                // The window size callback by GLFW gets called every time the user is resizing the window so the heavy resize processing is done here
                // after the player has stopped resizing the window.
                if (mainWindow_->wasResized())
                    mainWindow_->resizeHeavyProcessing();

                // Clear the screen to draw the next frame.
                renderer_->clear();


                /*
                2D rendering.
                */
                GUImanager::drawGUI(true);

                // Swap front and back buffers.
                glfwSwapBuffers(mainWindow_->windowAPIpointer());


                /*
                Event handling
                */

                // Poll for and process Graphic API events.
                glfwPollEvents();

                // Handle user input.
                input::handleInputs();

            }
        
        }

    }

    void game::gameLoop(const std::string& terrainFile) {

        if (loopSelection_ == engineMode::INITLEVEL || loopSelection_ == engineMode::INITRECORD) {

            if (!graphicalModeInitialised_)
                initGraphicalMode();

            /*
            Level loading.
            */

            // Start the terrain management and loading of the world.
            if (chunkManager::infiniteWorld())
                chunkManagementThread_ = new std::thread(&chunkManager::manageChunks, nMeshingThreads_);
            else
                chunkManagementThread_ = new std::thread(&chunkManager::finiteWorldLoading, terrainFile);

            if (loopSelection_ == engineMode::INITRECORD) {

                if (!GUImanager::isLevelGUIElementRegistered("pauseIcon")) {

                    GUImanager::addGUIBox("pauseIcon", 0.15, 0.85, 0.1, 0.1, 1021);
                    input::setControlAction(controlCode::rightArrow, inputFunctions::recordForward, false);
                    input::setControlAction(controlCode::downArrow, inputFunctions::recordPause, false);
                    input::setControlAction(controlCode::leftArrow, inputFunctions::recordBackwards, false);
                    input::setControlAction(controlCode::x, inputFunctions::exitRecord, false);

                    world::addGlobalTickFunction("playRecordTick", TickFunctions::playRecordTick);

                }

                // Things to apply when the terrain is loaded.
                chunkManager::waitTerrainLoaded();

                setLoopSelection(engineMode::PLAYINGRECORD);

            }
            else {

                if (!GUImanager::isLevelGUIElementRegistered("blockPreview")) {
                
                    GUImanager::addGUIBox("blockPreview", 0.15, 0.85, 0.1, 0.1, 1);
                    input::setControlAction(controlCode::alpha1, inputFunctions::selectBlockSlot1, false);
                    input::setControlAction(controlCode::alpha2, inputFunctions::selectBlockSlot2, false);
                    input::setControlAction(controlCode::alpha3, inputFunctions::selectBlockSlot3, false);
                    input::setControlAction(controlCode::alpha4, inputFunctions::selectBlockSlot4, false);
                    input::setControlAction(controlCode::alpha5, inputFunctions::selectBlockSlot5, false);
                    input::setControlAction(controlCode::alpha6, inputFunctions::selectBlockSlot6, false);
                    input::setControlAction(controlCode::alpha7, inputFunctions::selectBlockSlot7, false);
                    input::setControlAction(controlCode::alpha8, inputFunctions::selectBlockSlot8, false);
                    input::setControlAction(controlCode::alpha9, inputFunctions::selectBlockSlot9, false);
                    input::setControlAction(controlCode::p, inputFunctions::intentionalCrash, false);

                }

                // Things to apply when the terrain is loaded.
                chunkManager::waitTerrainLoaded();

                setLoopSelection(engineMode::EDITLEVEL);

            }

            // Start threads that require the world to be loaded first.
            if (!AIAPI::aiGame::playingRecord())
                playerInputThread_ = new std::thread(&player::processSelectionRaycast);
            tickManagementThread_ = new std::thread(&world::processWorldTicks);


            /*
            Rendering loop.
            */

            // Configure game window's settings.
            mainWindow_->changeStateMouseLock(false);

            // Set shader options.
            vec3 lightpos(10.0f, 150.0f, -10.0f);
            defaultShader_->setUniformVec3f("u_sunLightPos", lightpos);
            defaultShader_->setUniform1i("u_useComplexLighting", 0);

            // Time/FPS related stuff.
            double lastSecondTime = glfwGetTime(), // How much time has passed since the last second passed.
                   lastFrameTime = lastSecondTime,
                   actualTime;
            int nFramesDrawn = 0; 
            unsigned int nVertices = 0;
            while (loopSelection_ == engineMode::EDITLEVEL || loopSelection_ == engineMode::PLAYINGRECORD) {

                // The window size callback by GLFW gets called every time the user is resizing the window so the heavy resize processing is done here
                // after the player has stopped resizing the window.
                if (mainWindow_->wasResized())
                    mainWindow_->resizeHeavyProcessing();

                if (!mainWindow_->isMouseFree())
                    playerCamera_->updatePos(game::timeStep());
                playerCamera_->updateView();

                MVPmatrix_ = playerCamera_->projectionMatrix() * playerCamera_->viewMatrix();
                defaultShader_->setUniformMatrix4f("u_MVP", MVPmatrix_);
                defaultShader_->setUniformVec3f("u_viewPos", playerCamera_->pos());

                // Times calculation.
                actualTime = glfwGetTime();
                timeStep_ = actualTime - lastFrameTime;
                lastFrameTime = actualTime;

                // Clear the screen to draw the next frame.
                renderer_->clear();

                // ms/frame calculation and display.
                nFramesDrawn++;
                if (actualTime - lastSecondTime >= 1.0) {

                    //std::cout << "\r" << 1000.0 / nFramesDrawn << "ms/frame" << " and resolution is " << mainWindow_.width() << " x " << mainWindow_.height();
                    nFramesDrawn = 0;
                    lastSecondTime = glfwGetTime();

                }

                /*
                3D rendering.
                */
                defaultShader_->setUniform1i("u_renderMode", 0); // renderMode = 0 stands for 3D rendering mode.

                // Coordinate rendering thread and chunk management thread if necessary.
                if (chunkManager::managerThreadMutex().try_lock()) {

                    if (chunkManager::forceSyncFlag())
                        chunkManager::updatePriorityChunks();
                    else {

                        chunkManager::swapDrawableChunksLists();
                        chunksToDraw_ = chunkManager::drawableChunksRead();

                    }

                    chunkManager::managerThreadMutex().unlock();
                    chunkManager::managerThreadCV().notify_one();

                }

                // Coordinate rendering thread and the thread in charge of generating entity render data if necessary.
                if (entityManager::syncMutex().try_lock()) {

                    entityManager::swapReadWrite();
                    batchesToDraw_ = entityManager::renderingData();

                    entityManager::syncMutex().unlock();
                    entityManager::entityManagerCV().notify_one();

                }

                // Binding section.
                va_->bind();
                vbo_->bind();
                
                // Render chunks.
                if (chunksToDraw_) {

                    // chunk.first refers to the chunk's postion.
                    // chunk.second refers to the chunk's vertex data.
                    for (auto const& chunk : *chunksToDraw_) {

                        if (nVertices = chunk.second.size()) {

                            vbo_->prepareStatic(chunk.second.data(), sizeof(vertex) * nVertices);

                            renderer_->draw3D(nVertices);

                        }

                    }

                }

                // Render batches.
                if (batchesToDraw_) {

                    for (auto const& batch : *batchesToDraw_) {

                        if (nVertices = batch.size()) {

                            vbo_->prepareStatic(batch.data(), sizeof(vertex) * nVertices);

                            renderer_->draw3D(nVertices);

                        }

                    }

                }


                /*
                2D rendering.
                */
                graphics::setDepthTest(false);
                defaultShader_->setUniform1i("u_renderMode", 1);

                GUImanager::drawGUI();

                graphics::setDepthTest(true);

                // Unbinding section.
                va_->unbind();
                vbo_->unbind();

                // Swap front and back buffers.
                glfwSwapBuffers(mainWindow_->windowAPIpointer());

                // Poll for and process events.
                glfwPollEvents();

                // Handle user inputs.
                input::handleInputs();

            }

        }
    
    }

    void game::setLoopSelection(engineMode mode) {
    
        switch (loopSelection_) {

            case engineMode::AIMENULOOP:

                switch (mode) {
            
                    case engineMode::EXIT:

                        input::inputMutex().lock(); // Input management needs this.
                        cleanUp();

                        loopSelection_ = engineMode::EXIT;
                        input::inputMutex().unlock();

                        break;

                    case engineMode::AIMENULOOP:
                        break;

                    case engineMode::GRAPHICALMENU:

                        setAImode(false);

                        initGraphicalMode();

                        loopSelection_ = engineMode::GRAPHICALMENU;

                        break;

                    case engineMode::INITRECORD:

                        setAImode(false);

                        initGraphicalMode();

                        loopSelection_ = engineMode::INITRECORD;
                        threadsExecute[0] = true;
                        threadsExecute[1] = true;
                        threadsExecute[2] = true;

                        break;

                    default:
                        logger::errorLog("Unsupported engine mode transition");
            
                }

                break;

            case engineMode::GRAPHICALMENU:

                switch (mode) {

                    case engineMode::EXIT:

                        setLoopSelection(engineMode::AIMENULOOP);
                        setLoopSelection(engineMode::EXIT);

                        break;

                    case engineMode::AIMENULOOP:

                        cleanUpGraphicalMode();
                        setAImode(true);

                        if (!chunkManager::openedTerrainFileName().empty())
                            chunkManager::openedTerrainFileName("");

                        loopSelection_ = VoxelEng::engineMode::AIMENULOOP;

                        break;

                    case engineMode::GRAPHICALMENU:
                        break;

                    case engineMode::INITLEVEL:

                        threadsExecute[0] = true;
                        threadsExecute[1] = true;
                        threadsExecute[2] = true;
                        loopSelection_ = engineMode::INITLEVEL;
                        
                        break;

                    default:
                        logger::errorLog("Unsupported engine mode transition");

                }

                break;

            case engineMode::INITLEVEL:

                switch (mode) {

                    case engineMode::EXIT:

                        setLoopSelection(engineMode::GRAPHICALMENU);
                        setLoopSelection(engineMode::EXITLEVEL);
                        setLoopSelection(engineMode::AIMENULOOP);
                        setLoopSelection(engineMode::EXIT);

                        break;

                    case engineMode::INITLEVEL:
                        break;

                    case engineMode::EDITLEVEL:

                        loopSelection_ = engineMode::EDITLEVEL;

                        break;

                    case engineMode::EXITLEVEL:

                        game::stopAuxiliaryThreads();
                        loopSelection_ = engineMode::EXITLEVEL;

                        break;

                    default:
                        logger::errorLog("Unsupported engine mode transition");

                }

                break;

            case engineMode::EDITLEVEL:

                switch (mode) {

                    case engineMode::EXIT:

                        setLoopSelection(engineMode::EXITLEVEL);
                        setLoopSelection(engineMode::GRAPHICALMENU);
                        setLoopSelection(engineMode::AIMENULOOP);
                        setLoopSelection(engineMode::EXIT);

                        break;

                    case engineMode::INITLEVEL:

                        // Reload anything necessary between changing worlds.
                        setLoopSelection(engineMode::EXITLEVEL);
                        cleanUpLevel();
                        threadsExecute[0] = true;
                        threadsExecute[1] = true;
                        threadsExecute[2] = true;
                        loopSelection_ = engineMode::INITLEVEL;

                        break;

                    case engineMode::EDITLEVEL:
                        break;

                    case engineMode::EXITLEVEL:

                        game::stopAuxiliaryThreads();
                        loopSelection_ = engineMode::EXITLEVEL;

                        break;

                    default:
                        logger::errorLog("Unsupported engine mode transition");

                }

                break;

            case engineMode::EXITLEVEL:

                switch (mode) {
                
                    case engineMode::GRAPHICALMENU:

                        cleanUpLevel();
                        loopSelection_ = engineMode::GRAPHICALMENU;

                        break;

                    default:
                        logger::errorLog("Unsupported engine mode transition");
                
                }

                break;

            case engineMode::INITRECORD:

                switch (mode) {

                    case engineMode::EXIT:

                        setLoopSelection(engineMode::EXITRECORD);
                        setLoopSelection(engineMode::AIMENULOOP);
                        setLoopSelection(engineMode::EXIT);

                        break;

                    case engineMode::AIMENULOOP:

                        cleanUpLevel();
                        cleanUpGraphicalMode();
                        setAImode(true);

                        if (!chunkManager::openedTerrainFileName().empty())
                            chunkManager::openedTerrainFileName("");

                        loopSelection_ = engineMode::AIMENULOOP;

                        break;

                    case engineMode::INITRECORD:
                        break;

                    case engineMode::PLAYINGRECORD:

                        loopSelection_ = engineMode::PLAYINGRECORD;

                        break;

                    case engineMode::EXITRECORD:

                        game::stopAuxiliaryThreads();
                        loopSelection_ = engineMode::EXITRECORD;

                        break;

                    default:
                        logger::errorLog("Unsupported engine mode transition");

                }

                break;

            case engineMode::PLAYINGRECORD:

                switch (mode) {

                    case engineMode::EXIT:

                        setLoopSelection(engineMode::EXITRECORD);
                        setLoopSelection(engineMode::AIMENULOOP);
                        setLoopSelection(engineMode::EXIT);

                        break;

                    case engineMode::PLAYINGRECORD:
                        break;

                    case engineMode::EXITRECORD:

                        game::stopAuxiliaryThreads();
                        loopSelection_ = engineMode::EXITRECORD;

                        break;

                    default:
                        logger::errorLog("Unsupported engine mode transition");

                }

                break;

            case engineMode::EXITRECORD:

                switch (mode) {

                    case engineMode::AIMENULOOP:

                        cleanUpLevel();
                        cleanUpGraphicalMode();
                        setAImode(true);

                        if (!chunkManager::openedTerrainFileName().empty())
                            chunkManager::openedTerrainFileName("");

                        loopSelection_ = engineMode::AIMENULOOP;

                        break;
                
                    default:
                        logger::errorLog("Unsupported engine mode transition");
                
                }

                break;

            default:
                logger::errorLog("Unspecified current engine mode selected");
        
        }
    
    }

    void game::switchComplexLighting() {

        defaultShader_->setUniform1i("u_useComplexLighting", useComplexLighting_ ? 1 : 0);
        useComplexLighting_ = !useComplexLighting_;

    }

    void game::setAImode(bool ON) {

        engineMode mode = game::selectedEngineMode();
        if (mode == engineMode::EDITLEVEL)
            logger::errorLog("Cannot change chunkManager's AI mode while in a level");
        else if (mode == engineMode::PLAYINGRECORD)
            logger::errorLog("Cannot change chunkManager's AI mode while playing a record");
        else
            AImodeON_ = ON;

        chunkManager::setAImode(AImodeON_);
        entityManager::setAImode(AImodeON_);

    }

    void game::stopAuxiliaryThreads() {

        if (chunkManagementThread_ && threadsExecute[2]) {

            // Notify the chunk management and the high priority update threads to stop.
            // In case a thread is waiting on its corresponding condition variable, send a notification to unblock it.
            {

                threadsExecute[2] = false;
                std::unique_lock<std::mutex> lock(chunkManager::managerThreadMutex());

            }
            chunkManager::managerThreadCV().notify_one();
            
            chunkManagementThread_->join();
            delete chunkManagementThread_;
            chunkManagementThread_ = nullptr;

        }

        if (playerInputThread_ && threadsExecute[0]) {

            {

                threadsExecute[0] = false;
                std::unique_lock<std::recursive_mutex> lock(chunkManager::chunksMutex());

            }

            playerInputThread_->join();
            delete playerInputThread_;
            playerInputThread_ = nullptr;

        }

        if (tickManagementThread_ && threadsExecute[1]) {

            // Notify the tick management thread to stop.
            // In case it is waiting on its corresponding condition variable, send a notification to unblock it.
            {
            
                threadsExecute[1] = false;
                std::unique_lock<std::mutex> syncLock(entityManager::syncMutex());
            
            }
            entityManager::entityManagerCV().notify_one();

            tickManagementThread_->join();
            delete tickManagementThread_;
            tickManagementThread_ = nullptr;

        }
    
    }

    void game::cleanUpLevel() {
    
        // Clear everything related to the engine that is exclusively generated when inside a level that is not
        // already handled in game::cleanUp or game::cleanUpGraphicalMode.
        
    }

    void game::cleanUp() {

        std::unique_lock<std::recursive_mutex> lock(input::inputMutex());
        input::shouldProcessInputs(false);

        stopAuxiliaryThreads();

        // Clear everything related to the engine that is not related to the engine's graphical mode.

        worldGen::cleanUp();

        if (graphicalModeInitialised_)
            cleanUpGraphicalMode();

        initialised_ = false;

        input::shouldProcessInputs(true);

    }

    void game::cleanUpGraphicalMode() {

        std::unique_lock<std::recursive_mutex> lock(input::inputMutex());

        // Clear everything that is related to the engine's graphical mode.

        GUImanager::setMMGUIChanged(true);

        GUImanager::cleanUp();

        player::cleanUp();

        input::cleanUp();

        inputFunctions::cleanUp();

        chunk::cleanUp();

        chunkManager::cleanUp();

        models::cleanUp();

        entityManager::cleanUp();

        world::cleanUp();

        graphics::cleanUp();

        if (blockTextureAtlas_)
            delete blockTextureAtlas_;

        if (defaultShader_)
            delete defaultShader_;

        if (vbo_)
            delete vbo_;

        if (va_)
            delete va_;

        if (layout_)
            delete layout_;

        if (renderer_)
            delete renderer_;

        mainWindow_ = nullptr;

        playerCamera_ = nullptr;

        chunksToDraw_ = nullptr;
        batchesToDraw_ = nullptr;

        graphicalModeInitialised_ = false;

    }

}