#include "lab_m1/tema2/tema2.h"
#include "lab_m1/tema2/tema2_transform2D.h"
#include "lab_m1/tema2/2dtObject.h"

#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <math.h>
#include <random>
#include <iostream>

using namespace std;
using namespace m1;

#define SPEED 2.0f
#define MAXDISTANCE 1.5


struct Entity
{
    GLfloat scaleX;
    GLfloat scaleY;
    GLfloat scaleZ;
    GLfloat posY;
    GLfloat posX;
    GLfloat posZ;
    GLfloat sizeX;
    GLfloat sizeY;
    GLfloat sizeZ;
    GLfloat angle;
    GLfloat health;
    GLfloat start;
    GLfloat anglefp;
    GLfloat direction; // 0-w; 1-a; 2-s; 3-d
    GLfloat distance;
    int render;
    GLfloat speed;
    glm::vec3 directionVector;
    glm::vec3 pos; 
};

void CreateRandomMaze(int n, int m, int translateX, int translateZ, int maze_exit_x, int maze_exit_z, vector<vector<int>>& maze);

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

float translateX, translateY, translateZ, mazeZ, mazeX;

void Tema2::Init()
{
    firstPerson = false;
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* healthBarEmpty = object2d::create_rectangle("healthBarEmpty", glm::vec3(0, 0, 0), 1, 5, glm::vec3(1, 0.09, 0.09));
        AddMeshToList(healthBarEmpty);
    }
    {
        Mesh* healthBar = object2d::create_rectangle("healthBar", glm::vec3(0, 0, 0), 1, 5, glm::vec3(0.5, 1, 0.5), true);
        AddMeshToList(healthBar);
    }
    {
        Mesh* timeBarEmpty = object2d::create_rectangle("timeBarEmpty", glm::vec3(0, 0, 0), 0.8, 5, glm::vec3(0.53, 0.12, 0.47));
        AddMeshToList(timeBarEmpty);
    }
    {
        Mesh* timeBar = object2d::create_rectangle("timeBar", glm::vec3(0, 0, 0), 0.8, 5, glm::vec3(0.737255, 0.560784, 0.560784), true);
        AddMeshToList(timeBar);
    }

    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);
    {
        Shader* shader = new Shader("Shader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        //Shader for enemy deformation
        Shader* shader = new Shader("DeathShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "NoiseVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    CreateRandomMaze(25, 15, translateX, translateZ, mazeX, mazeZ, maze_matrix);

    while (1) {
        translateX = rand() % 15;
        translateZ = rand() % 15;
        if (maze_matrix[translateX][translateZ] != 1 && (translateX != mazeX) && (translateZ != mazeZ)) {
            translateX += 0.5;
            translateZ += 0.5;
            break;
        }
    }

    for (int i = 0; i < maze_matrix.size(); i++) {
        for (int j = 0; j < maze_matrix[i].size(); j++) {
            if (maze_matrix[i][j] == 1) { //MAZE

                Entity block;
                block.posX = i;
                block.posZ = j;
                block.posY = 0;
                block.sizeX = 1;
                block.sizeY = 1;
                block.sizeZ = 1;
                mazeBlocks.push_back(block);
                mazeBlocksNo++;
            }
            else { //ENEMIES
                 if (i != (translateX - 0.5) && j != (translateZ - 0.5)) { 
                    int chance_to_render = rand() % 7;
                    if (chance_to_render == 0) {
                        Entity enemy;
                        enemy.posX = i + 0.2;
                        enemy.posZ = j + 0.1;
                        enemy.posY = 0.6;
                        enemy.sizeX = 0.2;
                        enemy.sizeY = 0.2;
                        enemy.sizeZ = 0.2;
                        enemy.direction = 2;
                        enemy.distance = 0;
                        enemy.pos = glm::vec3(enemy.posX, enemy.posY, enemy.posZ);
                        enemy.speed = 0.5;
                        enemy.health = 100;
                        enemy.render = 1;

                        enemies.push_back(enemy);
                        enemiesNo++;
                    }
                }
            }
        }
    }

    translateY = 0.6;

    camera = new m1::Camera();
    camera->Set(glm::vec3(translateX, translateY + 2.5, translateZ + 1.5f), glm::vec3(translateX, translateY + 0.5, translateZ), glm::vec3(0, 1, 0));

    player.sizeX = 0.6;
    player.sizeY = 0.8;
    player.sizeZ = 0.6;
    player.posX = translateX;
    player.posY = translateY;
    player.posZ = translateZ;
    player.angle = 0;
    player.health = 100;

    healthBar.scaleX = 1;
    healthBar.scaleY = 1;
    healthBar.sizeX = 1;
    healthBar.sizeY = 5;

    healthBarEmpty.scaleX = 1;
    healthBarEmpty.scaleY = 1;
    healthBarEmpty.sizeX = 1;
    healthBarEmpty.sizeY = 5;

    timeBar.scaleX = 1;
    timeBar.scaleY = 1;
    timeBar.sizeX = 0.8;
    timeBar.sizeY = 5;

    timeBarEmpty.scaleX = 1;
    timeBarEmpty.scaleY = 1;
    timeBarEmpty.sizeX = 0.8;
    timeBarEmpty.sizeY = 5;

    logicSpace.x = 0;
    logicSpace.y = 0;
    logicSpace.width = 16;
    logicSpace.height = 16;
}

void Tema2::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
    
    auto res = window->GetResolution();
    auto camera2D = GetSceneCamera();
    camera2D->SetPosition(glm::vec3(0, 0, 50));
    camera2D->SetRotation(glm::vec3(0, 0, 0));
    camera2D->Update();
    viewSpace = ViewportSpace(0, 0, res.x, res.y);
    GetCameraInput()->SetActive(false);
    SetViewportArea(viewSpace, glm::vec3(0.62f, 0.73f, 0.80f), true);
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

      modelMatrix2D = glm::mat3(1);
      modelMatrix2D = visMatrix * transform2D::Translate(logicSpace.width + 0.5, logicSpace.height - 1.2);
      RenderMesh2D(meshes["healthBarEmpty"], shaders["VertexColor"], modelMatrix2D);

       modelMatrix2D = glm::mat3(1);
       modelMatrix2D = visMatrix * transform2D::Translate(logicSpace.width + 0.5, logicSpace.height - 1.2);
       modelMatrix2D *= transform2D::Scale(healthBar.scaleX, healthBar.scaleY);
       RenderMesh2D(meshes["healthBar"], shaders["VertexColor"], modelMatrix2D);

       modelMatrix2D = glm::mat3(1);
       modelMatrix2D = visMatrix * transform2D::Translate(logicSpace.width + 0.5, logicSpace.height - 2.2);
       RenderMesh2D(meshes["timeBarEmpty"], shaders["VertexColor"], modelMatrix2D);

       modelMatrix2D = glm::mat3(1);
       modelMatrix2D = visMatrix * transform2D::Translate(logicSpace.width + 0.5, logicSpace.height - 2.2);
       modelMatrix2D *= transform2D::Scale(timeBar.scaleX, timeBar.scaleY);
       RenderMesh2D(meshes["timeBar"], shaders["VertexColor"], modelMatrix2D);

    GetCameraInput()->SetActive(true);

    timePassedProjectile += deltaTimeSeconds;
    timeLeft -= deltaTimeSeconds;
    timeBar.scaleX -= 0.007 * deltaTimeSeconds;

    if (player.health != 0 && timeLeft >= 0) {
        if ( (player.posX + 0.5) >= mazeX && (player.posZ  + 0.5) >= mazeZ) {
            cout << "YOU WON ! " << endl;
            exit(1);
        }
    }
    else {
        cout << "GAME OVER !" << endl;
        exit(1);
    }


    if (wasMousePressed) {
       RenderProjectile(deltaTimeSeconds);
    }
    RenderPlayer();
    RenderEnemies(deltaTimeSeconds);
    RenderMaze();
     //Render ground
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.5, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
        RenderSimpleMesh(meshes["plane"], shaders["Shader"], modelMatrix, glm::vec3(0.184314, 0.184314, 0.309804));
    }
}

void Tema2::FrameEnd()
{
    //DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}
void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}
void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    shader->Use();

    GLint object_clr = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(object_clr, 1, glm::value_ptr(color));

    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}
void Tema2::RenderAnimationMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, float animation_time)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;
    float r = rand() % (10 + 2) / 10.f;
    float g = rand() % (10 + 2) / 10.f;
    float b = rand() % (10 + 2) / 10.f;
    glm::vec3 color = glm::vec3(r, g, b);

    shader->Use();
    int model_location = glGetUniformLocation(shader->GetProgramID(), "Model");

    GLint object_clr = glGetUniformLocation(shader->program, "object_color");

    glUniform3fv(object_clr, 1, glm::value_ptr(color));

    glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    GLint location_View = glGetUniformLocation(shader->GetProgramID(), "View");

    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glUniformMatrix4fv(location_View, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    GLint location_Projection = glGetUniformLocation(shader->GetProgramID(), "Projection");

    glUniformMatrix4fv(location_Projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    GLint timeLocation = glGetUniformLocation(shader->GetProgramID(), "AnimationTime");
    glUniform1f(timeLocation, animation_time);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}
void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    bool collision = false;

    if (!firstPerson) {
        if (window->KeyHold(GLFW_KEY_W)) {   
           /**/ for (int i = 0; i < mazeBlocks.size(); i++) {
                if (player.posX + player.sizeX > mazeBlocks[i].posX &&
                    (mazeBlocks[i].posX + mazeBlocks[i].sizeX) > player.posX && (player.posY < (mazeBlocks[i].posY + mazeBlocks[i].sizeY) &&
                        (player.posY + player.sizeY) > mazeBlocks[i].posY)) {
                    collision = CheckCollision(player, player.posX, (player.posZ - deltaTime * SPEED - 0.2), mazeBlocks[i]);
                    if (collision) break;
                }
            }
            if (!collision) {
                    player.angle = 0;
                     camera->position.z -= deltaTime * SPEED;
                     translateZ -= deltaTime * SPEED;
            }
        }else if (window->KeyHold(GLFW_KEY_A)) {
            for (int i = 0; i < mazeBlocks.size(); i++) {
                if (player.posZ + player.sizeZ >= mazeBlocks[i].posZ &&
                    mazeBlocks[i].posZ + mazeBlocks[i].sizeZ >= player.posZ && (player.posY <= (mazeBlocks[i].posY + mazeBlocks[i].sizeY) &&
                        (player.posY + player.sizeY) >= mazeBlocks[i].posY)) {
                    collision = CheckCollision(player, (player.posX - deltaTime * SPEED - 0.25), player.posZ , mazeBlocks[i]);
                    if (collision) break;
                }
            }
            if (!collision) {
                player.angle = -90;
                camera->position.x -= deltaTime * SPEED;
                translateX -= deltaTime * SPEED;
            }
        } else if (window->KeyHold(GLFW_KEY_S)) {
           /**/   for (int i = 0; i < mazeBlocks.size(); i++) {
                if ((player.posX + player.sizeX) > mazeBlocks[i].posX &&
                    (mazeBlocks[i].posX + mazeBlocks[i].sizeX) > player.posX && (player.posY < (mazeBlocks[i].posY + mazeBlocks[i].sizeY) &&
                        (player.posY + player.sizeY) > mazeBlocks[i].posY)) {
                    collision = CheckCollision(player, player.posX, (player.posZ + deltaTime * SPEED ), mazeBlocks[i]);
                    if (collision) break;
                }
            }                                            

            /**/if (!collision) {
             player.angle = 180;
                camera->position.z += deltaTime * SPEED;
                translateZ += deltaTime * SPEED;
            }
               
            
        } else if (window->KeyHold(GLFW_KEY_D)) {
            for (int i = 0; i < mazeBlocks.size(); i++) {
                if ((player.posZ + player.sizeZ >= mazeBlocks[i].posZ &&
                    mazeBlocks[i].posZ + mazeBlocks[i].sizeZ >= player.posZ) && (player.posY <= (mazeBlocks[i].posY + mazeBlocks[i].sizeY) &&
                        (player.posY + player.sizeY) >= mazeBlocks[i].posY)) {
                    collision = CheckCollision(player, (player.posX + deltaTime * SPEED + 0.25), player.posZ, mazeBlocks[i]);
                    if (collision) break;
                }
            }
            if (!collision) {
                player.angle = 90;
                camera->position.x += deltaTime * SPEED;
                translateX += deltaTime * SPEED;
            }
            
        }
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
    if (window->KeyHold(GLFW_KEY_LEFT_CONTROL)) {
        if (!firstPerson) {
                if (player.angle == 0) { //forward
                    camera->Set(glm::vec3(translateX, translateY + 0.3, translateZ - 0.02), glm::vec3(translateX, translateY + 0.3, translateZ - 0.5),
                        glm::vec3(0, 1, 0));
                }else if (player.angle == 90) { //right
                    camera->Set(glm::vec3(translateX + 0.2, translateY + 0.3, translateZ), glm::vec3(translateX + 1, translateY + 0.3, translateZ),
                        glm::vec3(0, 1, 0));
                }else if (player.angle == 180) { //backwards
                    camera->Set(glm::vec3(translateX, translateY + 0.3, translateZ + 0.02), glm::vec3(translateX, translateY + 0.3, translateZ + 0.5),
                        glm::vec3(0, 1, 0));
                }
                else if (player.angle == -90) { //left
                    camera->Set(glm::vec3(translateX - 0.2, translateY + 0.3, translateZ), glm::vec3(translateX - 1, translateY + 0.3, translateZ),
                        glm::vec3(0, 1, 0));
                }
            firstPerson = true;
        }
        else {
            camera->Set(glm::vec3(translateX, translateY + 3, translateZ + 1.5), glm::vec3(translateX, translateY + 0.5, translateZ), glm::vec3(0, 1, 0));

            firstPerson = false;
        }
    }
}

void Tema2::OnKeyRelease(int key, int mods)
{
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)  {
    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && firstPerson) {
        camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
        camera->RotateFirstPerson_OY(sensivityOY * -deltaX);
    }
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && (timePassedProjectile > 0.5) && firstPerson/**/) {
        wasMousePressed = true;
        Entity proj;
        proj.directionVector = camera->forward;
        proj.posX = player.posX + player.sizeX/2;
        proj.posY = player.posY;
        proj.posZ = player.posZ + player.sizeZ / 2;
        proj.sizeX = 0.05;
        proj.sizeY = 0.05;   
        proj.sizeZ = 0.1;
        proj.render = 1;
        proj.distance = 0;
        proj.speed = 1;
        projectiles.push_back(proj);

        projectilesNo++;
        timePassedProjectile = 0;
    }
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}
void Tema2::RenderProjectile(GLfloat deltaTimeSeconds) {
    for (int i = 0; i < projectilesNo; i++) {
        if (projectiles[i].render) {

            projectileMatrix = glm::mat4(1);
            projectileMatrix = glm::translate(projectileMatrix, glm::vec3(projectiles[i].posX, projectiles[i].posY, projectiles[i].posZ));
            projectileMatrix = glm::scale(projectileMatrix, glm::vec3(0.05, 0.05, 0.1));
            RenderSimpleMesh(meshes["box"], shaders["Shader"], projectileMatrix, glm::vec3(0.752941, 0.752941, 0.752941));

            glm::vec3 nextMove = deltaTimeSeconds * projectiles[i].speed * projectiles[i].directionVector;
            glm::vec3 n2 = glm::vec3(projectiles[i].posX, projectiles[i].posY, projectiles[i].posZ);
            n2 += nextMove;
            projectiles[i].posX = n2.x;
            projectiles[i].posY = n2.y;
            projectiles[i].posZ = n2.z;

            projectiles[i].distance += projectiles[i].speed * deltaTimeSeconds;
            if (projectiles[i].distance > MAXDISTANCE) {
                projectiles[i].render = 0;
            }

            for (int j = 0; j < enemiesNo; j++) {
                if (CheckCollision(projectiles[i], projectiles[i].posX,
                    projectiles[i].posZ, enemies[j]) && (enemies[j].render)/**/) {
                    projectiles[i].render = false;
                    enemies[j].health -= 50 * deltaTimeSeconds;

                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[j].posX, enemies[j].posY, enemies[j].posZ));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2));
                    RenderAnimationMesh(meshes["sphere"], shaders["DeathShader"], modelMatrix, 2);
                    if (enemies[j].health < 0) {
                        enemies[j].render = false;
                    }
                    break;
                }
            }

            for (int j = 0; j < mazeBlocksNo; j++) {
                if (CheckCollision(projectiles[i], projectiles[i].posX,
                    projectiles[i].posZ, mazeBlocks[j])) {
                    projectiles[i].render = false;
                    break;
                }
            }/**/
        }
    }
}
void Tema2::RenderPlayer()
{
    player.posX = translateX;
    player.posY = translateY;
    player.posZ = translateZ;

    /* */ modelMatrix = glm::mat4(1);
      modelMatrix = playerMatrix * glm::translate(modelMatrix, glm::vec3(player.sizeX - 0.35, player.sizeY - 1.6, player.sizeZ - 0.45));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 0.6, 0.7));
      RenderSimpleMesh(meshes["box"], shaders["Shader"], modelMatrix, glm::vec3(0.560784, 0.737255, 0.560784));

      modelMatrix = glm::mat4(1); //LEFT LEG
      modelMatrix = playerMatrix * glm::translate(modelMatrix, glm::vec3(player.sizeX - 0.85, player.sizeY - 1.6, player.sizeZ - 0.45));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 0.6, 0.7));
      RenderSimpleMesh(meshes["box"], shaders["Shader"], modelMatrix, glm::vec3(0.560784, 0.737255, 0.560784));

      modelMatrix = glm::mat4(1); // RIGHT ARM
      modelMatrix = playerMatrix * glm::translate(modelMatrix, glm::vec3(player.sizeX + 0.1, player.sizeY - 0.7, player.sizeZ - 0.5));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35, 0.7, 0.7));
      RenderSimpleMesh(meshes["box"], shaders["Shader"], modelMatrix, glm::vec3(1, 0, 1));

      modelMatrix = glm::mat4(1); //LEFT ARM
      modelMatrix = playerMatrix * glm::translate(modelMatrix, glm::vec3(player.sizeX - 1.3, player.sizeY - 0.7, player.sizeZ - 0.5));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4, 0.7, 0.7));
      RenderSimpleMesh(meshes["box"], shaders["Shader"], modelMatrix, glm::vec3(1, 0, 1));

    playerMatrix = glm::mat4(1);//BODY
    playerMatrix = glm::translate(playerMatrix, glm::vec3(translateX, translateY, translateZ));
    playerMatrix = glm::scale(playerMatrix, glm::vec3(0.4, 0.4, 0.1));
    playerMatrix = glm::rotate(playerMatrix, RADIANS(player.angle), glm::vec3(0, 1, 0));
    RenderSimpleMesh(meshes["box"], shaders["Shader"], playerMatrix, glm::vec3(0.94, 0.81, 0.99));

    modelMatrix = glm::mat4(1); //HEAD
    modelMatrix = playerMatrix * glm::translate(modelMatrix, glm::vec3(player.sizeX - 0.6, player.sizeY - 0.1, player.sizeZ - 0.45));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7, 0.5, 0.7));
    RenderSimpleMesh(meshes["box"], shaders["Shader"], modelMatrix, glm::vec3(0.96, 0.80, 0.69));
}
void Tema2::RenderEnemies(GLfloat deltaTimeSeconds) {
    for (int i = 0; i < enemiesNo; i++) {
        if (enemies[i].render) {
            if (enemies[i].direction == 2) {
                if (enemies[i].distance < 4)/**/{ //goes down
                enemies[i].distance += 0.05;
                enemies[i].posZ += enemies[i].distance * enemies[i].speed * deltaTimeSeconds;
            }
                else {
                    enemies[i].distance = 0;
                    enemies[i].direction = 3;
                }
            
        }
            else   if (enemies[i].direction == 3)
            {
                if (enemies[i].distance < 4)
                { //goes right
              enemies[i].distance += 0.05;
              enemies[i].posX += enemies[i].distance * enemies[i].speed * deltaTimeSeconds;
                }
                else {
                    enemies[i].distance = 0;
                    enemies[i].direction = 0;
                }
            }
            //    
            else if (enemies[i].direction == 0) {
                if (enemies[i].distance < 4) { // goes up
                    enemies[i].distance += 0.05;
                    enemies[i].posZ -= enemies[i].distance * enemies[i].speed * deltaTimeSeconds;
                }
                else {
                    enemies[i].distance = 0;
                    enemies[i].direction = 1;
                }
            }

            else if (enemies[i].direction == 1)
            {
                if (enemies[i].distance < 4) { // goes left
                    enemies[i].distance += 0.05;
                    enemies[i].posX -= enemies[i].distance * enemies[i].speed * deltaTimeSeconds;
                }
                else {
                    enemies[i].distance = 0;
                    enemies[i].direction = 2;
                    enemies[i].posX = enemies[i].pos.x;
                    enemies[i].posY = enemies[i].pos.y;
                    enemies[i].posZ = enemies[i].pos.z;
                }
            }

            if (CheckCollision(player, player.posX, player.posZ, enemies[i]) ) {
                player.health -= 15 * deltaTimeSeconds;
                modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].posX, enemies[i].posY, enemies[i].posZ));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 0.2, 0.2));
                RenderAnimationMesh(meshes["sphere"], shaders["DeathShader"], modelMatrix, 2);

                healthBar.scaleX -= 0.06 * deltaTimeSeconds;

                if (healthBar.scaleX < 0) {
                    cout << "Game Over!" << endl;
                    exit(0);
                }
            }
            else {
                modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(enemies[i].posX, enemies[i].posY, enemies[i].posZ));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 0.2, 0.2));
                RenderSimpleMesh(meshes["box"], shaders["Shader"], modelMatrix, glm::vec3(1, 0.43, 0.78));
            }
        }
    }
}

GLboolean Tema2::CheckCollision(const Entity& one, int positionX, int positionZ, const Entity& two) {
    return (positionX  < (two.posX + two.sizeX) && (positionX + one.sizeX) > two.posX) &&
        (one.posY < (two.posY + two.sizeY) && (one.posY + one.sizeY) > two.posY) &&
        (positionZ < (two.posZ + two.sizeZ) && (positionZ + one.sizeZ) > two.posZ);
}

void Tema2::RenderMaze()
{
    for (int i = 0; i < mazeBlocksNo; i++) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(mazeBlocks[i].posX + 0.5, 0.5, mazeBlocks[i].posZ + 0.5));
        RenderSimpleMesh(meshes["box"], shaders["VertexNormal"], modelMatrix, glm::vec3(0.752941, 0.752941, 0.752941));
    }
}

int di[] = {0,1,0,-1};
int dj[] = {1,0,-1,0};
int di_next[] = {0,2,0,-2};
int dj_next[] = {2,0,-2,0};

bool isValidWall(float x, float y, float coming_x, float coming_y, vector<vector<int>> maze) {
    for (int i = 0; i < 4; i++) {
        if (x + di[i] != coming_x &&
            y + dj[i] != coming_y &&
            x + di[i] < maze.size() &&
            y + dj[i] < maze[i].size() &&
            x + di[i] >= 0 && y + dj[i] >= 0) {
            if (maze[x + di[i]][y + dj[i]] == 0) return false;
        }
    }
    return true;
}

bool isValidCell(int x, int y, vector<vector<int>> maze) {

    if (x >= 0 && x < maze.size() && y >= 0 && y < maze[x].size() && maze[x][y] == 1) return true;
    else return false;
}

void mazeAlgorithm(int x, int y, vector<vector<int>>& maze) {
    maze[x][y] = 0;

    vector<int> wayV;
    wayV.push_back(0);
    wayV.push_back(1);
    wayV.push_back(2);
    wayV.push_back(3);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(wayV.begin(), wayV.end(), default_random_engine(seed));


    for (int i = 0; i < wayV.size(); i++) {
        int way = wayV[i];

        if (isValidCell(x + di_next[way], y + dj_next[way], maze) && isValidWall(x + di[way], y + dj[way], x, y, maze)) {
            mazeAlgorithm(x + di[way], y + dj[way], maze);
        }
    }

}

void CreateRandomMaze(int n, int m, int translateX, int translateZ, int maze_exit_x, int maze_exit_z, vector<vector<int>>& maze) {
    maze = vector<vector<int>>(n, vector<int>(m, 1));
    translateX = rand() % 6 + 2;
    translateZ = rand() % 6 + 2;
    mazeAlgorithm(translateX, translateZ, maze);

    //setting exit
    for (int j = 0; j < maze[maze.size() - 1].size(); j++) {
        if (maze[maze.size() - 2][j] == 0) {
            maze[maze.size() - 1][j] = 0;
            mazeX = maze.size() - 1;
            mazeZ = j;
            break;
        }
    }

}

glm::mat3 Tema2::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

void Tema2::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}